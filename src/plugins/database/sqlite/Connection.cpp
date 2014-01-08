/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "Connection.h"
#include "TableManager.h"
#include "QueryBuilder.h"
#include "ObjectManager.h"
#include "SearchCriteria.h"
#include "SortCriteria.h"

using namespace Sqlite;

Connection::Connection()
{
    handle = NULL;
}

bool Connection::connect()
{
    if(NULL != handle) {
        return false;
    }

    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    if(m_parameters.readonly) {
        flags = SQLITE_OPEN_READONLY;
    }

    // connect
    if(SQLITE_OK != sqlite3_open_v2(m_parameters.filename.c_str(), &handle, flags, NULL)) {
        fprintf(stderr, "[sqlite] can't create/open database: %s :: %s\n", sqlite3_errmsg(handle), m_parameters.filename.c_str());
        return false;
    }

    sqlite3_busy_timeout(handle, 1000);
    sqlite3_exec(handle, "pragma temp_store = MEMORY", NULL, NULL, NULL);
    sqlite3_exec(handle, "pragma synchronous = OFF", NULL, NULL, NULL);
    sqlite3_exec(handle, "pragma journal_mode = OFF", NULL, NULL, NULL);

    return true;
}

void Connection::close()
{
    sqlite3_close(handle);
    handle = NULL;
}

Database::AbstractTableManager* Connection::createTableManager()
{
    return new Sqlite::TableManager(*this);
}

Database::AbstractQueryBuilder* Connection::createQueryBuilder()
{
    return new Sqlite::QueryBuilder(*this);
}

Database::AbstractPreparedStatement* Connection::createPreparedStatement()
{
    return new Sqlite::PreparedStatement(*this);
}

Database::AbstractObjectManager* Connection::createObjectManager()
{
    return new Sqlite::ObjectManager(*this);
}

Database::AbstractSearchCriteria* Connection::createSearchCriteria()
{
    return new Sqlite::SearchCriteria();
}

Database::AbstractSortCriteria* Connection::createSortCriteria()
{
    return new Sqlite::SortCriteria();
}

