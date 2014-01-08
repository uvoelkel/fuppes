/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "PreparedStatement.h"

using namespace Database;

PreparedStatement::PreparedStatement() :
        AbstractPreparedStatement(*Connection::getDefaultConnection())
{
    m_preparedStatement = m_connection.createPreparedStatement();
}

PreparedStatement::PreparedStatement(Database::Connection& connection) :
        AbstractPreparedStatement(connection)

{
    m_preparedStatement = m_connection.createPreparedStatement();
}

PreparedStatement::PreparedStatement(Database::Connection* connection) :
        AbstractPreparedStatement(NULL == connection ? *Connection::getDefaultConnection() : *connection)
{
    m_preparedStatement = m_connection.createPreparedStatement();
}

PreparedStatement::~PreparedStatement()
{
    if(NULL != m_preparedStatement) {
        delete m_preparedStatement;
    }
}

bool PreparedStatement::prepare(const std::string statement)
{
    return m_preparedStatement->prepare(statement);
}

bool PreparedStatement::clear()
{
    return m_preparedStatement->clear();
}

bool PreparedStatement::bindNull(int index)
{
    return m_preparedStatement->bindNull(index);
}

bool PreparedStatement::bindText(int index, std::string value)
{
    return m_preparedStatement->bindText(index, value);
}

bool PreparedStatement::bindInt64(int index, fuppes_int64_t value)
{
    return m_preparedStatement->bindInt64(index, value);
}

bool PreparedStatement::execute()
{
    return m_preparedStatement->execute();
}

bool PreparedStatement::select(Database::AbstractQueryResult& result)
{
    return m_preparedStatement->select(result);
}


bool PreparedStatement::next()
{
    return m_preparedStatement->next();
}

bool PreparedStatement::next(Database::AbstractItem& item)
{
    return m_preparedStatement->next(item);
}

bool PreparedStatement::assign(Database::AbstractItem& item)
{
    return m_preparedStatement->assign(item);
}
