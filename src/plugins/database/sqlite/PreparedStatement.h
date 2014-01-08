/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_PREPAREDSTATEMENT_H
#define _SQLITE_PREPAREDSTATEMENT_H

#include <fuppes_database_plugin.h>
#include "Connection.h"
#include <sqlite3.h>

namespace Sqlite
{
    class PreparedStatement: public Database::AbstractPreparedStatement
    {
        public:
            PreparedStatement(Database::AbstractConnection& connection);
            ~PreparedStatement();

            bool prepare(const std::string statement);
            bool clear();
            std::string getError();

            bool bindNull(int index);
            bool bindText(int index, std::string value);
            bool bindInt64(int index, fuppes_int64_t value);

            bool execute();
            bool select(Database::AbstractQueryResult& result);

            fuppes_int64_t getLastInsertId();

            /**
             * @return  boolean true on success. false on eof or error
             */
            bool next();
            bool next(Database::AbstractItem& item);

            bool assign(Database::AbstractItem& item);

            sqlite3_stmt* m_statement;
    };
}

#endif // _SQLITE_PREPAREDSTATEMENT_H
