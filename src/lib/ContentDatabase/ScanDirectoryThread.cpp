/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ScanDirectoryThread.h"

#include "../Common/Directory.h"
#include "../Database/ObjectManager.h"
#include "../Database/QueryBuilder.h"

#include "ContentDatabase.h"

using namespace ContentDatabase;

ScanDirectoryThread::ScanDirectoryThread(ContentDatabase& contentDatabase) :
        Thread::Thread("ContentDatabase::ScanDirectoryThread"),
        m_contentDatabase(contentDatabase)
{
    m_connection = NULL;
}

ScanDirectoryThread::~ScanDirectoryThread()
{
    if (NULL != m_connection) {
        m_connection->close();
        delete m_connection;
    }
}

int ScanDirectoryThread::checkDirectories()
{
    int count = 0;

    Database::QueryBuilder qb(*m_connection);
    qb.select(Database::Objects)
            .where("o", "TYPE", ">", "?")
            .andWhere("o", "TYPE", "<", "?")
            .andWhere("o", "DEVICE", "is", "NULL")
            .andWhere("o", "REF_ID", "=", "?")
            .andWhere("(o", "UPDATED_AT", "=", "?")
            .orWhere("o", "UPDATED_AT", "<", "o.MODIFIED_AT)");

    Database::PreparedStatement getDirectories(*m_connection);
    qb.getQuery(getDirectories);

    getDirectories.bindInt64(0, CONTAINER);
    getDirectories.bindInt64(1, CONTAINER_MAX);
    getDirectories.bindInt64(2, 0);
    getDirectories.bindInt64(3, 0);

    Database::ObjectManager objMgr(*m_connection);

    Database::Item parent;
    while (!stopRequested() && getDirectories.next(parent)) {

        fuppes::Directory directory(parent.path);
        directory.open();
        fuppes::DirEntryList entries = directory.dirEntryList();
        directory.close();

        for (unsigned int i = 0; i < entries.size() && !stopRequested(); i++) {

            if (fuppes::DirEntry::Directory == entries.at(i).type()) {
                m_contentDatabase.insertDirectory(entries.at(i).path(), entries.at(i).name(), parent.objectId, objMgr);
            }
            else if (fuppes::DirEntry::File == entries.at(i).type()) {
                m_contentDatabase.insertFile(entries.at(i).path(), entries.at(i).name(), parent.objectId, objMgr);
            }

        }

        if (!stopRequested()) {
            parent.updated = fuppes::DateTime::now().toInt();
            objMgr.persist(parent);
            count++;
        }
    }

    return count;
}

void ScanDirectoryThread::run()
{
    if (Database::Connection::getDefaultConnection()->getParameters().readonly) {
        return;
    }

    if (NULL == m_connection) {
        m_connection = Database::Connection::createConnection();
    }

    int count = 0;
    do {
        count = checkDirectories();
    }
    while (count > 0 && !stopRequested());

    m_contentDatabase.onScanDirectoryThreadFinished(stopRequested());
}
