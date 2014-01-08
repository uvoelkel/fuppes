/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "SearchCriteria.h"

using namespace Sqlite;

#include <iostream>

const std::string SearchCriteria::getSearchCapabilities()
{
    // The property names returned MUST include the appropriate namespace prefixes, except for the DIDL-Lite namespace.
    // Properties in the DIDL-Lite namespace MUST always be returned without the prefix.
    return "@id,@parentID,@refID,dc:title,upnp:class,upnp:artist,upnp:genre,upnp:album,upnp:originalTrackNumber";
}

bool SearchCriteria::processExistsOp(const std::string property, const std::string value, std::string& result)
{
    //std::cout << "SearchCriteria::processExistsOp " << property << " - " << value << std::endl;

    std::string field;
    FieldType type;

    if(!getField(property, field, type)) {
        return false;
    }

    result = field + " ";
    switch (type) {
        case SearchCriteria::Text:
            result += (0 == value.compare("true")) ? "is not NULL" : "is NULL";
            break;
        case SearchCriteria::Number:
            result += (0 == value.compare("true")) ? "> 0" : "= 0";
            break;
    }

    return true;
}

bool SearchCriteria::processRelOp(const std::string property, const Database::SearchCriteriaRelOp op, const std::string value, std::string& result)
{
    //std::cout << "SearchCriteria::processRelOp " << property << " - " << op << " - " << value << std::endl;

    std::string field;
    FieldType type;

    if(!getField(property, field, type)) {
        return false;
    }

    switch (op) {
        case Database::eq: // =
            if(SearchCriteria::Text == type) {
                result = field + " = \"" + value + "\"";
            }
            else if(SearchCriteria::Number == type) {
                result = field + " = " + value + "";
            }
            break;
        case Database::neq: // !=
            if(SearchCriteria::Text == type) {
                result = field + " != \"" + value + "\"";
            }
            else if(SearchCriteria::Number == type) {
                result = field + " != " + value + "";
            }
            break;
        case Database::lt: // <
            if(SearchCriteria::Text == type) {
                return false;
            }
            else if(SearchCriteria::Number == type) {
                result = field + " < " + value + "";
            }
            break;
        case Database::lte: // <=
            if(SearchCriteria::Text == type) {
                return false;
            }
            else if(SearchCriteria::Number == type) {
                result = field + " <= " + value + "";
            }
            break;
        case Database::gt:  // >
            if(SearchCriteria::Text == type) {
                return false;
            }
            else if(SearchCriteria::Number == type) {
                result = field + " > " + value + "";
            }
            break;
        case Database::gte: // >=
            if(SearchCriteria::Text == type) {
                return false;
            }
            else if(SearchCriteria::Number == type) {
                result = field + " >= " + value + "";
            }
            break;
    }

    return true;
}

bool SearchCriteria::processStringOp(const std::string property, const Database::SearchCriteriaStringOp op, const std::string value, std::string& result)
{
    //std::cout << "SearchCriteria::processStringOp " << property << " - " << op << " - " << value << std::endl;

    std::string field;
    FieldType type;

    if(!getField(property, field, type)) {
        return false;
    }

    switch (op) {
        case Database::contains:
            result = field + " like \"%" + value + "%\"";
            break;
        case Database::doesNotContain:
            result = field + " not like \"%" + value + "%\"";
            break;
        case Database::startsWith:
            result = field + " like \"" + value + "%\"";
            break;
        case Database::derivedFrom:
            result = field + " between " + value;
            break;
    }

    return true;
}

bool SearchCriteria::getField(const std::string property, std::string& field, SearchCriteria::FieldType& type)
{
    if(0 == property.compare("@id")) {
        field = "o.OBJECT_ID";
        type = SearchCriteria::Text;
    }
    else if(0 == property.compare("@parentID")) {
        field = "o.PARENT_ID";
        type = SearchCriteria::Number;
    }
    else if(0 == property.compare("@refID")) {
        field = "o.REF_ID";
        type = SearchCriteria::Number;
    }
    else if(0 == property.compare("dc:title")) {
        field = "o.TITLE";
        type = SearchCriteria::Text;
    }
    else if(0 == property.compare("upnp:class")) {
        field = "o.TYPE";
        type = SearchCriteria::Number;
    }
    else if(0 == property.compare("upnp:artist")) {
        field = "d.AV_ARTIST";
        type = SearchCriteria::Text;
    }
    else if(0 == property.compare("upnp:genre")) {
        field = "d.AV_GENRE";
        type = SearchCriteria::Text;
    }
    else if(0 == property.compare("upnp:album")) {
        field = "d.AV_ALBUM";
        type = SearchCriteria::Text;
    }
    else if(0 == property.compare("upnp:originalTrackNumber")) {
        field = "d.A_TRACK_NUMBER";
        type = SearchCriteria::Number;
    }
    else {
        return false;
    }

    return true;
}
