/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            ContentDatabase.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _CONTENTDATABASE_H
#define _CONTENTDATABASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <map>
#include <list>
#include <stdint.h>
#include "../Common/Common.h"
#include "../Thread/Thread.h"
#include "FileAlterationMonitor.h"
#include "FileAlterationHandler.h"
//#include "UpdateThread.h"

#include "../Configuration/FuppesConfig.h"
#include "FileDetails.h"
#include "../Fam/Monitor.h"
#include "../Fam/EventHandler.h"
#include "../Database/ObjectManager.h"
#include "../VirtualContainer/VirtualContainerMgr.h"


class CContentDatabase;

class RebuildThread: public Threading::Thread
{
    public:

        enum RebuildType
        {
            rebuild = 1, addNew = 2, removeMissing = 4
        };

        RebuildThread(CContentDatabase* contentDb, int delay = 0) :
                Thread::Thread("db rebuild thread")
        {
            m_contentDb = contentDb;
            m_rebuildType = rebuild;
            m_delay = delay;
        }

        ~RebuildThread()
        {
            close();
        }

        void setRebuildType(int type)
        {
            m_rebuildType = type;
        }

    private:
        void run();

        void DbScanDir(CContentDatabase* db, Database::ObjectManager& objMgr, std::string p_sDirectory, object_id_t p_nParentId);
        object_id_t InsertFile(CContentDatabase* pDb, Database::ObjectManager& objMgr, object_id_t p_nParentId, std::string p_sFileName, bool hidden = false);

        CContentDatabase* m_contentDb;
        int m_rebuildType;
        int m_delay;
};

class ScanDirectoryThread: public Threading::Thread
{
    public:
        ScanDirectoryThread(CContentDatabase* contentDb, std::string path) :
                Threading::Thread("ScanDirectoryThread" + path)
        {
            m_contentDb = contentDb;
            m_path = path;
        }

    private:
        void run();
        void scanDir(Database::ObjectManager& objMgr, std::string path, unsigned int parentId);

        CContentDatabase* m_contentDb;
        std::string m_path;
};


class CContentDatabase//: public Fam::EventHandler
{
        friend class RebuildThread;

    public:
        //static CContentDatabase* Shared();
        CContentDatabase(Fuppes::FuppesConfig& fuppesConfig, CFileDetails& fileDetails, Plugins::CPluginMgr& pluginMgr, VirtualContainer::Manager& virtualContainerMgr);
        ~CContentDatabase();

        bool Init();

        void RebuildDB(int delay = 0);
        void UpdateDB();
        void AddNew();
        void RemoveMissing();
        bool IsRebuilding();

        static object_id_t GetObjId();

        CFileAlterationMonitor* fileAlterationMonitor()
        {
            return m_pFileAlterationMonitor;
        }
        CFileDetails* getFileDetails()
        {
            return &m_fileDetails;
        }
        VirtualContainer::Manager* getVirtualContainerMgr()
        {
            return &m_virtualContainerMgr;
        }

        Fuppes::FuppesConfig* getFuppesConfig()
        {
            return &m_fuppesConfig;
        }

        /*void deleteObject(unsigned int objectId);
         void deleteContainer(std::string path);    */

        object_id_t insertFile(std::string fileName, object_id_t parentId, Database::ObjectManager& objMgr, bool lock = true);
        object_id_t insertDirectory(std::string path, std::string title, object_id_t parentId, Database::ObjectManager& objMgr, bool lock = true);

        object_id_t insert_file_helper(
                  std::string file_name,
                  object_id_t parent_id,
                  Database::ObjectManager& objMgr);
        
        object_id_t insert_directory_helper(
                  std::string directory_path,
                  std::string title,
                  object_id_t parent_id,
                  Database::ObjectManager& objMgr);

        void scanDirectory(std::string path);

        static uint32_t getSystemUpdateId();
        static void incSystemUpdateId();

        void startUpdateThread();

        void moveDirectory(std::string oldpath, std::string newparent, std::string newdir);
        void removeDirectory(std::string path);

        void addSharedObject(Configuration::SharedObject* so);
        void removeSharedObject(Configuration::SharedObject* so);

        /* export all objects in path to fileName and remove from local db*/
        bool exportData(std::string fileName, std::string path, bool remove);
        /* import all objects from fileName with path */
        bool importData(std::string fileName, std::string path);

        void updateOther(Configuration::SharedObject* so);

        /*void onFamEvent(Fam::Event& event);

        void onDirectoryCreateEvent(const std::string path, const std::string name);
        void onDirectoryRemoveEvent(Fam::Event& event);
        void onDirectoryMoveEvent(Fam::Event& event);*/

    private:
        void BuildDB(int rebuildType, int delay = 0);

        //CDatabase& m_database;
        Fuppes::FuppesConfig& m_fuppesConfig;
        CFileDetails& m_fileDetails;
        Plugins::CPluginMgr& m_pluginMgr;
        VirtualContainer::Manager& m_virtualContainerMgr;

        RebuildThread* m_rebuildThread;
        //fuppes::UpdateThread m_updateThread;

        //static CContentDatabase* m_Instance;

        CFileAlterationMonitor* m_pFileAlterationMonitor;
        FileAlterationHandler m_fileAlterationHandler;

        Fam::Monitor* m_fileAlterationMonitor;

        static object_id_t m_objectId;
        static uint32_t m_systemUpdateId;

        //std::list<ScanDirectoryThread*>     m_scanDirThreadList;
        Threading::Mutex m_insertMutex;
};

#endif // _CONTENTDATABASE_H
