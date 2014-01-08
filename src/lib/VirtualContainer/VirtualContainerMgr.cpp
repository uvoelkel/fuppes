/*
 * This file is part of fuppes
 *
 * (c) 2007-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "VirtualContainerMgr.h"
#include "../Database/QueryBuilder.h"
#include "../Database/QueryResult.h"
#include "../Database/ObjectManager.h"

#include "../Common/Common.h"

#include <cassert>
#include <iostream>

using namespace VirtualContainer;

Manager::Manager(VirtualContainer::Configuration& configuration) :
        m_configuration(configuration)
{
    m_objectId = 1000;

    m_connection = NULL;

    m_getContainerPaths = NULL;
    m_getContainerObjectId = NULL;
    m_getContainer = NULL;
}

Manager::~Manager()
{
    if (NULL != m_getContainerPaths) {
        delete m_getContainerPaths;
    }

    if (NULL != m_getContainerObjectId) {
        delete m_getContainerObjectId;
    }

    if (NULL != m_getContainer) {
        delete m_getContainer;
    }
}

void Manager::setConnection(Database::Connection* connection)
{
    if (NULL != m_getContainerPaths) {
        delete m_getContainerPaths;
        m_getContainerPaths = NULL;
    }

    if (NULL != m_getContainerObjectId) {
        delete m_getContainerObjectId;
        m_getContainerObjectId = NULL;
    }

    if (NULL != m_getContainer) {
        delete m_getContainer;
        m_getContainer = NULL;
    }

    m_connection = connection;
    if (NULL == m_connection) {
        return;
    }

    Database::QueryBuilder qb(*m_connection);

    m_getContainerPaths = new Database::PreparedStatement(m_connection);
    qb.select("o", "VCONTAINER_PATH", Database::Objects, Database::AbstractQueryBuilder::distinct)
            .where("o", "VCONTAINER_PATH", "like", "?")
            .andWhere("o", "DEVICE", "=", "?");
    qb.getQuery(*m_getContainerPaths);

    m_getContainerObjectId = new Database::PreparedStatement(m_connection);
    qb.select("o", "OBJECT_ID", Database::Objects)
            .where("o", "VCONTAINER_TYPE", "=", "?")
            .andWhere("o", "VCONTAINER_PATH", "=", "?")
            .andWhere("o", "DEVICE", "=", "?");
    qb.getQuery(*m_getContainerObjectId);

    m_getContainer = new Database::PreparedStatement(m_connection);
    qb.select("o", "OBJECT_ID", Database::Objects)
            .where("o", "PARENT_ID", "=", "?")
            .andWhere("o", "VCONTAINER_TYPE", "=", "?")
            .andWhere("o", "VCONTAINER_PATH", "=", "?")
            .andWhere("o", "TITLE", "=", "?")
            .andWhere("o", "DEVICE", "=", "?");
    qb.getQuery(*m_getContainer);
}

void Manager::init()
{
    if (Database::Connection::getDefaultConnection()->getParameters().readonly) {
        return;
    }

    Database::QueryBuilder qb;
    qb.select("o", "OBJECT_ID", Database::Objects, Database::AbstractQueryBuilder::max).where("o", "DEVICE", "is not", "NULL");

    Database::PreparedStatement getObjectId;
    qb.getQuery(getObjectId);

    Database::QueryResult result;
    getObjectId.select(result);

    if (0 < result.getRow(0)->getColumnInt64("OBJECT_ID")) {
        m_objectId = result.getRow(0)->getColumnInt64("OBJECT_ID");
    }

}

void Manager::clear()
{
    Database::QueryBuilder qb(*m_connection);
    qb.delete_(Database::Objects).where("", "DEVICE", "is not", "NULL");

    Database::PreparedStatement qry(m_connection);
    qb.getQuery(qry);
    qry.execute();
}

fuppes_int64_t Manager::getObjectId()
{
    return ++m_objectId;
}

void Manager::rebuildContainerLayouts(const bool insertFiles)
{
    if (!m_configuration.enabled) {
        return;
    }

    this->clear();

    VirtualContainerIter iter;
    for (iter = m_configuration.virtualContainers.begin(); iter != m_configuration.virtualContainers.end(); iter++) {
        if (!iter->enabled) {
            continue;
        }

        this->buildContainerLayoutFromCfgFile(iter->name, iter->cfgfile);
    }

    if (insertFiles) {
        Database::QueryBuilder qb(*m_connection);
        qb.select(Database::ObjectDetails).where("o", "TYPE", ">=", "?").andWhere("o", "DEVICE", "is", "NULL");
        Database::PreparedStatement qry(m_connection);
        qb.getQuery(qry);
        qry.bindInt64(0, ITEM);

        Database::Item item;
        while (qry.next(item)) {
            insertFile(item);
        }
    }
}

void Manager::insertFile(Database::Item& item)
{
    if (!m_configuration.enabled) {
        return;
    }

    VirtualContainerIter iter;
    for (iter = m_configuration.virtualContainers.begin(); iter != m_configuration.virtualContainers.end(); iter++) {
        if (!iter->enabled) {
            continue;
        }
        this->insertFileForLayout(item, iter->name);
    }
}

void Manager::removeFile(Database::Item& item)
{
    if (!m_configuration.enabled) {
        return;
    }

    Database::QueryBuilder qb(m_connection);
    qb.delete_(Database::Objects).where("o", "DEVICE", "is not", "NULL").andWhere("o", "VREF_ID", "=", "?");

    Database::PreparedStatement qry(m_connection);
    qb.getQuery(qry);
    qry.bindInt64(0, item.vrefId);

    qry.execute();
}

void Manager::buildContainerLayoutFromCfgFile(const std::string layout, const std::string cfgfile)
{
    xmlDocPtr doc = xmlReadFile(cfgfile.c_str(), "UTF-8", XML_PARSE_NOBLANKS);
    if (NULL == doc) {
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (NULL == root) {
        xmlFreeDoc(doc);
        return;
    }

    // todo: check version

    xmlNodePtr child = xmlFirstElementChild(root);
    while (NULL != child) {
        this->processLayoutNode(child, 0, layout);
        child = xmlNextElementSibling(child);
    }

    xmlFreeDoc(doc);
}

void Manager::processLayoutNode(xmlNodePtr node, const object_id_t pid, const std::string layout)
{

    if (0 == std::string("vfolder").compare((char*)node->name)) {

        std::string path = createVirtualContainerPath(node);

        object_id_t objectId;
        xmlAttrPtr attr = xmlHasProp(node, BAD_CAST "id");
        if (NULL != attr) {
            objectId = strToOffT((char*)xmlGetProp(node, BAD_CAST "id"));
        }
        else {
            objectId = this->getObjectId();
        }

        std::string title;
        attr = xmlHasProp(node, BAD_CAST "name");
        assert(NULL != attr);
        title = (char*)attr->children->content;

        Database::Item folder;
        folder.objectId = objectId;
        folder.parentId = pid;
        folder.type = CONTAINER_STORAGEFOLDER;
        folder.title = title;
        folder.device = layout;
        folder.vcontainerType = VirtualContainer::Folder;
        folder.vcontainerPath = createVirtualContainerPath(node);

        Database::ObjectManager objMgr(m_connection);
        objMgr.persist(folder);

        xmlNodePtr child = xmlFirstElementChild(node);
        while (NULL != child) {
            this->processLayoutNode(child, objectId, layout);
            child = xmlNextElementSibling(child);
        }

        /*  if(node->AttributeAsUInt("id") > 0)
         folder.setObjectId(node->AttributeAsUInt("id"));
         else
         folder.setObjectId(m_parent->getId());
         folder.setParentId(pid);
         folder.setType(CONTAINER_STORAGEFOLDER);
         folder.setTitle(node->attribute("name"));
         folder.setDevice(layout);
         folder.setVirtualContainerType(DbObject::Folder);
         folder.setVirtualContainerPath(path);
         folder.save();


         for(int i = 0; i < node->ChildCount(); i++) {
         if(node->ChildNode(i)->type() != CXMLNode::ElementNode)
         continue;
         createLayout(node->ChildNode(i), folder.objectId() ,qry, layout);
         }
         */
    }

}

std::string Manager::createVirtualContainerPath(xmlNodePtr node)
{
    std::string result;

    if (0 == std::string("vfolder").compare((char*)node->name)) {
        result = "folder|";

        // get parent folders
        xmlNodePtr parent = node->parent;
        while (NULL != parent && (0 == std::string("vfolder").compare((char*)parent->name))) {
            result += "folder|";
            parent = parent->parent;
        }
    }

    bool isFinal;
    std::string nodeType;
    xmlNodePtr child = xmlFirstElementChild(node);
    while (NULL != child) {

        if (0 == std::string("vfolder").compare((char*)child->name)) {
            child = xmlNextElementSibling(child);
            continue;
        }

        nodeType = this->getNodeType(child, isFinal);
        if (isFinal) {
            result += (nodeType + "&");
        }
        else {
            result += nodeType + "|";
            result += this->createVirtualContainerPath(child);
        }

        child = xmlNextElementSibling(child);
    }

    if (result.length() > 2 && (result[result.length() - 1] == '|' || result[result.length() - 1] == '&')) {
        result = result.substr(0, result.length() - 1);
    }

    return result;
}

std::string Manager::getNodeType(xmlNodePtr node, bool &isFinal)
{
    isFinal = false;
    std::string name = (char*)node->name;
    xmlAttrPtr attr;

    if (0 == name.compare("vfolder")) {
        return "folder";
    }
    else if (0 == name.compare("vfolders")) {

        attr = xmlHasProp(node, BAD_CAST "property");
        assert(NULL != attr);

        std::string prop = (char*)attr->children->content;
        if (0 == prop.compare("genre")) {
            return "genre";
        }
        else if (0 == prop.compare("artist")) {
            return "artist";
        }
        else if (0 == prop.compare("album")) {
            return "album";
        }
        else if (0 == prop.compare("composer")) {
            return "composer";
        }

    }
    else if (0 == name.compare("split")) {
        return "split";
    }

    else if (0 == name.compare("items")) {

        attr = xmlHasProp(node, BAD_CAST "type");
        assert(NULL != attr);

        isFinal = true;
        std::string type = (char*)attr->children->content;

        if (0 == type.compare("audioItem")) {
            return "audioItem";
        }
        else if (0 == type.compare("imageItem")) {
            return "imageItem";
        }
        else if (0 == type.compare("videoItem")) {
            return "videoItem";
        }

    }

    else if (0 == name.compare("folders")) {

        attr = xmlHasProp(node, BAD_CAST "filter");
        assert(NULL != attr);

        isFinal = true;
        return (char*)attr->children->content;
    }

    return "";
}

void Manager::insertFileForLayout(Database::Item& item, const std::string layout)
{
    //std::cout << "Manager::insertFileForLayout " << item.path + item.filename << " ** " << layout << std::endl;

    std::string path;
    switch (item.type) {
        case ITEM_AUDIO_ITEM:
            case ITEM_AUDIO_ITEM_MUSIC_TRACK:
            path = "audioItem";
            break;
        case ITEM_IMAGE_ITEM:
            case ITEM_IMAGE_ITEM_PHOTO:
            path = "imageItem";
            break;
        case ITEM_VIDEO_ITEM:
            case ITEM_VIDEO_ITEM_MOVIE:
            path = "videoItem";
            break;
        default:
            return;
            break;
    }

    // get all items containing the item type in the vcontainer path
    m_getContainerPaths->clear();
    m_getContainerPaths->bindText(0, "%" + path + "%");
    m_getContainerPaths->bindText(1, layout);

    Database::QueryResult getObjectIdResult;

    Database::ObjectManager objectManager(m_connection);
    Database::ResultRow* row;
    fuppes::StringList parts;

    object_id_t pid;
    object_id_t refId = 0;

    Database::QueryResult getObjectsResult;
    m_getContainerPaths->select(getObjectsResult);

    for (size_t i = 0; i < getObjectsResult.size(); i++) {

        row = getObjectsResult.getRow(i);

        parts = fuppes::String::split(row->getColumnString("VCONTAINER_PATH"), "|");
        assert("folder" == parts.at(0));

        // get folder id
        m_getContainerObjectId->clear();
        m_getContainerObjectId->bindInt64(0, VirtualContainer::Folder);
        m_getContainerObjectId->bindText(1, row->getColumnString("VCONTAINER_PATH"));
        m_getContainerObjectId->bindText(2, layout);

        getObjectIdResult.clear();
        m_getContainerObjectId->select(getObjectIdResult);

        //std::cout << "PATH: " << row->getColumnString("VCONTAINER_PATH") << " COUNT: " << getObjectIdResult.size() << std::endl;

        assert(1 == getObjectIdResult.size());

        pid = getObjectIdResult.getRow(0)->getColumnInt64("OBJECT_ID");

        // loop through the parts of the path
        // except for the first item (folder) and the last (audioItem)
        for (unsigned int j = 1; j < parts.size() - 1; j++) {

            VirtualContainer::VirtualContainerType type = VirtualContainer::None;
            if (parts.at(j) == "split") {
                type = VirtualContainer::Split;
                pid = 0; //getSplitParent(pid, object, parts.at(j + 1), layout);
                assert(0 != pid);
                continue;
            }
            else if (parts.at(j) == "genre") {
                type = VirtualContainer::Genre;
            }
            else if (parts.at(j) == "artist") {
                type = VirtualContainer::Artist;
            }
            else if (parts.at(j) == "composer") {
                type = VirtualContainer::Composer;
            }
            else if (parts.at(j) == "album") {
                type = VirtualContainer::Album;
            }
            else if (parts.at(j) == "folder") {
                continue;
            }
            else {
                std::cout << "TODO handle path : " << parts.at(j) << "*" << std::endl;
                continue;
            }

            pid = createFolderIfNotExists(item, pid, type, row->getColumnString("VCONTAINER_PATH"), layout);
        }

        // contains(audioItem | videoItem | imageItem)
        if (parts.at(parts.size() - 1).substr(0, 8).compare("contains") == 0) {
            pid = 0; //createSharedDirFoldersIfNotExist(object, pid, paths.at(i), layout);
            if (pid == 0)
                return;
        }

        //std::cout << "insert item: " << item.title << " : " << item.id << std::endl;

        // insert the file
        Database::Item vitem;
        //std::cout << "ASSIGN" << std::endl;
        vitem = item;
        vitem.objectId = this->getObjectId();
        vitem.parentId = pid;
        vitem.device = layout;
        vitem.vrefId = item.objectId;
        vitem.vcontainerPath = row->getColumnString("VCONTAINER_PATH");
        vitem.refId = refId;

        //std::cout << "persist vitem: " << vitem.id << std::endl;

        objectManager.persist(vitem);

        // we take the object id of the first inserted item
        // and use it as ref_id for the next ones (if any)
        if (0 == refId) {
            refId = vitem.objectId;
        }
    }

}

object_id_t Manager::createFolderIfNotExists(Database::Item& item, object_id_t pid, VirtualContainer::VirtualContainerType type, const std::string path, const std::string layout)
{
    std::string title;
    object_type_t objType = CONTAINER_STORAGEFOLDER;
    switch (type) {
        case VirtualContainer::Genre:
            title = item.details->av_genre;
            objType = CONTAINER_GENRE_MUSICGENRE;  // video genre
            break;
        case VirtualContainer::Artist:
            title = item.details->av_artist;
            objType = CONTAINER_PERSON_MUSICARTIST; // video artist
            break;
        case VirtualContainer::Composer:
            title = item.details->a_composer;
            break;
        case VirtualContainer::Album:
            title = item.details->av_album;
            objType = CONTAINER_ALBUM_MUSICALBUM;  // image album
            break;
        default:
            ASSERT(true == false);
            break;
    }

    title = TrimWhiteSpace(title);
    if (title.length() == 0) {
        title = "unknown";
    }

    //std::cout << "createFolderIfNotExists " << title << " :: pid: " << pid << " path: " << path << std::endl;

    m_getContainer->clear();
    m_getContainer->bindInt64(0, pid);
    m_getContainer->bindInt64(1, type);
    m_getContainer->bindText(2, path);
    m_getContainer->bindText(3, title);
    m_getContainer->bindText(4, layout);

    Database::QueryResult result;
    m_getContainer->select(result);

    //std::cout << "SIZE: " << result.size() << std::endl;

    assert(0 == result.size() || 1 == result.size());

    if (1 == result.size()) {
        return result.getRow(0)->getColumnInt64("OBJECT_ID");
    }

    Database::Item folder;
    folder.objectId = this->getObjectId();
    folder.parentId = pid;
    folder.type = objType;
    folder.title = title;
    folder.vcontainerType = type;
    folder.vcontainerPath = path;
    folder.device = layout;

    Database::ObjectManager objMgr(m_connection);
    objMgr.persist(folder);

    //std::cout << "createFolderIfNotExists2" << folder.objectId << std::endl;

    /* todo set metadata for artist, composer or album folders
     switch(type) {
     case DbObject::Artist:
     break;
     case DbObject::Composer:
     break;
     case DbObject::Album:
     break;
     default:
     break;
     }*/

    /* details = *(object->details());
     details.save();

     folder.setDetailId(details.id());
     folder.save();*/

//cout << "createFolderIfNotExists" << endl << DbObject::toString(&folder) << endl;
    return folder.objectId;
}

