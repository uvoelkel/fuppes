/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "QueryBuilder.h"

#include "ObjectManager.h"

#include <iostream>

using namespace Sqlite;

QueryBuilder::QueryBuilder(Database::AbstractConnection& connection) :
        Database::AbstractQueryBuilder(connection)
{

}

Database::AbstractQueryBuilder& QueryBuilder::select(Database::Tables from)
{
    m_stream.str("");
    m_stream.clear();

    switch (from) {
        case Database::Objects:
            m_stream << sqlSelectItemWithoutDetails;
            break;
        case Database::Details:
            break;
        case Database::ObjectDetails:
            m_stream << sqlSelectItemWithDetails;
            break;
    }

    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::select(std::string prefix, std::string field, Database::Tables from, Database::AbstractQueryBuilder::Operation op)
{
    //m_stream.str("");
    //m_stream.clear();

    m_stream << "select ";

    bool bracketOpen = false;

    if(Database::AbstractQueryBuilder::count == op) {
        m_stream << " count(";
        bracketOpen = true;
    }
    else if(Database::AbstractQueryBuilder::distinct == op) {
        m_stream << " distinct(";
        bracketOpen = true;
    }
    else if(Database::AbstractQueryBuilder::max == op) {
        m_stream << " max(";
        bracketOpen = true;
    }

    m_stream << prefix << "." << field;

    if(bracketOpen) {
        m_stream << ")";
    }

    if(Database::AbstractQueryBuilder::count == op) {
        m_stream << " as COUNT";
    }

    m_stream << " from ";

    switch (from) {
        case Database::Objects:
            m_stream << "OBJECTS " << prefix;
            break;
        case Database::Details:
            m_stream << "OBJECT_DETAILS " << prefix;
            break;
        case Database::ObjectDetails:
            m_stream << "OBJECTS " << prefix;
            m_stream << " left join OBJECT_DETAILS d on (d.ID = " << prefix << ".DETAIL_ID)";
            break;
    }

    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::delete_(Database::Tables from)
{
    m_stream << "delete from ";

    switch (from) {
        case Database::Objects:
            m_stream << "OBJECTS";
            break;
        case Database::Details:
            m_stream << "OBJECT_DETAILS";
            break;
        case Database::ObjectDetails:
            break;
    }

    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::where(std::string prefix, std::string field, std::string op, std::string value)
{
    m_stream << " where " << prefix << (prefix.empty() ? "" : ".") << field << " " << op << " " << value;
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::andWhere(std::string prefix, std::string field, std::string op, std::string value)
{
    m_stream << " and " << prefix << (prefix.empty() ? "" : ".") << field << " " << op << " " << value;
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::orWhere(std::string prefix, std::string field, std::string op, std::string value)
{
    m_stream << " or " << prefix << (prefix.empty() ? "" : ".") << field << " " << op << " " << value;
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::bracketOpen()
{
    m_stream << " ( ";
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::bracketClose()
{
    m_stream << " ) ";
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::orderBy(std::string fields)
{
    m_stream << " order by " << fields;
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::limit(const int offset, const int count)
{
    m_stream << " limit " << offset << ", " << count;
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::rawSql(std::string sql)
{
    m_stream << " " << sql;
    return *this;
}

bool QueryBuilder::getQuery(Database::AbstractPreparedStatement& preparedStatement)
{
    //std::cout << "Sqlite::QueryBuilder::getQuery" << std::endl << m_stream.str() << std::endl;
    std::string sql = m_stream.str();
    m_stream.str("");
    m_stream.clear();
    return preparedStatement.prepare(sql);
}
