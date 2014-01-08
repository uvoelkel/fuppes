/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "PreparedStatement.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "ObjectManager.h"

using namespace Sqlite;

#include <iostream>

PreparedStatement::PreparedStatement(Database::AbstractConnection& connection) :
        Database::AbstractPreparedStatement(connection)
{
    m_statement = NULL;
}

PreparedStatement::~PreparedStatement()
{
    sqlite3_finalize(m_statement); // safe to call on NULL
}

std::string PreparedStatement::getError()
{
    Sqlite::Connection* connection = (Sqlite::Connection*)&m_connection;
    return sqlite3_errmsg(connection->handle);
}


bool PreparedStatement::prepare(const std::string statement)
{
    Sqlite::Connection* connection = (Sqlite::Connection*)&m_connection;

    if(NULL != m_statement) {
        sqlite3_reset(m_statement);
        sqlite3_clear_bindings(m_statement);
    }

    int ret = sqlite3_prepare_v2(connection->handle, statement.c_str(), statement.length() + 1, &m_statement, NULL);

    //std::cout << "PREPARE: " << statement << " :: " << ret << std::endl;

    if(SQLITE_OK != ret) {
        std::cout << "PreparedStatement::prepare() :: ERROR_: " << getError() << std::endl;
        std::cout << "STATEMENT: #" << statement << " ##" << sqlite3_sql(m_statement) << "#" << std::endl;

        sqlite3_finalize(m_statement);
        m_statement = NULL;
        return false;
    }

    return true;
}

bool PreparedStatement::clear()
{
    sqlite3_reset(m_statement);
    return (SQLITE_OK == sqlite3_clear_bindings(m_statement));
}

bool PreparedStatement::bindNull(int index)
{
    return (SQLITE_OK == sqlite3_bind_null(m_statement, index + 1)); // sqlite uses 1 for the first value
}

bool PreparedStatement::bindText(int index, std::string value)
{
    return (SQLITE_OK == sqlite3_bind_text(m_statement, index + 1, value.c_str(), value.length(), SQLITE_TRANSIENT ));
}

bool PreparedStatement::bindInt64(int index, fuppes_int64_t value)
{
    return (SQLITE_OK == sqlite3_bind_int64(m_statement, index + 1, value));
}

bool PreparedStatement::execute()
{
    int ret = sqlite3_step(m_statement);
    if(SQLITE_DONE != ret) {
        std::cout << "PreparedStatement::execute() :: ERROR_: " << getError() << std::endl;
        std::cout << "STATEMENT: #" << sqlite3_sql(m_statement) << "#" << std::endl;
    }
    return (SQLITE_DONE == ret);
}

bool PreparedStatement::select(Database::AbstractQueryResult& result)
{
    int ret = SQLITE_BUSY;
    int columns = sqlite3_column_count(m_statement);
    Database::AbstractResultRow* row = NULL;

    while (true) {

        ret = sqlite3_step(m_statement);

        //std::cout << "SELECT: " << ret << std::endl;

        if(SQLITE_ROW == ret) {

            row = result.addRow();
            for(int i = 0; i < columns; i++) {
                switch (sqlite3_column_type(m_statement, i)) {

                    case SQLITE_INTEGER:
                        row->setColumnInt64(sqlite3_column_name(m_statement, i), sqlite3_column_int64(m_statement, i));
                        break;
                    case SQLITE_TEXT:
                        row->setColumnText(sqlite3_column_name(m_statement, i), (const char*)sqlite3_column_text(m_statement, i));
                        break;
                    case SQLITE_NULL:
                        row->setColumnNull(sqlite3_column_name(m_statement, i));
                        break;
                    case SQLITE_BLOB:
                    case SQLITE_FLOAT:
                        // fuppes does not use blobs or floats
                        break;
                }
            }

            continue;
        }

        break;
    }

    return (SQLITE_DONE == ret);
}

fuppes_int64_t PreparedStatement::getLastInsertId()
{
    Sqlite::Connection* connection = (Sqlite::Connection*)&m_connection;
    return sqlite3_last_insert_rowid(connection->handle);
}

bool PreparedStatement::next()
{
    int ret = sqlite3_step(m_statement);

    //std::cout << "NEXT: " << ret << std::endl;

    return (SQLITE_ROW == ret);
}

bool PreparedStatement::next(Database::AbstractItem& item)
{
    if(!this->next()) {
        return false;
    }
    return ObjectManager::assignItemValues(m_statement, item, true);
}


bool PreparedStatement::assign(Database::AbstractItem& item)
{
    return ObjectManager::assignItemValues(m_statement, item, true);
}
