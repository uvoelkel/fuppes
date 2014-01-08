/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_CONNECTION_H
#define _DATABASE_CONNECTION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>

#include "../Plugin/Manager.h"

namespace Database
{
    class Connection: public AbstractConnection
    {
        friend class QueryBuilder;
        friend class TableManager;
        friend class ObjectManager;
        friend class PreparedStatement;
        friend class SearchCriteria;
        friend class SortCriteria;

        public:
            Connection(Plugin::Manager& pluginManager);
            ~Connection();
            bool connect();
            void close();

            static void setDefaultConnection(Database::Connection* defaultConnection);
            static Database::Connection* getDefaultConnection();
            static Database::Connection* createConnection();

        private:
            Database::AbstractTableManager* createTableManager();
            Database::AbstractQueryBuilder* createQueryBuilder();
            Database::AbstractPreparedStatement* createPreparedStatement();
            Database::AbstractObjectManager* createObjectManager();
            Database::AbstractSearchCriteria* createSearchCriteria();
            Database::AbstractSortCriteria* createSortCriteria();

            Plugin::Manager& m_pluginManager;
            AbstractConnection* m_connection;

            static Database::Connection* m_defaultConnection;

    };
}

#endif // _DATABASE_CONNECTION_H
