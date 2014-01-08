/*
 * This file is part of fuppes
 *
 * (c) 2007-2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _VIRTUALCONTAINER_MANAGER_H
#define _VIRTUALCONTAINER_MANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Configuration.h"
//#include "VirtualContainerDatabase.h"

#include "../Database/Connection.h"
#include "../Database/Item.h"
#include "../Database/PreparedStatement.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

namespace VirtualContainer
{
    enum VirtualContainerType
    {
        None = 0,
        Folder = 1 << 0,
        Split = 1 << 1,
        Filter = 1 << 2,
        Genre = 1 << 3,
        Artist = 1 << 4,
        Composer = 1 << 5,
        Album = 1 << 6,
        SharedDir = 1 << 7
    };

    class Manager
    {
        public:
            Manager(VirtualContainer::Configuration& configuration);
            ~Manager();
            void init();
            void clear();

            void setConnection(Database::Connection* connection);

            void rebuildContainerLayouts(const bool insertFiles);
            void insertFile(Database::Item& item);
            void removeFile(Database::Item& item);

        private:
            VirtualContainer::Configuration& m_configuration;

            void buildContainerLayoutFromCfgFile(const std::string layout, const std::string cfgfile);
            void processLayoutNode(xmlNodePtr node, const object_id_t pid, const std::string layout);

            std::string createVirtualContainerPath(xmlNodePtr node);
            std::string getNodeType(xmlNodePtr node, bool &isFinal);

            void insertFileForLayout(Database::Item& item, const std::string layout);
            object_id_t createFolderIfNotExists(Database::Item& item, object_id_t pid, VirtualContainer::VirtualContainerType type, const std::string path, const std::string layout);

            fuppes_int64_t getObjectId();
            fuppes_int64_t m_objectId;

            Database::Connection*           m_connection;

            Database::PreparedStatement*    m_getContainerPaths;
            Database::PreparedStatement*    m_getContainerObjectId;
            Database::PreparedStatement*    m_getContainer;
    };
}

#endif // _VIRTUALCONTAINER_MANAGER_H
