/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ObjectManager.h"

#include "QueryResult.h"
#include "QueryBuilder.h"

using namespace Database;

ObjectManager::ObjectManager()
{
    m_connection = Connection::getDefaultConnection();
    m_objectManager = m_connection->createObjectManager();
}

ObjectManager::ObjectManager(Database::Connection* connection)
{
    if (NULL != m_connection) {
        m_connection = connection;
    }
    else {
        m_connection = Connection::getDefaultConnection();
    }
    m_objectManager = m_connection->createObjectManager();
}

ObjectManager::ObjectManager(Database::Connection& connection)
{
    m_connection = &connection;
    m_objectManager = m_connection->createObjectManager();
}

ObjectManager::~ObjectManager()
{
    if (NULL != m_objectManager) {
        delete m_objectManager;
    }
}

bool ObjectManager::getMaxObjectId(object_id_t& result)
{
    return m_objectManager->getMaxObjectId(result);
}

bool ObjectManager::getAllContainerPaths(Database::AbstractQueryResult& result)
{
    return m_objectManager->getAllContainerPaths(result);
}

bool ObjectManager::findOne(const object_id_t objectId, const std::string device, Database::Item& item, bool withDetails /*= true*/)
{
    return m_objectManager->findOne(objectId, device, item, withDetails);
}

bool ObjectManager::findOneByFilename(const std::string path, const std::string filename, Database::AbstractItem& item, bool withDetails)
{
    return m_objectManager->findOneByFilename(path, filename, item, withDetails);
}

fuppes_int64_t ObjectManager::getChildCount(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions)
{
    Database::QueryResult result;
    if (false == m_objectManager->countChildren(objectId, device, objectTypes, extensions, result)) {
        return -1;
    }
    return result.getRow(0)->getColumnInt64("COUNT");
}

bool ObjectManager::findChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, const std::string orderBy, const int offset, const int limit, Database::AbstractItem& item)
{
    return m_objectManager->findChildren(objectId, device, objectTypes, extensions, orderBy, offset, limit, item);
}

bool ObjectManager::nextChild(Database::AbstractItem& item)
{
    return m_objectManager->nextChild(item);
}

bool ObjectManager::persist(Database::AbstractItem& item)
{
    return m_objectManager->persist(item);
}

bool ObjectManager::persist(Database::AbstractItemDetails& details)
{
    return m_objectManager->persist(details);
}

bool ObjectManager::remove(Database::AbstractItem& item)
{
    Database::QueryBuilder qb(*m_connection);
    Database::PreparedStatement del(*m_connection);

    if (CONTAINER_MAX > item.type) {

        qb.delete_(Database::Details).where("", "ID", "in", "").bracketOpen().select("o", "DETAIL_ID", Database::Objects, Database::AbstractQueryBuilder::distinct).where("o", "PATH", "like", "?").bracketClose();
        qb.getQuery(del);
        del.bindText(0, item.path + "%");
        del.execute();

        qb.delete_(Database::Objects).where("", "PATH", "like", "?");
        qb.getQuery(del);
        del.bindText(0, item.path + "%");
        del.execute();
    }
    else if (ITEM >= item.type) {

        if (0 < item.detailId) {
            qb.delete_(Database::Details).where("", "ID", "=", "?");
            qb.getQuery(del);
            del.bindInt64(0, item.detailId);
            del.execute();
        }

        qb.delete_(Database::Objects).where("", "ID", "=", "?");
        qb.getQuery(del);
        del.bindInt64(0, item.objectId);
        del.execute();
    }

    return true;
}
