/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "SearchCriteria.h"

using namespace Database;

#include "../Common/RegEx.h"

#include <iostream>

const std::string sc_hTab = "\\x09"; // UTF-8 code 0x09, horizontal tab character
const std::string sc_lineFeed = "\\x0a"; // UTF-8 code 0x0A, line feed character
const std::string sc_vTab = "\\x0b"; // UTF-8 code 0x0B, vertical tab character
const std::string sc_formFeed = "\\x0c"; // UTF-8 code 0x0C, form feed character
const std::string sc_return = "\\x0d"; // UTF-8 code 0x0D, carriage return character
const std::string sc_space = "\\x20"; // UTF-8 code 0x20, space character
const std::string sc_dQuote = "\\x22"; // UTF-8 code 0x22, double quote character
const std::string sc_asterisk = "\\x2a"; // UTF-8 code 0x2A, asterisk character
const std::string sc_at = "\\x40"; // UTF-8 code 0x40, at character
const std::string sc_wChar = "[" + sc_space + "|" + sc_hTab + "|" + sc_lineFeed + "|" + sc_vTab + "|" + sc_formFeed + "|" + sc_return + "]";

const std::string sc_existsOp = "exists";
const std::string sc_boolVal = "true|false";
const std::string sc_logOp = "and|or";
const std::string sc_relOp = "=|!=|<|<=|>|>=";
// don't know if thats a typo or for some compatiblility reasons
// but the upnp spec actually defines "derivedFrom" and "derivedfrom"
const std::string sc_stringOp = "contains|doesNotContain|derivedfrom|startsWith|derivedFrom";
const std::string sc_binOp = sc_relOp + "|" + sc_stringOp;

const std::string sc_property = sc_at + "*[\\p{L}|\\p{N}|:]+"; // property name as defined in Section 2.2.20
const std::string sc_escapedQuote = ".*?[^\\\\]"; // double-quote escaped string as defined in Section 1.2.2

const std::string sc_quotedVal = sc_dQuote + sc_escapedQuote + sc_dQuote;

const std::string sc_bracketOpen = "\\(*" + sc_wChar + "*";
const std::string sc_bracketClose = sc_wChar + "*\\)*";

const std::string sc_relExp = "(" + sc_property + ")" + sc_wChar + "+([" + sc_binOp + "|" + sc_existsOp + "]+)" + sc_wChar + "+(" + sc_quotedVal + "|" + sc_boolVal + ")";

const std::string sc_searchExp = sc_bracketOpen + sc_relExp + sc_bracketClose; // + "[" + sc_wChar + "+(" + sc_logOp + ")" + sc_wChar + "+]*";

SearchCriteria::SearchCriteria()
{
    m_valid = false;
    m_searchCriteria = Database::Connection::getDefaultConnection()->createSearchCriteria();
}

SearchCriteria::~SearchCriteria()
{
    if(NULL != m_searchCriteria) {
        delete m_searchCriteria;
    }
}

bool SearchCriteria::parse(const std::string searchCriteria, std::string& sql)
{
	std::cout << "PARSE search criteria: " << searchCriteria << std::endl;

    RegEx rxSearch(sc_searchExp);
    bool ret = rxSearch.Search(searchCriteria.c_str());
    if(false == ret) {
        std::cout << "error parsing search criteria: " << std::endl << searchCriteria << std::endl;
        return false;
    }

    std::string prop;
    std::string op;
    std::string val;

    std::string sqlPart;
    size_t prevEnd = 0;

    m_valid = true;

    while (true == ret) {

        sqlPart = "";
        prop = rxSearch.match(1);
        op = rxSearch.match(2);
        val = rxSearch.match(3);

        // existsOp
        if(0 == op.compare("exists")) {

            // val has to be boolVal
            if(std::string::npos == sc_boolVal.find(val)) {
                m_valid = false;
                break;
            }

            if(false == m_searchCriteria->processExistsOp(prop, val, sqlPart)) {
                m_valid = false;
                break;
            }
        }
        // relOp
        else if(std::string::npos != sc_relOp.find(op)) {

            Database::SearchCriteriaRelOp relOp = eq;
            if(0 == op.compare("=")) {
                relOp = eq;
            }
            else if(0 == op.compare("!=")) {
                relOp = neq;
            }
            else if(0 == op.compare("<")) {
                relOp = lt;
            }
            else if(0 == op.compare("<=")) {
                relOp = lte;
            }
            else if(0 == op.compare(">")) {
                relOp = gt;
            }
            else if(0 == op.compare(">=")) {
                relOp = gte;
            }

            val = unescape(val);

            if(0 == prop.compare("upnp:class")) {
                if(eq != relOp) {
                    m_valid = false;
                    break;
                }
                val = replaceUpnpClass(val, false);
            }

            if(false == m_searchCriteria->processRelOp(prop, relOp, val, sqlPart)) {
                m_valid = false;
                break;
            }
        }
        // stringOp
        else if(std::string::npos != sc_stringOp.find(op)) {

            Database::SearchCriteriaStringOp stringOp = contains;
            if(0 == op.compare("contains")) {
                stringOp = contains;
            }
            else if(0 == op.compare("doesNotContain")) {
                stringOp = doesNotContain;
            }
            else if(0 == op.compare("derivedFrom") || 0 == op.compare("derivedfrom")) {
                stringOp = derivedFrom;
            }
            else if(0 == op.compare("startsWith")) {
                stringOp = startsWith;
            }

            val = unescape(val);

            // the only valid property for 'derivedFrom' is 'upnp:class' and the only valid string operator for 'upnp:class' is 'derivedFrom'
            if((0 != prop.compare("upnp:class") && derivedFrom == stringOp) || (0 == prop.compare("upnp:class") && derivedFrom != stringOp)) {
                m_valid = false;
                break;
            }

            if(0 == prop.compare("upnp:class")) {
                val = replaceUpnpClass(val, true);
            }

            if(false == m_searchCriteria->processStringOp(prop, stringOp, val, sqlPart)) {
                m_valid = false;
                break;
            }
        }
        else {
            m_valid = false;
            break;
        }

        // copy the gap between the end of the previous value
        // and the beginning of the current property
        sql += searchCriteria.substr(prevEnd, rxSearch.offset(1) - prevEnd);
        sql += sqlPart;

        prevEnd = rxSearch.offset(3) + rxSearch.length(3);

        /*std::cout << rxSearch.offset(0) << ": " << rxSearch.match(0) << std::endl;
        std::cout << rxSearch.offset(1) << ": " << rxSearch.match(1) << std::endl;
        std::cout << rxSearch.offset(2) << ": " << rxSearch.match(2) << std::endl;
        std::cout << rxSearch.offset(3) << ": " << rxSearch.match(3) << std::endl;
        //std::cout << rxSearch.offset(4) << ": " << rxSearch.match(4) << std::endl;
        std::cout << std::endl;*/

        ret = rxSearch.SearchAgain();
    }

    // append the rest of the criteria
    if(m_valid && prevEnd < searchCriteria.length()) {
        sql += searchCriteria.substr(prevEnd, searchCriteria.length() - prevEnd);
    }

    //std::cout << "SQL: " << std::endl << sql << std::endl;

    if(false == m_valid) {
        std::cout << "error parsing search criteria: " << std::endl << searchCriteria << std::endl;
    }

    return m_valid;
}

bool SearchCriteria::isValid()
{
    return m_valid;
}

const std::string SearchCriteria::getSearchCapabilities() // static
{
    Database::AbstractSearchCriteria* searchCriteria = Database::Connection::getDefaultConnection()->createSearchCriteria();
    std::string result = searchCriteria->getSearchCapabilities();
    delete searchCriteria;
    return result;
}

std::string SearchCriteria::unescape(const std::string escapedQuote)
{
    std::string result = escapedQuote.substr(1, escapedQuote.length() - 2); // trim leading and trailing "

    return result;
}

std::string SearchCriteria::replaceUpnpClass(const std::string value, const bool derived)
{
    std::stringstream result;

    for(int i = 0; true; i++) {

        upnp_object_type_t type = upnp_object_types[i];
        if(0 == type.str) {
            break;
        }

        if(0 == value.compare(type.str)) {

            if(!derived) {
                result << type.type;
            }
            else {
                result << type.type << " and " << type.max;
            }

            break;
        }
    }

    return result.str();
}

/*
 searchCrit      ::= searchExp|asterisk
 searchExp       ::= relExp|searchExp wChar+ logOp wChar+ searchExp|'(' wChar* searchExp wChar* ')'
 logOp           ::= 'and'|'or'
 relExp          ::= property wChar+ binOp wChar+ quotedVal|property wChar+ existsOp wChar+ boolVal
 binOp           ::= relOp|stringOp
 relOp           ::= '='|'!='|'<'|'<='|'>'|'>='
 stringOp        ::= 'contains'|'doesNotContain'|'derivedfrom'|'startsWith'|'derivedFrom'
 existsOp        ::= 'exists'
 boolVal         ::= 'true'|'false'
 quotedVal       ::= dQuote escapedQuote dQuote
 wChar           ::= space|hTab|lineFeed|vTab|formFeed|return
 property        ::= (* property name as defined in Section 2.2.20 *)
 escapedQuote    ::= (* double-quote escaped string as defined in Section 1.2.2 *)
 hTab            ::= (* UTF-8 code 0x09, horizontal tab character *)
 lineFeed        ::= (* UTF-8 code 0x0A, line feed character *)
 vTab            ::= (* UTF-8 code 0x0B, vertical tab character *)
 formFeed        ::= (* UTF-8 code 0x0C, form feed character *)
 return          ::= (* UTF-8 code 0x0D, carriage return character *)
 space           ::= ' ' (* UTF-8 code 0x20, space character *)
 dQuote          ::= '"' (* UTF-8 code 0x22, double quote character *)
 asterisk        ::= '*' (* UTF-8 code 0x2A, asterisk character *)
 */
