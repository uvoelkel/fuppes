/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Connection.h"

using namespace Database;

#include <iostream>

Database::Connection* Connection::m_defaultConnection = NULL;

void Connection::setDefaultConnection(Database::Connection* defaultConnection) // static
{
    m_defaultConnection = defaultConnection;
}

Database::Connection* Connection::getDefaultConnection() // static
{
    return m_defaultConnection;
}

Database::Connection* Connection::createConnection() // static
{
    if(NULL == m_defaultConnection) {
        return NULL;
    }

    Database::Connection* connection = new Database::Connection(m_defaultConnection->m_pluginManager);
    connection->setParameters(m_defaultConnection->getParameters());
    if(false == connection->connect()) {
        delete connection;
        connection = NULL;
    }
    return connection;
}

Connection::Connection(Plugin::Manager& pluginManager) :
        m_pluginManager(pluginManager)
{
    m_connection = NULL;
}

Connection::~Connection()
{
    if(NULL != m_connection) {
        delete m_connection;
    }
}

bool Connection::connect()
{
    Plugin::Database* plugin = m_pluginManager.getDatabasePlugin(m_parameters.type);
    if(!plugin) {
        std::cout << "db plugin not found" << std::endl;
        return false;
    }

    m_connection = plugin->createConnection();
    if(NULL == m_connection) {
        std::cout << "error creating connection" << std::endl;
        return false;
    }

    m_connection->setParameters(m_parameters);
    if(false == m_connection->connect()) {

        std::cout << "error connecting" << std::endl;

        delete m_connection;
        m_connection = NULL;
        return false;
    }

    return true;
}

void Connection::close()
{
    if(NULL == m_connection) {
        return;
    }
    m_connection->close();
}

AbstractTableManager* Connection::createTableManager()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createTableManager();
}

/*
Database::AbstractQuery* Connection::createQuery()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createQuery();
}
*/

Database::AbstractQueryBuilder* Connection::createQueryBuilder()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createQueryBuilder();
}

Database::AbstractPreparedStatement* Connection::createPreparedStatement()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createPreparedStatement();
}

AbstractObjectManager* Connection::createObjectManager()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createObjectManager();
}

Database::AbstractSearchCriteria* Connection::createSearchCriteria()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createSearchCriteria();
}

Database::AbstractSortCriteria* Connection::createSortCriteria()
{
    if(NULL == m_connection) {
        return NULL;
    }
    return m_connection->createSortCriteria();
}
