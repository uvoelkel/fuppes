/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _CONTENTDATABASE_CONTENTDATABASE_H
#define _CONTENTDATABASE_CONTENTDATABASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Configuration.h"
#include "FileDetails.h"
#include "ScanDirectoryThread.h"
#include "UpdateThread.h"
#include "RemoveMissingThread.h"

#include "../Fam/Monitor.h"
#include "../Fam/EventHandler.h"

#include "../Database/ObjectManager.h"

namespace ContentDatabase
{
    class ContentDatabase: public Fam::EventHandler
    {
        public:

            enum Status
            {
                Idle,
                Stopped,

                RemoveMissing,
                ScanDirectories,
                UpdateMetadata
            };

            static object_id_t getObjectId();
            static uint32_t getSystemUpdateId();

            ContentDatabase(Configuration& configuration, CFileDetails& fileDetails, Plugin::Manager& pluginManager, VirtualContainer::Manager& virtualContainerMgr);
            ~ContentDatabase();

            void init();

            void insertDirectory(const std::string path, const std::string name, const object_id_t parentId, Database::ObjectManager& objMgr);
            void insertFile(const std::string path, const std::string name, const object_id_t parentId, Database::ObjectManager& objMgr);

            void onScanDirectoryThreadFinished(const bool stopped);
            void onUpdateThreadFinished(const bool stopped);
            void onRemoveMissingThreadFinished(const bool stopped);

            ContentDatabase::Status getStatus()
            {
                return m_status;
            }
            void rebuild();
            void update();

            //void onFamEvent(Fam::Event& event);

            void onDirectoryCreateEvent(const std::string path, const std::string name);
            void onDirectoryDeleteEvent(const std::string path);
            void onDirectoryMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name);

            void onFileCreateEvent(const std::string path, const std::string name);
            void onFileDeleteEvent(const std::string path, const std::string name);
            void onFileMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name);
            void onFileModifiedEvent(const std::string path, const std::string name);

        private:
            static object_id_t m_objectId;
            static uint32_t m_systemUpdateId;

            Configuration& m_configuration;
            CFileDetails& m_fileDetails;
            VirtualContainer::Manager& m_virtualContainerMgr;

            ScanDirectoryThread m_scanDirectoryThread;
            UpdateThread m_updateThread;
            RemoveMissingThread m_removeMissingThread;

            ContentDatabase::Status m_status;

            Fam::Monitor* m_fam;
    };

}

#endif // _CONTENTDATABASE_CONTENTDATABASE_H
