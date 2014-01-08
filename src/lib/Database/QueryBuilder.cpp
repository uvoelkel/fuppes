/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fuppes_database_plugin.h>
#include "QueryBuilder.h"

using namespace Database;

QueryBuilder::QueryBuilder() :
        Database::AbstractQueryBuilder(*Connection::getDefaultConnection())
{
    m_queryBuilder = m_connection.createQueryBuilder();
}

QueryBuilder::QueryBuilder(Database::Connection* connection) :
	Database::AbstractQueryBuilder(*connection)
{
	m_queryBuilder = m_connection.createQueryBuilder();
}

QueryBuilder::QueryBuilder(Database::Connection& connection) :
        Database::AbstractQueryBuilder(connection)
{
    m_queryBuilder = m_connection.createQueryBuilder();
}

QueryBuilder::~QueryBuilder()
{
    if(NULL != m_queryBuilder) {
        delete m_queryBuilder;
    }
}

AbstractQueryBuilder& QueryBuilder::select(Database::Tables from)
{
    m_queryBuilder->select(from);
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::select(std::string prefix, std::string field, Database::Tables from, Database::AbstractQueryBuilder::Operation op /*= Database::AbstractQueryBuilder::all*/)
{
    m_queryBuilder->select(prefix, field, from, op);
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::delete_(Database::Tables from)
{
    m_queryBuilder->delete_(from);
    return *this;
}

AbstractQueryBuilder& QueryBuilder::where(std::string prefix, std::string field, std::string op, std::string value)
{
    m_queryBuilder->where(prefix, field, op, value);
    return *this;
}
AbstractQueryBuilder& QueryBuilder::andWhere(std::string prefix, std::string field, std::string op, std::string value)
{
    m_queryBuilder->andWhere(prefix, field, op, value);
    return *this;
}
AbstractQueryBuilder& QueryBuilder::orWhere(std::string prefix, std::string field, std::string op, std::string value)
{
    m_queryBuilder->orWhere(prefix, field, op, value);
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::bracketOpen()
{
    m_queryBuilder->bracketOpen();
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::bracketClose()
{
    m_queryBuilder->bracketClose();
    return *this;
}

AbstractQueryBuilder& QueryBuilder::orderBy(std::string fields)
{
    m_queryBuilder->orderBy(fields);
    return *this;
}

Database::AbstractQueryBuilder& QueryBuilder::limit(const int offset, const int count)
{
    m_queryBuilder->limit(offset, count);
    return *this;
}

AbstractQueryBuilder& QueryBuilder::rawSql(std::string sql)
{
    m_queryBuilder->rawSql(sql);
    return *this;
}

bool QueryBuilder::getQuery(AbstractPreparedStatement& preparedStatement)
{
    return m_queryBuilder->getQuery(preparedStatement);
}
