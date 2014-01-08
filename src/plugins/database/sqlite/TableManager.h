/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_TABLEMANAGER_H
#define _SQLITE_TABLEMANAGER_H

#include <fuppes_database_plugin.h>
#include "Connection.h"
#include <sqlite3.h>

namespace Sqlite
{
    class TableManager: public Database::AbstractTableManager
    {
        public:
            TableManager(Database::AbstractConnection& connection);
            ~TableManager();

            bool exist();
            bool isVersion(int version);
            bool create(int version);
            bool empty();
    };
}

#endif // _SQLITE_TABLEMANAGER_H
