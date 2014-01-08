/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "TableManager.h"
#include "Connection.h"

using namespace Database;

TableManager::TableManager():
        AbstractTableManager(*Connection::getDefaultConnection())
{
    m_tableManager = m_connection.createTableManager();
}

TableManager::~TableManager()
{
    if(NULL != m_tableManager) {
        delete m_tableManager;
    }
}

bool TableManager::exist()
{
    return m_tableManager->exist();
}

bool TableManager::isVersion(int version)
{
    return m_tableManager->isVersion(version);
}

bool TableManager::create(int version)
{
    return m_tableManager->create(version);
}

bool TableManager::empty()
{
    return m_tableManager->empty();
}
