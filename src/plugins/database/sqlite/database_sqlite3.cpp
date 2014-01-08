/*
 * This file is part of fuppes
 *
 * (c) 2009-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fuppes_database_plugin.h>

#include <sqlite3.h>
#include "Connection.h"

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptDatabase;
    strcpy(plugin->name, "sqlite3");
}

extern "C" Database::AbstractConnection* fuppes_create_db_connection(void** data) // __attribute__((unused))
{
    return new Sqlite::Connection();
}

extern "C" void fuppes_unregister_plugin()
{
}

