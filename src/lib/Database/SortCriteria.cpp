/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "SortCriteria.h"

using namespace Database;

#include <iostream>

SortCriteria::SortCriteria()
{
    m_valid = false;
    m_sortCriteria = Database::Connection::getDefaultConnection()->createSortCriteria();
}

SortCriteria::~SortCriteria()
{
    if(NULL != m_sortCriteria) {
        delete m_sortCriteria;
    }
}

bool SortCriteria::parse(const std::string sortCriteria, std::string& sql)
{

    //std::cout << "SortCriteria::parse: " << sortCriteria << std::endl;

    m_valid = true;
    m_originalValue = sortCriteria;

    std::string criteria = sortCriteria;
    // set a default sort criteria
    if(0 == criteria.length()) {
        criteria = "+dc:title";
    }

    // append a "," for easier splitting
    criteria += ",";

    // UPnP ContentDir 1.3.1
    // white space before, after, or interior to any numeric data type is not allowed.
    // white space before, after, or interior to any other data type is part of the value.

    // split the string (e.g. +upnp:artist,-dc:date,+dc:title)
    std::string::size_type offset = 0;
    std::string::size_type pos;

    std::string part;
    std::string direction;
    std::string sqlPart;

    while(std::string::npos != (pos = criteria.find(",", offset))) {
        part = criteria.substr(offset, pos - offset);
        offset = pos + 1;

        direction = part.substr(0, 1);
        part = part.substr(1);
        sqlPart = "";

        if(false == m_sortCriteria->processField(direction, part, sqlPart)) {
            m_valid = false;
            break;
        }

        sql += sqlPart + ",";
    }

    // remove trailing ","
    if(m_valid && ',' == sql[sql.length() - 1]) {
        sql = sql.substr(0, sql.length() - 1);
    }

    return m_valid;
}

bool SortCriteria::isValid()
{
    return m_valid;
}

const std::string SortCriteria::getSortCapabilities() // static
{
    Database::AbstractSortCriteria* sortCriteria = Database::Connection::getDefaultConnection()->createSortCriteria();
    std::string result = sortCriteria->getSortCapabilities();
    delete sortCriteria;
    return result;
}

const std::string SortCriteria::getSortExtensionCapabilities() // static
{
    Database::AbstractSortCriteria* sortCriteria = Database::Connection::getDefaultConnection()->createSortCriteria();
    std::string result = sortCriteria->getSortExtensionCapabilities();
    delete sortCriteria;
    return result;
}
