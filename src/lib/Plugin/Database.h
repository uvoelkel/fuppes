/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _PLUGIN_DATABASE_H
#define _PLUGIN_DATABASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>

#include "Plugin.h"

namespace Plugin
{
    typedef Database::AbstractConnection* (*createConnection_t)(void** data);

    class Database: public Plugin
    {
        public:
            Database();
            bool init(LibraryHandle handle);

            ::Database::AbstractConnection* createConnection();
        private:
            createConnection_t m_createConnection;
    };
}

#endif // _PLUGIN_DATABASE_H
