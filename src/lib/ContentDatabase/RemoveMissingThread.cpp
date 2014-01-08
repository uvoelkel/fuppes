/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "RemoveMissingThread.h"

#include "ContentDatabase.h"
#include "../Database/QueryBuilder.h"

using namespace ContentDatabase;

RemoveMissingThread::RemoveMissingThread(ContentDatabase& contentDatabase) :
        Thread("contentdatabase remove missing"),

        m_contentDatabase(contentDatabase)
{
    m_connection = NULL;
}

RemoveMissingThread::~RemoveMissingThread()
{
    if (NULL != m_connection) {
        m_connection->close();
        delete m_connection;
    }
}

void RemoveMissingThread::run()
{
    if (Database::Connection::getDefaultConnection()->getParameters().readonly) {
        return;
    }

    if (NULL == m_connection) {
        m_connection = Database::Connection::createConnection();
    }


    Database::QueryBuilder qb(*m_connection);
    qb.select(Database::Objects).where("o", "DEVICE", "is", "NULL");

    Database::PreparedStatement getItems(*m_connection);
    qb.getQuery(getItems);

    Database::Item item;
    while (getItems.next(item)) {

        if (CONTAINER <= item.type && CONTAINER_MAX > item.type) {

            if (!fuppes::Directory::exists(item.path)) {

            }

        }
        else if (ITEM <= item.type && ITEM_MAX > item.type) {

            if (!fuppes::File::exists(item.path + item.filename)) {

            }

        }
    }

    m_contentDatabase.onRemoveMissingThreadFinished(stopRequested());
}

