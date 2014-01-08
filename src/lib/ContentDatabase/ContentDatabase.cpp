/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ContentDatabase.h"

#include "../Database/ObjectManager.h"
#include "../Database/TableManager.h"
#include "../Database/QueryBuilder.h"

#include "../Log/Log.h"

object_id_t ContentDatabase::ContentDatabase::m_objectId = 0;
uint32_t ContentDatabase::ContentDatabase::m_systemUpdateId = 0;

object_id_t ContentDatabase::ContentDatabase::getObjectId() // static
{
    return ++ContentDatabase::m_objectId;
}

uint32_t ContentDatabase::ContentDatabase::getSystemUpdateId() // static
{
    return ContentDatabase::m_systemUpdateId;
}


ContentDatabase::ContentDatabase::ContentDatabase(Configuration& configuration, CFileDetails& fileDetails, Plugin::Manager& pluginManager, VirtualContainer::Manager& virtualContainerMgr) :
        m_configuration(configuration),
        m_fileDetails(fileDetails),
        m_virtualContainerMgr(virtualContainerMgr),
        m_scanDirectoryThread(*this),
        m_updateThread(*this, configuration, fileDetails, pluginManager, virtualContainerMgr),
        m_removeMissingThread(*this)
{
    m_fam = NULL;
    m_status = Idle;
}

ContentDatabase::ContentDatabase::~ContentDatabase()
{
    m_updateThread.stop();
    m_scanDirectoryThread.stop();
    m_removeMissingThread.stop();

    m_updateThread.close();
    m_scanDirectoryThread.close();
    m_removeMissingThread.close();

    if (NULL != m_fam) {
        m_fam->stop();
        m_fam->close();
        delete m_fam;
    }
}

void ContentDatabase::ContentDatabase::init()
{
    if (m_configuration.isReadonly()) {
        return;
    }

    Database::ObjectManager objMgr;
    objMgr.getMaxObjectId(ContentDatabase::m_objectId);

    // setup file alteration monitoring
    m_fam = Fam::Monitor::createMonitor(*this, false);

    Database::QueryResult result;
    objMgr.getAllContainerPaths(result);
    for (size_t i = 0; i < result.size(); i++) {
        m_fam->addWatch(result.getRow(i)->getColumnString("PATH"), true);
    }

    m_fam->start();
}

void ContentDatabase::ContentDatabase::insertDirectory(const std::string path, const std::string name, const object_id_t parentId, Database::ObjectManager& objMgr)
{
    Database::Item dir;
    objMgr.findOneByFilename(path, "", dir, false);
    if (0 != dir.id) {
        return;
    }

    dir.objectId = ContentDatabase::getObjectId();
    dir.parentId = parentId;
    dir.type = CONTAINER_STORAGEFOLDER;
    dir.path = path;
    dir.title = ToUTF8(name, m_configuration.getLocalCharset());
    dir.modified = fuppes::DateTime::now().toInt();
    dir.updated = 0;

    objMgr.persist(dir);

    m_fam->addWatch(path);
}

void ContentDatabase::ContentDatabase::insertFile(const std::string path, const std::string name, const object_id_t parentId, Database::ObjectManager& objMgr)
{
    Database::Item file;
    objMgr.findOneByFilename(path, name, file, false);
    if (0 != file.id) {
        return;
    }

    std::string filename = path + name;

    file.type = m_fileDetails.GetObjectType(filename);
    if (OBJECT_TYPE_UNKNOWN == file.type) {
        return;
    }

    file.visible = !m_fileDetails.isAlbumArtFile(filename);

    file.objectId = ContentDatabase::getObjectId();
    file.parentId = parentId;
    file.title = fuppes::FormatHelper::fileNameToTitle(name, m_configuration.getLocalCharset());
    file.path = path;
    file.filename = name;
    file.extension = ExtractFileExt(name);
    file.size = fuppes::File::getSize(filename);
    file.modified = fuppes::File::lastModified(filename);
    if (0 == file.modified) {
        file.modified = fuppes::DateTime::now().toInt();
    }
    file.updated = 0;

    objMgr.persist(file);
}

void ContentDatabase::ContentDatabase::onScanDirectoryThreadFinished(const bool stopped)
{
    if (!stopped) {
        m_updateThread.stop();
        m_updateThread.close();

        m_status = UpdateMetadata;
        m_updateThread.start();
    }
    else {
        m_status = Stopped;
    }
}

void ContentDatabase::ContentDatabase::onUpdateThreadFinished(const bool stopped)
{
    m_status = stopped ? Stopped : Idle;
}

void ContentDatabase::ContentDatabase::onRemoveMissingThreadFinished(const bool stopped)
{
    if(!stopped) {
        m_scanDirectoryThread.stop();
        m_scanDirectoryThread.close();

        m_status = ScanDirectories;
        m_scanDirectoryThread.start(m_fam);
    }
    else {
        m_status = Stopped;
    }
}

void ContentDatabase::ContentDatabase::rebuild()
{
    logExt("contentdb") << "rebuild database";

    Database::TableManager tblMgr;
    tblMgr.empty();

    m_virtualContainerMgr.setConnection(Database::Connection::getDefaultConnection());
    m_virtualContainerMgr.rebuildContainerLayouts(false);

    update();
}

void ContentDatabase::ContentDatabase::update()
{
    logExt("contentdb") << "update database";

    m_scanDirectoryThread.stop();
    m_updateThread.stop();

    m_scanDirectoryThread.close();
    m_updateThread.close();

    // insert shared directories
    Database::ObjectManager objMgr;
    Database::Item dir;

    SharedObjects_t sharedObjects = m_configuration.getSharedObjects();
    for (size_t i = 0; i < sharedObjects.size(); i++) {

        if (SharedObject::directory != sharedObjects.at(i).type) {
            continue;
        }

        dir.clear();
        objMgr.findOneByFilename(sharedObjects.at(i).path, "", dir, false);
        if (0 != dir.id) {
            continue;
        }

        std::string title;
        ExtractFolderFromPath(sharedObjects.at(i).path, &title);

        dir.objectId = ContentDatabase::getObjectId();
        dir.parentId = 0;
        dir.type = CONTAINER_STORAGEFOLDER;
        dir.path = sharedObjects.at(i).path;
        dir.title = ToUTF8(title, m_configuration.getLocalCharset());
        dir.modified = fuppes::DateTime::now().toInt();
        dir.updated = 0;

        objMgr.persist(dir);
        m_fam->addWatch(dir.path);

        logDbg("contentdb") << "inserted directory" << dir.path;
    }

    m_status = RemoveMissing;
    m_removeMissingThread.stop();
    m_removeMissingThread.close();
    m_removeMissingThread.start();
}

void ContentDatabase::ContentDatabase::onDirectoryCreateEvent(const std::string path, const std::string name)
{
    Database::ObjectManager objMgr;

    Database::Item parent;
    objMgr.findOneByFilename(path, "", parent, false);
    if (0 == parent.id) {
        return;
    }

    // if the parent is already marked as modified we can ignore this event
    if (parent.updated < parent.modified) {
        return;
    }

    insertDirectory(path + name + "/", name, parent.objectId, objMgr);

    if (!m_scanDirectoryThread.running()) {
        m_scanDirectoryThread.close();
        m_scanDirectoryThread.start();
    }
}

void ContentDatabase::ContentDatabase::onDirectoryDeleteEvent(const std::string path)
{
    m_scanDirectoryThread.stop();
    m_updateThread.stop();

    Database::ObjectManager objMgr;

    Database::Item dir;
    objMgr.findOneByFilename(path, "", dir, false);
    if (0 == dir.id) {
        return;
    }

    m_fam->removeWatch(dir.path);
    objMgr.remove(dir);
}

void ContentDatabase::ContentDatabase::onDirectoryMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name)
{
    Database::ObjectManager objMgr;

    Database::Item dir;
    objMgr.findOneByFilename(oldpath + oldname + "/", "", dir, false);
    if (0 == dir.id) {
        return;
    }

    // moved
    Database::Item parent;
    if (0 != oldpath.compare(path)) {

        objMgr.findOneByFilename(path, "", parent, false);
        if (0 == parent.id) {
            return;
        }

        dir.parentId = parent.objectId;
    }

    // renamed
    if (0 != oldname.compare(name)) {
        dir.title = ToUTF8(name, m_configuration.getLocalCharset());
    }

    dir.path = path + name + "/";

    objMgr.persist(dir);

    m_fam->removeWatch(oldpath + oldname + "/");
    m_fam->addWatch(dir.path);
}

void ContentDatabase::ContentDatabase::onFileCreateEvent(const std::string path, const std::string name)
{
    Database::ObjectManager objMgr;

    Database::Item dir;
    objMgr.findOneByFilename(path, "", dir, false);
    if (0 == dir.id) {
        return;
    }

    insertFile(path, name, dir.objectId, objMgr);

    if (!m_updateThread.running()) {
        m_updateThread.close();
        m_updateThread.start();
    }
}

void ContentDatabase::ContentDatabase::onFileDeleteEvent(const std::string path, const std::string name)
{
    Database::ObjectManager objMgr;

    Database::Item file;
    objMgr.findOneByFilename(path, name, file, false);
    if (0 == file.id) {
        return;
    }

    objMgr.remove(file);
}

void ContentDatabase::ContentDatabase::onFileMoveEvent(const std::string oldpath, const std::string oldname, const std::string path, const std::string name)
{
    Database::ObjectManager objMgr;

    Database::Item file;
    objMgr.findOneByFilename(oldpath, oldname, file, false);
    if (0 == file.id) {
        return;
    }

    // moved
    Database::Item parent;
    if (0 != oldpath.compare(path)) {

        objMgr.findOneByFilename(path, "", parent, false);
        if (0 == parent.id) {
            return;
        }

        file.parentId = parent.objectId;
    }

    // renamed
    if (0 != oldname.compare(name)) {
        file.filename = name;
        file.extension = ExtractFileExt(name);

        // check if the old title was created from the filename
        std::string oldtitle;
        ExtractFolderFromPath(oldpath, &oldtitle);
        oldtitle = ToUTF8(oldtitle, m_configuration.getLocalCharset());
        if (0 == file.title.compare(oldtitle)) {
            std::string title;
            ExtractFolderFromPath(path, &title);
            file.title = ToUTF8(title, m_configuration.getLocalCharset());
        }

    }

    file.path = path;

    objMgr.persist(file);
}

void ContentDatabase::ContentDatabase::onFileModifiedEvent(const std::string path, const std::string name)
{
    Database::ObjectManager objMgr;

    Database::Item file;
    objMgr.findOneByFilename(path, name, file, false);
    if (0 == file.id) {
        return;
    }

    time_t lastModified = fuppes::File::lastModified(path + name);
    if (lastModified <= file.modified) {
        return;
    }

    file.size = fuppes::File::getSize(path + name);
    file.modified = lastModified;
    objMgr.persist(file);

    if (!m_updateThread.running()) {
        m_updateThread.close();
        m_updateThread.start();
    }
}
