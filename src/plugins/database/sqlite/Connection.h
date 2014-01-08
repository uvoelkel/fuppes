/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_CONNECTION_H
#define _SQLITE_CONNECTION_H

#include <fuppes_database_plugin.h>

#include <sqlite3.h>

namespace Sqlite
{
    class Connection: public Database::AbstractConnection
    {
        public:
            Connection();

            bool connect();
            void close();

            Database::AbstractQueryBuilder* createQueryBuilder();
            Database::AbstractTableManager* createTableManager();
            Database::AbstractObjectManager* createObjectManager();
            Database::AbstractPreparedStatement* createPreparedStatement();
            Database::AbstractSearchCriteria* createSearchCriteria();
            Database::AbstractSortCriteria* createSortCriteria();

            sqlite3* handle;
    };
}

#endif // _SQLITE_CONNECTION_H
