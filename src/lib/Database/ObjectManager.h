/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _DATABASE_OBJECTMANAGER_H
#define _DATABASE_OBJECTMANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_database_plugin.h>

#include "Connection.h"
#include "Item.h"
#include "QueryResult.h"

namespace Database
{
    class ObjectManager
    {
        public:
            ObjectManager();
            ObjectManager(Database::Connection* connection);
            ObjectManager(Database::Connection& connection);
            ~ObjectManager();

            bool getMaxObjectId(object_id_t& result);
            bool getAllContainerPaths(Database::AbstractQueryResult& result);

            bool findOne(const object_id_t objectId, const std::string device, Database::Item& item, bool withDetails = true);
            bool findOneByFilename(const std::string path, const std::string filename, Database::AbstractItem& item, bool withDetails);

            fuppes_int64_t getChildCount(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions);
            bool findChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, const std::string orderBy, const int offset, const int limit, Database::AbstractItem& item);
            bool nextChild(Database::AbstractItem& item);

            bool persist(Database::AbstractItem& item);
            bool persist(Database::AbstractItemDetails& details);


            bool remove(Database::AbstractItem& item);
        private:
            Database::Connection* m_connection;
            AbstractObjectManager* m_objectManager;
    };
}

#endif // _DATABASE_OBJECTMANAGER_H
