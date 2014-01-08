/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            ContentDatabase.cpp
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





#include "ContentDatabase.h"
#include "../ContentDatabase/ContentDatabase.h"
#include "../SharedLog.h"
#include "FileDetails.h"
#include "../Common/RegEx.h"
#include "../Common/Common.h"
#include "../Common/Directory.h"
#include "../Common/File.h"
#include "HotPlug.h"
#include "../Plugins/PluginMgr.h"

//#include "DatabaseObject.h"
#include "../Thread/ThreadPool.h"



#include <sstream>
#include <string>
#include <fstream>
#include <cstdio>
#ifndef WIN32
#include <dirent.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
//#include <iostream>

#include "../Plugins/ItemSource.h"

using namespace std;
using namespace fuppes;
using namespace Threading;

//static bool g_bIsRebuilding;
/*static bool g_bFullRebuild;
 static bool g_bAddNew;
 static bool g_bRemoveMissing;
 */

object_id_t CContentDatabase::m_objectId = 0;
uint32_t CContentDatabase::m_systemUpdateId = 0;

/*
 CContentDatabase* CContentDatabase::m_Instance = 0;

 CContentDatabase* CContentDatabase::Shared()
 {
 if(m_Instance == 0) {
 m_Instance = new CContentDatabase();
 }
 return m_Instance;
 }
 */

CContentDatabase::CContentDatabase(Fuppes::FuppesConfig& fuppesConfig, CFileDetails& fileDetails, Plugins::CPluginMgr& pluginMgr, VirtualContainer::Manager& virtualContainerMgr):
        m_fuppesConfig(fuppesConfig),
        m_fileDetails(fileDetails),
        m_pluginMgr(pluginMgr),
        m_virtualContainerMgr(virtualContainerMgr),

        //m_updateThread(this, fuppesConfig, fileDetails, pluginMgr, virtualContainerMgr, m_fileAlterationHandler),
        m_fileAlterationHandler(this)
{
    m_rebuildThread = NULL;
    m_objectId = 0;
    m_systemUpdateId = 0;
    //m_fileAlterationHandler = new FileAlterationHandler();
    m_pFileAlterationMonitor = CFileAlterationMgr::CreateMonitor(&m_fileAlterationHandler);
    m_fileAlterationHandler.setMonitor(m_pFileAlterationMonitor);
    //m_updateThread = new UpdateThread(&m_fileAlterationHandler);

    m_fileAlterationMonitor = NULL;

    //HotPlugMgr::init();
}

CContentDatabase::~CContentDatabase()
{
    //HotPlugMgr::uninit();

    if (NULL != m_fileAlterationMonitor) {
        m_fileAlterationMonitor->stop();
        m_fileAlterationMonitor->close();
        delete m_fileAlterationMonitor;
    }

    //delete m_updateThread;
    delete m_pFileAlterationMonitor;
    //delete m_fileAlterationHandler;

    if(m_rebuildThread != NULL) {
        delete m_rebuildThread;
        m_rebuildThread = NULL;
    }
}

bool CContentDatabase::Init()
{


    //return true;


    //SQLQuery qry;
    /*  string sql = qry.build(SQL_TABLES_EXIST, 0);
    qry.select(sql);
    if(qry.eof()) {
        *p_bIsNewDB = true;

        // create tables
        sql = qry.build(SQL_CREATE_TABLE_DB_INFO, 0);
        qry.exec(sql);

        sql = qry.build(SQL_CREATE_TABLE_OBJECTS, 0);
        qry.exec(sql);

        sql = qry.build(SQL_CREATE_TABLE_OBJECT_DETAILS, 0);
        qry.exec(sql);

*/

        // create indices
//#warning "todo"
        /*StringList indices = String::split(qry.connection()->getStatement(SQL_CREATE_INDICES), ";");
        for(unsigned int i = 0; i < indices.size(); i++) {
            qry.exec(indices.at(i));
        }*/
/*
        // set db version
        sql = qry.build(SQL_SET_DB_INFO, DB_VERSION);
        qry.exec(sql);
    }

*/

    // this is mysql only code
    /*
     qry.select("SHOW VARIABLES LIKE  'char%'");
     while(!qry.eof()) {
     cout << qry.result()->asString("Variable_name") << " :: " << qry.result()->asString("Value") << endl;
     qry.next();
     }
     */

    /*
    qry.select("select VERSION from FUPPES_DB_INFO");
    if(qry.eof()) {
        CSharedLog::Shared()->UserError("no database version information found. remove the fuppes.db and restart fuppes");
        return false;
    }

    if(qry.result()->asInt("VERSION") != DB_VERSION) {
        CSharedLog::Shared()->UserError("database version mismatch. remove the fuppes.db and restart fuppes");
        return false;
    }
*/

    m_fileAlterationMonitor = NULL; //Fam::Monitor::createMonitor(*this, true); // m_fuppesConfig.databaseSettings.readonly);

    if(true == m_fuppesConfig.databaseSettings.readonly) {
        return true;
    }

    // get max object id
    Database::ObjectManager objMgr;
    objMgr.getMaxObjectId(CContentDatabase::m_objectId);






        // setup file alteration monitor
        //if(m_pFileAlterationMonitor->isActive()) {

            Database::QueryResult result;
            objMgr.getAllContainerPaths(result);

            for(size_t i = 0; i < result.size(); i++) {
               // m_pFileAlterationMonitor->addWatch(result.getRow(i)->getColumnString("PATH"));
                //m_fileAlterationMonitor->addWatch(result.getRow(i)->getColumnString("PATH"), true);
            }
       // }

        //m_fileAlterationMonitor->start();

        //m_updateThread.start();

        /*
        SQLQuery qry;

        // get max object id
        qry.select("select max(OBJECT_ID) as VALUE from OBJECTS where DEVICE is NULL");
        if(!qry.eof()) {
            CContentDatabase::m_objectId = qry.result()->asUInt("VALUE");
        }

        // setup file alteration monitor
        if(m_pFileAlterationMonitor->isActive()) {
            qry.select("select PATH from OBJECTS where TYPE >= 1 and TYPE < 100 and DEVICE is NULL");
            while (!qry.eof()) {
                m_pFileAlterationMonitor->addWatch(qry.result()->asString("PATH"));
                qry.next();
            }
        }
        */

        // start update thread
        //if(!*p_bIsNewDB)
        //    m_updateThread.start();


    return true;
}

uint32_t CContentDatabase::getSystemUpdateId() // static
{
    return CContentDatabase::m_systemUpdateId;
}

void CContentDatabase::incSystemUpdateId() // static
{
    CContentDatabase::m_systemUpdateId++;
    // todo: execute "Service Reset Procedure" if m_systemUpdateId reaches it's max value
}

void CContentDatabase::startUpdateThread()
{
    /*if(!m_updateThread.running()) {
        if(m_updateThread.finished()) {
            m_updateThread.close();
        }
        m_updateThread.start();
    }*/
}

/*
void CContentDatabase::onFamEvent(Fam::Event& event)
{
    std::cout << "CContentDatabase::onFamEvent" << std::endl;
}

void CContentDatabase::onDirectoryCreateEvent(const std::string path, const std::string name)
{
    std::cout << "CContentDatabase::onDirectoryCreateEvent" << std::endl;

    Database::Item parent;

    Database::ObjectManager objMgr;
    if (!objMgr.findOneByFilename(path, "", parent, false)) {
        return;
    }

    // if the parent is already marked as modified we can ignore this event
    if (parent.modified > parent.updated) {
        return;
    }


    Database::Item dir;
    dir.objectId = CContentDatabase::GetObjId();
    dir.parentId = parent.objectId;
    dir.type = CONTAINER_STORAGEFOLDER;
    dir.path = path + name + "/";
    dir.title = name;
    dir.modified = DateTime::now().toInt();
    dir.updated = 0;

    objMgr.persist(dir);

    m_fileAlterationMonitor->addWatch(dir.path);
}
void CContentDatabase::onDirectoryRemoveEvent(Fam::Event& event)
{
    std::cout << "ON EVENT" << std::endl;
}
void CContentDatabase::onDirectoryMoveEvent(Fam::Event& event)
{
    std::cout << "ON EVENT" << std::endl;
}
*/

void CContentDatabase::moveDirectory(std::string oldpath, std::string newparent, std::string newdir) // static
{
#warning todo
    /*
    DbObject* obj = DbObject::createFromFileName(oldpath);
    if(!obj) {
        cout << "CContentDatabase error: directory: " << oldpath << " not found 1" << endl;
        return;
    }

    // get the new parent
    DbObject* parent = DbObject::createFromFileName(newparent);
    if(!parent) {
        cout << "CContentDatabase directory: " << newparent << " not found" << endl;
        delete obj;
        return;
    }

    string newPath = Directory::appendTrailingSlash(newparent + newdir);

    obj->setPath(newPath);
    obj->setTitle(newdir);
    obj->setParentId(parent->objectId());
    obj->save();

    delete obj;
    delete parent;
    */

    // increment systemUpdateId
    CContentDatabase::incSystemUpdateId();
}

void CContentDatabase::removeDirectory(std::string path) // static
{
#warning todo
    /*
    DbObject* dir = DbObject::createFromFileName(path);
    if(!dir) {
        cout << "CContentDatabase: directory: " << path << " not found 2" << endl;
        return;
    }

    // remove all virtual files that are build from the directories content
    //m_virtualContainerMgr.deleteDirectory(dir);

    // remove the dir
    dir->remove();
    delete dir;
    */

    // increment systemUpdateId
    CContentDatabase::incSystemUpdateId();
}

/*
 void BuildPlaylists();
 void ParsePlaylist(CSQLResult* pResult);
 void ParseM3UPlaylist(CSQLResult* pResult);
 void ParsePLSPlaylist(CSQLResult* pResult);
 */

std::string findAlbumArtFile(std::string dir, CFileDetails* fileDetails);

//unsigned int InsertFile(CContentDatabase* pDb, SQLQuery* qry, unsigned int p_nParentId, std::string p_sFileName, bool hidden = false);

object_id_t GetObjectIDFromFileName(Database::ObjectManager& objMgr, std::string p_sFileName);

void CContentDatabase::BuildDB(int rebuildType, int delay /*= 0*/)
{
    if(IsRebuilding())
        return;

    if(m_rebuildThread) {
        delete m_rebuildThread;
        m_rebuildThread = NULL;
    }

   // m_updateThread.stop();

    m_rebuildThread = new RebuildThread(this, delay);
    m_rebuildThread->setRebuildType(rebuildType);
    m_rebuildThread->start();
}

void CContentDatabase::RebuildDB(int delay /*= 0*/)
{
    if(IsRebuilding() || m_fuppesConfig.databaseSettings.readonly)
        return;

    BuildDB(RebuildThread::rebuild, delay);
}

void CContentDatabase::UpdateDB()
{
    if(IsRebuilding() || m_fuppesConfig.databaseSettings.readonly)
        return;

    /*SQLQuery qry; // = CDatabase::query();
    qry.select("select max(OBJECT_ID) as VALUE from OBJECTS where DEVICE is NULL");
    if(!qry.eof()) {
        CContentDatabase::m_objectId = qry.result()->asUInt("VALUE");
    }*/
    //delete qry;

    BuildDB(RebuildThread::addNew | RebuildThread::removeMissing);
}

void CContentDatabase::AddNew()
{
    if(IsRebuilding() || m_fuppesConfig.databaseSettings.readonly)
        return;

    /*SQLQuery qry; // = CDatabase::query();
    qry.select("select max(OBJECT_ID) as VALUE from OBJECTS where DEVICE is NULL");
    if(!qry.eof()) {
        CContentDatabase::m_objectId = qry.result()->asUInt("VALUE");
    }*/
    //delete qry;

    BuildDB(RebuildThread::addNew);
}

void CContentDatabase::RemoveMissing()
{
    if(IsRebuilding() || m_fuppesConfig.databaseSettings.readonly)
        return;

    BuildDB(RebuildThread::removeMissing);
}

bool CContentDatabase::IsRebuilding()
{
    return (m_rebuildThread && m_rebuildThread->running());
}

object_id_t CContentDatabase::GetObjId() // static
{
    return ++CContentDatabase::m_objectId;
}

object_id_t CContentDatabase::insertFile(std::string fileName, object_id_t parentId /*= 0*/, Database::ObjectManager& objMgr /*= NULL*/, bool lock /*= false*/)
{
    if(lock) {
        MutexLocker locker(&m_insertMutex);
        return insert_file_helper(fileName, parentId, objMgr);
    } else {
        return insert_file_helper(fileName, parentId, objMgr);
    }
}

object_id_t CContentDatabase::insert_file_helper(
          std::string fileName,
          object_id_t parentId,
          Database::ObjectManager& objMgr)
{
    // split path and filename
    string path = File(fileName).path();
    fileName = fileName.substr(path.length(), fileName.length() - path.length());


    Database::Item item;
    objMgr.findOneByFilename(path, fileName, item, false);

    //DbObject* file = DbObject::createFromFileName(fileName, qry);
    if(0 < item.objectId) {
        return item.objectId;
        //object_id_t result = file->objectId();
        //delete file;
        //return result;
    }

    // get the object type
    OBJECT_TYPE objectType = m_fileDetails.GetObjectType(fileName);
    if(objectType == OBJECT_TYPE_UNKNOWN) {
        std::cout << "unknown object type: " << fileName << std::endl;
        return 0;
    }

    bool visible = !m_fileDetails.isAlbumArtFile(fileName);


    // get the parent object
    if(parentId == 0) {
        Database::Item parent;
        //DbObject* parent = DbObject::createFromFileName(path, qry);
        objMgr.findOneByFilename(path, "", parent, false);
        if(0 == parent.objectId) {
            cout << "CContentDatabase error: directory: " << path << " not found 3 for file: " << fileName << endl;
            assert(true == false);
            return 0;
        }
        parentId = parent.objectId; //parent->objectId();
        //delete parent;
    }

    // format title
    string title = fuppes::FormatHelper::fileNameToTitle(fileName, m_fuppesConfig.globalSettings.localCharset);

    // create the object
    item.objectId = CContentDatabase::GetObjId();
    item.parentId = parentId;
    item.type = objectType;
    item.path = path;
    item.filename = fileName;
    item.extension = ExtractFileExt(fileName);
    item.title = title;
    item.visible = visible;
    item.size = 0; //getFileSize(path + fileName);
    objMgr.persist(item);

    /*
    DbObject obj;
    obj.setParentId(parentId);
    obj.setType(objectType);
    obj.setPath(path);
    obj.setFileName(fileName);
    obj.setTitle(title);
    obj.setVisible(visible);
    obj.save(qry);
    */

    return item.objectId;
}

object_id_t CContentDatabase::insertDirectory(std::string path, std::string title, object_id_t parentId, Database::ObjectManager& objMgr /*= NULL*/, bool lock /*= false*/)
{
    if(lock) {
        MutexLocker locker(&m_insertMutex);
        return insert_directory_helper(path, title, parentId, objMgr);
    } else {
        return insert_directory_helper(path, title, parentId, objMgr);
    }
}

object_id_t CContentDatabase::insert_directory_helper(
          std::string directory_path,
          std::string title,
          object_id_t parent_id,
          Database::ObjectManager& objMgr)
{
    Database::Item dir;
    objMgr.findOneByFilename(directory_path, "", dir, false);

    //DbObject* dir = DbObject::createFromFileName(path, qry);
    if(0 == dir.objectId) {

        dir.objectId = CContentDatabase::GetObjId();
        dir.parentId = parent_id;
        dir.type = CONTAINER_STORAGEFOLDER;
        dir.path = directory_path;
        dir.title = title;

        objMgr.persist(dir);

        /*
        dir = new DbObject();
        dir->setParentId(parent_id);
        dir->setType(CONTAINER_STORAGEFOLDER);
        dir->setPath(path);
        dir->setTitle(title);
        dir->save(qry);
        */

        m_pFileAlterationMonitor->addWatch(directory_path);
    }

    return dir.objectId;
}

void CContentDatabase::addSharedObject(Configuration::SharedObject* so)
{
}

void CContentDatabase::removeSharedObject(Configuration::SharedObject* so)
{
}

void CContentDatabase::scanDirectory(std::string path)
{
    ScanDirectoryThread* thread = new ScanDirectoryThread(this, path);
    Threading::ThreadPool::deleteLater(thread);
    thread->start();

    /*std::list<ScanDirectoryThread*>::iterator iter;
     for(iter = m_Instance->m_scanDirThreadList.begin();
     iter != m_Instance->m_scanDirThreadList.end();
     ) {

     if((*iter)->finished()) {
     iter = m_Instance->m_scanDirThreadList.erase(iter);
     delete *iter;
     }
     else {
     iter++;
     }
     }

     m_Instance->m_scanDirThreadList.push_back(thread);
     */
}

void ScanDirectoryThread::run()
{
    Database::ObjectManager objMgr;

    Database::Item parent;
    objMgr.findOneByFilename(m_path, "", parent, false);


    //DbObject* parent = DbObject::createFromFileName(m_path);

    //SQLQuery qry;
    scanDir(objMgr, m_path, parent.objectId);
    //delete parent;
}

void ScanDirectoryThread::scanDir(Database::ObjectManager& objMgr, std::string path, unsigned int parentId)
{
    Directory dir(path);
    dir.open();
    DirEntryList entries = dir.dirEntryList();
    dir.close();

    DirEntry entry;
    unsigned int objectId = 0;
    for(unsigned int i = 0; i < entries.size(); i++) {
        entry = entries.at(i);

        if(entry.type() == DirEntry::Directory) {
            //CContentDatabase::insertFile(fileName, qry);
            objectId = m_contentDb->insertDirectory(entry.absolutePath(), entry.name(), parentId, objMgr, true);
            scanDir(objMgr, entry.absolutePath(), objectId);
        }
        else if(entry.type() == DirEntry::File) {
            m_contentDb->insertFile(entry.absolutePath(), parentId, objMgr, true);
        }

    }

}

void RebuildThread::DbScanDir(CContentDatabase* db, Database::ObjectManager& objMgr, std::string p_sDirectory, object_id_t p_nParentId)
{
    p_sDirectory = Directory::appendTrailingSlash(p_sDirectory);

    if(!Directory::exists(p_sDirectory))
        return;

    log(Logging::Log::contentdb, Logging::Log::extended)<< "read dir \"" << p_sDirectory << "\"";

    Directory dir(p_sDirectory);
    dir.open();
    DirEntryList list = dir.dirEntryList();
    dir.close();

   // DbObject obj;
    Database::Item item;
    object_id_t objectId = 0;

    for(unsigned int i = 0; i < list.size(); i++) {

        DirEntry entry = list.at(i);
        objectId = 0;

        if(entry.type() == DirEntry::Directory && !Directory::hidden(entry.path())) {

            if(m_rebuildType & RebuildThread::addNew) {
                objectId = GetObjectIDFromFileName(objMgr, entry.path());
            }

            if(0 == objectId) {

                objectId = db->GetObjId();
                OBJECT_TYPE folderType = CONTAINER_STORAGEFOLDER;

                // insert the directory
                /*obj.reset();
                obj.setObjectId(objectId);
                obj.setParentId(p_nParentId);
                obj.setType(folderType);
                obj.setPath(entry.path());
                obj.setTitle(ToUTF8(entry.name(), db->m_fuppesConfig.globalSettings.localCharset));
                obj.save(qry);*/

                item.clear();
                item.objectId = objectId;
                item.parentId = p_nParentId;
                item.type     = folderType;
                item.path     = entry.path();
                item.title    = ToUTF8(entry.name(), db->m_fuppesConfig.globalSettings.localCharset);

                objMgr.persist(item);

                // watch the dir
                db->fileAlterationMonitor()->addWatch(entry.absolutePath());

                // check for album art
                string albumArt = findAlbumArtFile(entry.path(), db->getFileDetails());
                if(albumArt.length() > 0) {
                    object_id_t artId = GetObjectIDFromFileName(objMgr, albumArt);
                    folderType = CONTAINER_ALBUM_MUSICALBUM;
                    if(artId == 0) {
                        InsertFile(db, objMgr, objectId, albumArt, true);
                    }
                }

            }

            // recursively scan subdirectories
            DbScanDir(db, objMgr, entry.absolutePath(), objectId);
        }
        else if(entry.type() == DirEntry::File) {

            string sExt = ExtractFileExt(entry.name());
            if(m_contentDb->getFileDetails()->IsSupportedFileExtension(sExt)) {

                if(m_rebuildType & RebuildThread::addNew) {
                    objectId = GetObjectIDFromFileName(objMgr, entry.absolutePath());
                }

                if(objectId == 0) {
                    InsertFile(db, objMgr, p_nParentId, entry.absolutePath());
                   // msleep(1);
                }
            }
        } // file

    } // for
}

std::string findAlbumArtFile(std::string dir, CFileDetails* fileDetails)
{
    string file;
    dir = Directory::appendTrailingSlash(dir);
    string result;

#ifdef WIN32
    WIN32_FIND_DATA data;
    HANDLE hFile = FindFirstFile(string(dir + "*").c_str(), &data);
    if(NULL == hFile)
    return "";

    while(FindNextFile(hFile, &data)) {
        if(((string(".").compare(data.cFileName) != 0) &&
                        (string("..").compare(data.cFileName) != 0))) {

            file = data.cFileName;
#else

    DIR* pDir;
    dirent* pDirEnt;

    if((pDir = opendir(dir.c_str())) != NULL) {
        while ((pDirEnt = readdir(pDir)) != NULL) {
            if(((string(".").compare(pDirEnt->d_name) != 0) && (string("..").compare(pDirEnt->d_name) != 0))) {

                file = pDirEnt->d_name;
#endif

                if(fileDetails->isAlbumArtFile(file)) {
                    result = dir + file;
                    break;
                }

            } // if

        } // while  FindNext | readdir

#ifndef WIN32
        closedir(pDir);
    } /* if opendir */
#endif

    return result;
}

unsigned int findAlbumArt(CFileDetails* fileDetails, std::string dir, std::string* ext/*, SQLQuery* qry*/)
{
    string file = findAlbumArtFile(dir, fileDetails);
    if(file.length() == 0) {
        return 0;
    }

    string path = File(file).path();
    file = file.substr(path.length(), file.length());

#warning todo

    /*
    qry->select("select * from OBJECTS "
            "where PATH = '" + SQLEscape(path) + "' and "
            "FILE_NAME = '" + SQLEscape(file) + "' and "
            "DEVICE is NULL");
    if(qry->eof()) {
        return 0;
    }

    *ext = ExtractFileExt(file);
    return qry->result()->asUInt("OBJECT_ID");
    */
    return 0;
}

object_id_t RebuildThread::InsertFile(CContentDatabase* pDb, Database::ObjectManager& objMgr, object_id_t p_nParentId, std::string p_sFileName, bool hidden /* = false*/)
{
    object_id_t nObjId = 0;

    if(m_rebuildType & RebuildThread::addNew) {
        nObjId = GetObjectIDFromFileName(objMgr, p_sFileName);
        if(nObjId > 0) {
            return nObjId;
        }
    }

    return pDb->insertFile(p_sFileName, p_nParentId, objMgr);
}

unsigned int InsertURL(std::string p_sURL, std::string p_sTitle = "", std::string p_sMimeType = "")
{
    // todo FIXME: object type
    OBJECT_TYPE nObjectType = ITEM_AUDIO_ITEM_AUDIO_BROADCAST;
    unsigned int nObjId = CContentDatabase::GetObjId();
    //SQLQuery qry;

    stringstream sSql;
    sSql << "insert into OBJECTS (TYPE, OBJECT_ID, PATH, FILE_NAME, TITLE, MIME_TYPE) values " << "(" << nObjectType << ", " << nObjId << ", " << "'" << SQLEscape(p_sURL) << "', "
            << "'" << SQLEscape(p_sURL) << "', " << "'" << SQLEscape(p_sTitle) << "', " << "'" << SQLEscape(p_sMimeType) << "');";

#warning todo

    //qry.exec(sSql.str());
    return nObjId;
}

/*
 void BuildPlaylists()
 {
 stringstream sGetPlaylists;
 sGetPlaylists <<
 "select     " <<
 "  *        " <<
 "from       " <<
 "  OBJECTS  " <<
 "where      " <<
 "  TYPE = " << CONTAINER_PLAYLIST_CONTAINER;

 SQLQuery qry;
 qry.select(sGetPlaylists.str());
 while(!qry.eof()) {
 ParsePlaylist(qry.result());
 qry.next();
 }
 }
 */

object_id_t GetObjectIDFromFileName(Database::ObjectManager& objMgr, std::string p_sFileName)
{
    //unsigned int nResult = 0;
    //stringstream sSQL;

    string path = File(p_sFileName).path();
    string fileName;
    if(path.length() < p_sFileName.length()) {
        fileName = p_sFileName.substr(path.length(), p_sFileName.length());
    }

    Database::Item item;
    if(false == objMgr.findOneByFilename(path, fileName, item, false)) {
        return 0;
    }

    return item.objectId;

//cout << "GetObjectIDFromFileName() : " << p_sFileName << " " << path << " " << fileName << endl;
/*
    sSQL << "select OBJECT_ID "
            "from OBJECTS "
            "where "
            "  REF_ID = 0 and "
            "  PATH = '" << SQLEscape(path) << "' ";

    if(fileName.empty())
        sSQL << " and FILE_NAME is NULL ";
    else
        sSQL << " and FILE_NAME = '" + SQLEscape(fileName) + "' ";

    sSQL << "and DEVICE is NULL";

    qry->select(sSQL.str());
    if(!qry->eof())
        nResult = qry->result()->asUInt("OBJECT_ID");

    return nResult;
    */
}

/*
 void ParsePlaylist(CSQLResult* pResult)
 {
 BasePlaylistParser* Parser = BasePlaylistParser::Load(pResult->asString("PATH") + pResult->asString("FILE_NAME"));
 if(Parser == NULL) {
 return;
 }

 unsigned int nPlaylistID = pResult->asUInt("OBJECT_ID");
 unsigned int nObjectID   = 0;

 //cout << "playlist id: " << nPlaylistID << endl;

 CContentDatabase* pDb = CContentDatabase::Shared(); //new CContentDatabase();
 SQLQuery qry;
 DbObject* existing;

 while(!Parser->Eof()) {
 if(Parser->Entry()->bIsLocalFile && File::exists(Parser->Entry()->sFileName)) {

 // nObjectID = GetObjectIDFromFileName(&qry, Parser->Entry()->sFileName);
 existing = DbObject::createFromFileName(Parser->Entry()->sFileName, &qry);

 //if(nObjectID == 0) {
 if(existing == NULL) {
 //nObjectID = InsertFile(pDb, &qry, nPlaylistID, Parser->Entry()->sFileName);
 }
 else {
 //MapPlaylistItem(nPlaylistID, nObjectID);

 DbObject* object = new DbObject(existing);
 object->setObjectId(pDb->GetObjId());
 object->setParentId(nPlaylistID);
 object->setRefId(existing->objectId());
 object->save(&qry);
 delete object;
 delete existing;
 }
 }
 else if(!Parser->Entry()->bIsLocalFile) {
 nObjectID = InsertURL(Parser->Entry()->sFileName, Parser->Entry()->sTitle, Parser->Entry()->sMimeType);
 //MapPlaylistItem(nPlaylistID, nObjectID);
 }

 Parser->Next();
 }

 delete Parser;
 //delete pDb;
 }
 */

//fuppesThreadCallback BuildLoop(void* arg)
void RebuildThread::run()
{
    if(m_contentDb->getFuppesConfig()->databaseSettings.readonly) {
        CSharedLog::Print("[ContentDatabase] readonly database rebuild disabled");
        return;
    }

    if(m_delay > 0)
        msleep(m_delay);

    // stop update thread
    //m_updateThread->stop();

    DateTime start = DateTime::now();
    CSharedLog::Print("[ContentDatabase] create database at %s", start.toString().c_str());

    //SQLQuery qry;
    stringstream sSql;

    if(m_rebuildType & RebuildThread::rebuild) {
        //qry.exec("delete from OBJECTS");
        //qry.exec("delete from OBJECT_DETAILS");
        CContentDatabase::m_objectId = 0;
        //qry.exec("delete from MAP_OBJECTS");
    }

    Database::ObjectManager objMgr;

    /*pDb->Execute("drop index IDX_OBJECTS_OBJECT_ID");
     pDb->Execute("drop index IDX_MAP_OBJECTS_OBJECT_ID");
     pDb->Execute("drop index IDX_MAP_OBJECTS_PARENT_ID");
     pDb->Execute("drop index IDX_OBJECTS_DETAIL_ID");
     pDb->Execute("drop index IDX_OBJECT_DETAILS_ID");*/

    if(m_rebuildType & RebuildThread::removeMissing) {

        CSharedLog::Print("remove missing");
        //CContentDatabase* pDel = new CContentDatabase();
        //SQLQuery del; // = CDatabase::query();

        /*qry.select("select * from OBJECTS");
        while (!qry.eof()) {
            CSQLResult* result = qry.result();

            if(result->asUInt("TYPE") < CONTAINER_MAX) {

                if(Directory::exists(result->asString("PATH")) || result->asString("PATH").substr(0, 1).compare("*") == 0) {
                    qry.next();
                    continue;
                }
            }
            else {

                if(result->asUInt("TYPE") == ITEM_AUDIO_ITEM_AUDIO_BROADCAST || result->asUInt("TYPE") == ITEM_VIDEO_ITEM_VIDEO_BROADCAST
                        || result->asUInt("TYPE") == ITEM_UNKNOWN_BROADCAST) {
                    // todo check stream sources availability
                    qry.next();
                    continue;
                }

                if(File::exists(result->asString("PATH") + result->asString("FILE_NAME"))) {
                    qry.next();
                    continue;
                }
            }
            DbObject* file = new DbObject(result);
            m_contentDb->getVirtualContainerMgr()->deleteFile(file);
            delete file;

            sSql << "delete from OBJECT_DETAILS where ID = " << result->asString("OBJECT_ID");
            del.exec(sSql.str());
            sSql.str("");

            sSql << "delete from OBJECTS where OBJECT_ID = " << result->asString("OBJECT_ID");
            del.exec(sSql.str());
            sSql.str("");

            qry.next();
        }
*/
        //delete del;
        CSharedLog::Print("[DONE] remove missing");
    }

#warning TODO
   // qry.connection()->vacuum();

    int i;
    object_id_t nObjId = 0;
    string sFileName;
    bool bInsert = true;
    //DbObject obj;
    Database::Item item;

    CSharedLog::Print("read shared directories");

    CContentDatabase* db = m_contentDb; //CContentDatabase::Shared();
    //SharedObjects* sos = CSharedConfig::Shared()->sharedObjects();

    Configuration::SharedObjects* sos = &db->getFuppesConfig()->sharedObjects;
    Configuration::SharedObject* so;

    for(i = 0; i < sos->count(); i++) {
        so = sos->getSharedObject(i);
        if(so->type != Configuration::SharedObject::directory)
            continue;
        string tempSharedDir = so->path;

        if(Directory::exists(tempSharedDir)) {

            db->fileAlterationMonitor()->addWatch(tempSharedDir);

            ExtractFolderFromPath(tempSharedDir, &sFileName);
            bInsert = true;
            if(m_rebuildType & RebuildThread::addNew) {
                if((nObjId = GetObjectIDFromFileName(objMgr, tempSharedDir)) > 0) {
                    bInsert = false;
                }
            }

            sSql.str("");
            if(bInsert) {
                nObjId = db->GetObjId();
                sFileName = ToUTF8(sFileName, m_contentDb->m_fuppesConfig.globalSettings.localCharset);

                item.clear();
                item.objectId = nObjId;
                item.parentId = 0;
                item.type = CONTAINER_STORAGEFOLDER;
                item.path = tempSharedDir;
                item.title = sFileName;
                objMgr.persist(item);

                /*obj.reset();
                obj.setObjectId(nObjId);
                obj.setParentId(0);
                obj.setType(CONTAINER_STORAGEFOLDER);
                obj.setPath(tempSharedDir);
                obj.setTitle(sFileName);
                obj.save(&qry);*/
            }

            DbScanDir(db, objMgr, tempSharedDir, nObjId);
        }
        else {
            CSharedLog::Log(L_EXT, __FILE__, __LINE__, "shared directory: \" %s \" not found", tempSharedDir.c_str());
        }
    } // for
    CSharedLog::Print("[DONE] read shared directories");

    /*CSharedLog::Print("parse playlists");
     BuildPlaylists();
     CSharedLog::Print("[DONE] parse playlists");*/

    // import iTunes db
    /*CSharedLog::Print("parse iTunes databases");
     CiTunesImporter* pITunes = new CiTunesImporter();
     for(i = 0; i < sos->sharedObjectCount(); i++) {
     so = sos->sharedObject(i);
     if(so->type() != SharedObject::itunes)
     continue;
     pITunes->Import(so->path());
     }
     delete pITunes;
     CSharedLog::Print("[DONE] parse iTunes databases");*/

    // import other item sources
    CSharedLog::Print("update other");
    for(i = 0; i < sos->count(); i++) {
        so = sos->getSharedObject(i);
        if(so->type != Configuration::SharedObject::other)
            continue;
        //db->updateOther(so);
    }
    CSharedLog::Print("[DONE] updating other");

    // create virtual folder layout
    if(m_rebuildType & RebuildThread::rebuild) {
        m_contentDb->getVirtualContainerMgr()->rebuildContainerLayouts(false);
    //}
        //m_contentDb->getVirtualContainerMgr()->getMgr()->RebuildContainerList(true, false);
    }

    DateTime end = DateTime::now();
    CSharedLog::Print("[ContentDatabase] database created at %s", end.toString().c_str());

    // start update thread
    m_contentDb->startUpdateThread();
}

void CContentDatabase::updateOther(Configuration::SharedObject* so)
{
    /*Plugins::ItemSource* plugin = NULL; //CPluginMgr::itemSource(so->otherType());
    if(NULL == plugin)
        return;

    if(!plugin->open(so->path)) {
        delete plugin;
        return;
    }

#warning todo

    // get or create the base folder for the source type
    DbObject* base = DbObject::createFromFileName("plugin:" + plugin->name());
    if(NULL == base) {
        base = new DbObject();
        base->setPath("*plugin:" + plugin->name());
        base->setType(CONTAINER_STORAGEFOLDER);
        base->setTitle(plugin->baseName());
        base->setParentId(0);
        base->save();
    }

    bool firstImport = false;

    // get or create the base folder for the source item
    DbObject* source = DbObject::createFromFileName(plugin->name() + ":" + so->path);
    if(NULL == source) {
        source = new DbObject();
        source->setPath("*" + plugin->name() + ":" + so->path);
        source->setType(CONTAINER_STORAGEFOLDER);
        source->setTitle(so->name);
        source->setParentId(base->objectId());
        source->save();

        firstImport = true;
    }

    DbObject* last = NULL;

    // get the last imported item for the source
    if(!firstImport) {
        SQLQuery qry;
        string sql = ""; //qry.build(SQL_GET_CHILD_OBJECTS, source->objectId()) + "OBJECT_ID desc";
        qry.select(sql);
        if(!qry.eof()) {
            last = new DbObject(qry.result());
        }
    }

    std::list<metadata_t*> items;
    metadata_t* metadata;
    for(;;) {
        metadata = new metadata_t();
        init_metadata(metadata);

        if(plugin->next(metadata) != 0) {
            free_metadata(metadata);
            delete metadata;
            break;
        }

        if(NULL != last && last->fileName().compare(metadata->file_name) == 0) {
            free_metadata(metadata);
            delete metadata;
            break;
        }

        items.push_back(metadata);
    }

    if(plugin->flags() & sf_reverse) {
        items.reverse();
    }

    int number = 0;
    if(plugin->flags() & sf_numbered) {
        if(NULL == last) {
            number = 1;
        }
        else {
            size_t pos = last->title().find(" ");
            if(pos != string::npos) {
                string tmp = last->title().substr(0, pos);
                number = atoi(tmp.c_str());
            }
        }
    }

    std::list<metadata_t*>::iterator iter;
    for(iter = items.begin(); iter != items.end(); iter++) {

        number++;
    }
    */

    //delete metadata;

    /*

     * DbObject obj;
     ObjectDetails details;

     VideoItem item;
     *
     while(plugin->next(item.metadata())) {

     cout << "NEXT BLIP ITEM: " << item.title() <<endl;

     obj.reset();
     obj.setType(ITEM_VIDEO_ITEM_VIDEO_BROADCAST);
     obj.setTitle(item.title());
     obj.setPath(item.url());
     obj.setParentId(nc.objectId());

     details.reset();
     details = item;
     details.setAlbumArtExt("fail");
     details.setUpdated();
     details.save();

     obj.setDetailId(details.id());
     obj.save();
     obj.setUpdated();
     obj.save();
     }
     */

   /* delete source;
    delete base;

    plugin->close();
    delete plugin;*/
}

bool CContentDatabase::exportData(std::string fileName, std::string path, bool remove) // static
{
    Database::ConnectionParameters params;
    params.filename = fileName;

    Plugins::DatabasePlugin* sqlitePlugin = NULL; //CPluginMgr::databasePlugin("sqlite3");
    if(!sqlitePlugin)
        return false;

    Database::Connection* connection = NULL; //sqlitePlugin->createConnection();
    if(!connection)
        return false;

    connection->setParameters(params);
    if(!connection->connect()) {
        delete connection;
        return false;
    }

    stringstream sql;
    // get from local db
    //SQLQuery get;
    // write to new connection
   // SQLQuery set(connection);

    /*sql << set.build(SQL_TABLES_EXIST, 0);
    set.select(sql.str());
    sql.str("");
    if(!set.eof()) {
        set.exec("drop table FUPPES_DB_INFO");
        set.exec("drop table OBJECTS");
        set.exec("drop table OBJECT_DETAILS");
    }

    // create tables
    sql << set.build(SQL_CREATE_TABLE_DB_INFO, 0);
    set.exec(sql.str());
    sql.str("");

    sql << set.build(SQL_CREATE_TABLE_OBJECTS, 0);
    set.exec(sql.str());
    sql.str("");

    sql << set.build(SQL_CREATE_TABLE_OBJECT_DETAILS, 0);
    set.exec(sql.str());
    sql.str("");

    sql << set.build(SQL_SET_DB_INFO, DB_VERSION);
    set.exec(sql.str());
    sql.str("");
    */

    // get all objects in path
    path = Directory::appendTrailingSlash(path);
    sql.str("");
    sql << "select * from OBJECTS where " << "PATH like '" << SQLEscape(path) << "%' and " << "DEVICE is NULL";

    /*DbObject* in;
    DbObject* out;
    ObjectDetails details;

    object_id_t oid = 0;

    get.select(sql.str());
    cout << "START EXPORT" << endl;
    while (!get.eof()) {
        oid++;
        in = new DbObject(get.result());
        out = new DbObject(in);

        cout << "export OBJECT: " << in->title() << "*" << endl;

        // export details
        if(in->detailId() > 0) {
            details.reset();
            details = *in->details();
            details.save(&set);

            out->setDetailId(details.id());
        }

        // export object
        string newPath = in->path();
        newPath = "./" + StringReplace(newPath, path, "");

        out->setPath(newPath);
        out->setObjectId(in->objectId());
        out->save(&set);
        delete out;

        // delete object from local db

        delete in;

        get.next();
    }
    cout << "EXPORT FINISHED" << endl;
*/
    delete connection;
    return true;
}

bool CContentDatabase::importData(std::string fileName, std::string path) // static
{
    Database::ConnectionParameters params;
    params.filename = fileName;

    Plugins::DatabasePlugin* sqlitePlugin = NULL; //CPluginMgr::databasePlugin("sqlite3");
    if(!sqlitePlugin)
        return false;

    Database::Connection* connection = NULL; //sqlitePlugin->createConnection();
    if(!connection)
        return false;

    connection->setParameters(params);
    if(!connection->connect()) {
        delete connection;
        return false;
    }

    stringstream sql;
    // get from new connection
   // SQLQuery get(connection);
    // write to local db
   // SQLQuery set;

    delete connection;
    return false;
}

