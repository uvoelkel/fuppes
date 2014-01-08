/*
 * This file is part of fuppes
 *
 * (c) 2010-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "UpdateThread.h"

#include "ContentDatabase.h"
#include "../Database/QueryBuilder.h"

using namespace ContentDatabase;

#include <iostream>

UpdateThread::UpdateThread(ContentDatabase& contentDatabase, Configuration& configuration, CFileDetails& fileDetails, Plugin::Manager& pluginManager, VirtualContainer::Manager& virtualContainerMgr) :
        Thread("UpdateThread"),

        m_contentDatabase(contentDatabase),
        m_configuration(configuration),
        m_fileDetails(fileDetails),
        m_pluginManager(pluginManager),
        m_virtualContainerMgr(virtualContainerMgr)
{
    m_connection = NULL;
}

UpdateThread::~UpdateThread()
{
    close();
    if (NULL != m_connection) {
        m_virtualContainerMgr.setConnection(NULL);
        m_connection->close();
        delete m_connection;
    }
}

/*

 1. check for "normal" files to update (DEVICE == NULL && REF_ID == 0)
 2. check for "referenced" files to update (DEVICE == NULL && REF_ID > 0)
 if we find metadata in the playlist we add this else we set the DETAIL_ID to the same as the referenced object (REF_ID)
 3. check "normal" dirs for update (DEVICE == NULL)
 - check for album art
 - set container type to e.g. music.Album if it contains only audio items
 
 */
/*
 bool UpdateThread::famEventOccurred() // inline
 {
 return false;
 int diff = DateTime::now().toInt() - m_famHandler.lastEventTime().toInt();
 return (diff < 5);
 }
 */

void UpdateThread::run()
{
    if (Database::Connection::getDefaultConnection()->getParameters().readonly) {
        return;
    }

    if (NULL == m_connection) {
        m_connection = Database::Connection::createConnection();
    }
    m_virtualContainerMgr.setConnection(m_connection);

    int count;
    Database::ObjectManager objMgr(*m_connection);

    do {
        // 1. update the items metadata
        count = updateItemsMetadata(objMgr);
        if (stopRequested()) {
            break;
        }

        // 2. check for album art and update dirs/files
        count += updateAlbumArt(objMgr);
        if (stopRequested()) {
            break;
        }

        // 3. map images or create video thumbnails if enabled
        count += createVideoThumbnails(objMgr);
        if (stopRequested()) {
            break;
        }

        // 4. parse playlists
        // todo

    } while (count > 0 && !stopRequested());

    m_contentDatabase.onUpdateThreadFinished(stopRequested());
}

int UpdateThread::updateItemsMetadata(Database::ObjectManager& objMgr)
{
    Database::Item item;
    int count = 0;

    Database::QueryBuilder qb(*m_connection);
    qb.select(Database::ObjectDetails).where("o", "TYPE", ">", "?").andWhere("o", "DEVICE", "is", "NULL").andWhere("o", "REF_ID", "=", "?").andWhere("(o", "UPDATED_AT", "is", "NULL").orWhere("o", "UPDATED_AT", "<", "o.MODIFIED_AT)");

    Database::PreparedStatement getItems(*m_connection);
    qb.getQuery(getItems);

    getItems.bindInt64(0, ITEM);
    getItems.bindInt64(1, 0);

    bool isUpdate = false;

    // update metadata
    while (getItems.next(item) && !stopRequested()) {

        count++;
        //std::cout << "update object " << count << " :: " << item.filename << " :: " << item.extension << std::endl;

        isUpdate = (0 != item.detailId);

        switch (item.type) {

            case ITEM_AUDIO_ITEM:
            case ITEM_AUDIO_ITEM_MUSIC_TRACK:
                updateAudioFile(objMgr, item);
                break;

            case ITEM_IMAGE_ITEM:
            case ITEM_IMAGE_ITEM_PHOTO:
                updateImageFile(objMgr, item);
                break;

            case ITEM_VIDEO_ITEM:
            case ITEM_VIDEO_ITEM_MOVIE:
            case ITEM_VIDEO_ITEM_MUSIC_VIDEO_CLIP:
                updateVideoFile(objMgr, item);
                break;

            default:
                continue;
                break;
        }

        if (isUpdate) {
            m_virtualContainerMgr.removeFile(item);
        }
        m_virtualContainerMgr.insertFile(item);
    }

    return count;
}

void UpdateThread::updateAudioFile(Database::ObjectManager& objMgr, Database::Item& item)
{
    std::string fileName = item.path + item.filename;
    if (m_fileDetails.getMusicTrackDetails(fileName, item)) {

        if (!item.details->albumArtMimeType.empty()) {
            item.details->albumArtId = item.objectId;
            item.details->albumArtExtension = m_fileDetails.getBaseDevice()->extensionByMimeType(item.details->albumArtMimeType);
        }
        objMgr.persist(*item.details);
    }

    if (0 == item.detailId) {
        item.detailId = item.details->id;
    }
    item.updated = fuppes::DateTime::now().toInt();
    objMgr.persist(item);
}

void UpdateThread::updateImageFile(Database::ObjectManager& objMgr, Database::Item& item)
{
    std::string fileName = item.path + item.filename;
    if (m_fileDetails.getImageDetails(fileName, item)) {
        objMgr.persist(*item.details);
    }

    if (0 == item.detailId) {
        item.detailId = item.details->id;
    }
    item.updated = fuppes::DateTime::now().toInt();
    objMgr.persist(item);
}

void UpdateThread::updateVideoFile(Database::ObjectManager& objMgr, Database::Item& item) //DbObject* obj, SQLQuery* qry)
{
    std::string fileName = item.path + item.filename;
    if (m_fileDetails.getVideoDetails(fileName, item)) {
        objMgr.persist(*item.details);
    }

    // check for subtitles
    if (fuppes::File::exists(TruncateFileExt(fileName) + ".srt")) {
        item.details->v_hasSubtitles = true;
    }

    if (0 == item.detailId) {
        item.detailId = item.details->id;
    }
    item.updated = fuppes::DateTime::now().toInt();
    objMgr.persist(item);
}
/*
 void UpdateThread::updateBroadcastUrl(DbObject* obj, SQLQuery* qry)
 {
 //string fileName = obj->path() + obj->fileName();
 cout << "UPDATE BROADCAST URL: " << obj->path() << " " << obj->fileName() << endl;

 obj->setType(ITEM_AUDIO_ITEM_AUDIO_BROADCAST);
 //obj->setType(ITEM_AUDIO_ITEM_VIDEO_BROADCAST);

 obj->setUpdated();
 obj->save(qry);
 }
 */

int UpdateThread::updateAlbumArt(Database::ObjectManager& objMgr) //SQLQuery* qry, SQLQuery* get, SQLQuery* ins)
{
    int count = 0;
    Database::Item item;
    Database::Item parent;
    object_id_t lastPid = 0;

    Database::QueryBuilder qb;
    qb.select(Database::ObjectDetails)
    		.where("o", "TYPE", ">=", "?")
    		.andWhere("o", "TYPE", "<", "?")
    		.andWhere("o", "DEVICE", "is", "NULL")
    		.andWhere("o", "REF_ID", "=", "?")
    		.andWhere("o", "VISIBLE", "=", "?")
    		.andWhere("o", "FILE_NAME", "in", "(?)") // lower()
    		.orderBy("o.FILE_NAME, o.PARENT_ID");

    Database::PreparedStatement getImages(*m_connection);
    qb.getQuery(getImages);
    getImages.bindInt64(0, ITEM_IMAGE_ITEM);
    getImages.bindInt64(1, ITEM_IMAGE_ITEM_MAX);
    getImages.bindInt64(2, 0);
    getImages.bindInt64(3, 1);
    getImages.bindText(4, m_fileDetails.getAlbumArtFiles());


    qb.select(Database::ObjectDetails)
    		.where("o", "TYPE", ">=", "?")
    		.andWhere("o", "TYPE", "<", "?")
    		.andWhere("o", "DEVICE", "is", "NULL")
    		.andWhere("o", "REF_ID", "=", "?")
    		.andWhere("o", "VISIBLE", "=", "?")
    		.andWhere("o", "PARENT_ID", "=", "?");

    Database::PreparedStatement getSiblings(*m_connection);
	qb.getQuery(getSiblings);



    while (getImages.next(item)) {

        //std::cout << "update AlbumArt " << count << " :: " << item.filename << " :: " << item.extension << std::endl;

        if (item.parentId == lastPid) {
            continue;
        }
        lastPid = item.parentId;

        objMgr.findOne(item.parentId, "", parent, true);
        if (0 != parent.details->albumArtId) {
            continue;
        }

        // set the parent folder's album art values
        parent.details->albumArtId = item.id;
        parent.details->albumArtExtension = item.extension;
        parent.details->albumArtMimeType = m_fileDetails.getBaseDevice()->MimeType(item.extension);
        parent.details->albumArtWidth = item.details->iv_width;
        parent.details->albumArtHeight = item.details->iv_height;
        objMgr.persist(*parent.details);

        // get the audio siblings and set their album art id
        Database::Item sibling;
        getSiblings.bindInt64(0, ITEM_AUDIO_ITEM);
        getSiblings.bindInt64(1, ITEM_AUDIO_ITEM_MAX);
        getSiblings.bindInt64(2, 0);
        getSiblings.bindInt64(3, 1);
        getSiblings.bindInt64(4, item.parentId);
        while (getSiblings.next(sibling)) {
        	sibling.details->albumArtId = item.id;
        	sibling.details->albumArtExtension = item.extension;
        	sibling.details->albumArtMimeType = m_fileDetails.getBaseDevice()->MimeType(item.extension);
        	sibling.details->albumArtWidth = item.details->iv_width;
        	sibling.details->albumArtHeight = item.details->iv_height;
            objMgr.persist(*sibling.details);
        }
        getSiblings.clear();

        // Database::AbstractObjectManager::AudioItems
		// todo

        /*
         m_findItems->prepare(sqlSelectItemWithDetails + " where o.TYPE >= ? and o.TYPE < ? and o.PARENT_ID = ? and o.DEVICE is NULL");

         m_findItems->bindInt64(0, ITEM_AUDIO_ITEM);
         m_findItems->bindInt64(1, ITEM_AUDIO_ITEM_MAX);
         m_findItems->bindInt64(2, va_arg(args, object_id_t));
         */

        // hide the image
        item.visible = false;
        objMgr.persist(item);
        count++;
    }

    return count;

    // check for album art and update dirs/files

    // get the audio siblings and set their album art id
    /*sql.str("");
     sql << "select * from OBJECTS where PARENT_ID = " << obj->parentId() << " and " << "TYPE >= " << ITEM_AUDIO_ITEM << " and TYPE < " << ITEM_AUDIO_ITEM_MAX << " and " << "DEVICE is NULL";
     //cout << sql.str() << endl;
     get->select(sql.str());
     while (!get->eof()) {

     sibling = new DbObject(get->result());

     if(sibling->details()->albumArtId() == 0) {
     sibling->details()->setAlbumArtId(obj->objectId());
     sibling->details()->setAlbumArtExt(ext);
     sibling->details()->setAlbumArtMimeType(mime);
     sibling->details()->setAlbumArtWidth(obj->details()->width());
     sibling->details()->setAlbumArtHeight(obj->details()->height());
     sibling->details()->save(ins);
     sibling->setDetailId(sibling->details()->id());
     sibling->save(ins);
     }

     delete sibling;
     get->next();
     }

     // hide the image object
     lastPid = obj->parentId();
     obj->setVisible(false);
     obj->save();
     //obj->details()->setAlbumArtExt(ExtractFileExt(obj->fileName()));
     obj->details()->save(ins);
     delete obj;
     qry->next();
     msleep(1);
     } // while !eof (update album art)
     */

}

int UpdateThread::createVideoThumbnails(Database::ObjectManager& objMgr)
{
    int count = 0;
    Plugin::Metadata* thumbnailer = m_pluginManager.createMetadataPlugin("ffmpegthumbnailer");

    Database::QueryBuilder qb(*m_connection);

    Database::Item video;
    Database::PreparedStatement getVideos(*m_connection);

    qb.select(Database::ObjectDetails).where("o", "TYPE", ">=", "?").andWhere("o", "TYPE", "<", "?").andWhere("o", "DEVICE", "is", "NULL").andWhere("o", "REF_ID", "=", "?").andWhere("o", "DETAIL_ID", "in", "").bracketOpen().select("x", "ID", Database::Details).where("x",
            "ALBUM_ART_ID", "=", "?").andWhere("x", "ALBUM_ART_EXT", "is", "NULL").bracketClose();
    qb.getQuery(getVideos);

    getVideos.bindInt64(0, ITEM_VIDEO_ITEM);
    getVideos.bindInt64(1, ITEM_VIDEO_ITEM_MAX);
    getVideos.bindInt64(2, 0);
    getVideos.bindInt64(3, 0);

    Database::Item image;
    Database::PreparedStatement getImages(*m_connection);
    qb.select(Database::Objects).where("o", "PATH", "=", "?").andWhere("o", "FILE_NAME", "like", "?").andWhere("o", "TYPE", ">=", "?").andWhere("o", "TYPE", "<=", "?").andWhere("o", "DEVICE", "is", "NULL").andWhere("o", "REF_ID", "=", "?");
    qb.getQuery(getImages);

    while (getVideos.next(video)) {

        // check if an image with the same name exists
        getImages.clear();
        getImages.bindText(0, video.path);
        getImages.bindText(1, TruncateFileExt(video.filename) + ".%");
        getImages.bindInt64(2, ITEM_IMAGE_ITEM);
        getImages.bindInt64(3, ITEM_IMAGE_ITEM_MAX);
        getImages.bindInt64(4, 0);

        if (getImages.next(image)) {

            // set the image as the item's album art
            video.details->albumArtId = image.objectId;
            video.details->albumArtExtension = image.extension;
            video.details->albumArtWidth = image.details->iv_width;
            video.details->albumArtHeight = image.details->iv_height;
            objMgr.persist(*video.details);

            // hide the image
            image.visible = false;
            objMgr.persist(image);
            continue;
        }

        if (NULL == thumbnailer) {
            continue;
        }

        // create a thumbnail
        fuppes_off_t size = 0;
        unsigned char* buffer = (unsigned char*)malloc(1);
        char mimeType[100];
        bool hasImage;

        thumbnailer->openFile(video.path + video.filename);
        if ((hasImage = thumbnailer->readImage(&mimeType[0], &buffer, &size))) {
            std::stringstream tmpfile;
            tmpfile << m_configuration.getThumbnailDirectory() << video.objectId << ".jpg";

            fuppes::File out(tmpfile.str());
            out.open(fuppes::File::Write);
            out.write((char*)buffer, size);
            out.close();
        }
        thumbnailer->closeFile();
        free(buffer);

        // update the video details
        if (hasImage) {
            // set the album art id to the same value as the object id
            video.details->albumArtId = video.objectId;
            video.details->albumArtExtension = "jpg";
            video.details->albumArtMimeType = mimeType;

        }
        else {
            video.details->albumArtId = 0;
            video.details->albumArtExtension = "fail";
        }

        objMgr.persist(*video.details);
        count++;
    }

    if (NULL != thumbnailer) {
        delete thumbnailer;
    }

    return count;
}

/*void UpdateThread::parsePlaylists(SQLQuery* qry, SQLQuery* get, SQLQuery* ins)
 {*/

/*
 stringstream sql;
 sql << "select * from OBJECTS where TYPE = " << CONTAINER_PLAYLISTCONTAINER << " and (UPDATED_AT is NULL or UPDATED_AT < MODIFIED_AT)";

 BasePlaylistParser* parser;
 DbObject* playlist;
 DbObject* existing;

 qry->select(sql.str());
 while (!qry->eof() && !famEventOccurred()) {

 parser = BasePlaylistParser::Load(qry->result()->asString("PATH") + qry->result()->asString("FILE_NAME"));
 if(parser == NULL)
 continue;

 object_id_t playlistId = qry->result()->asUInt("OBJECT_ID");
 //object_id_t objectId   = 0;

 playlist = new DbObject(qry->result());

 while (!parser->Eof()) {
 if(parser->Entry()->bIsLocalFile && File::exists(parser->Entry()->sFileName)) {

 log(Logging::Log::contentdb, Logging::Log::normal)<< "PLENTRY: " << parser->Entry()->sFileName;

 existing = DbObject::createFromFileName(parser->Entry()->sFileName, get);
 if(existing == NULL) {
 //m_contentDb->insertFile(parser->Entry()->sFileName, playlistId, ins, false);
 CContentDatabase::incSystemUpdateId();
 }
 else {
 DbObject* object = new DbObject(existing);
 object->setObjectId(CContentDatabase::GetObjId());
 object->setParentId(playlistId);
 object->setRefId(existing->objectId());
 object->save(ins);
 delete object;
 delete existing;
 }
 }
 else if(!parser->Entry()->bIsLocalFile) {

 DbObject* object = new DbObject();
 object->setObjectId(CContentDatabase::GetObjId());
 object->setParentId(playlistId);
 object->setTitle(parser->Entry()->sTitle);
 object->setPath(parser->Entry()->sFileName);
 object->setType(ITEM_UNKNOWN_BROADCAST);
 object->save(ins);
 delete object;

 }

 parser->Next();
 }

 delete parser;

 playlist->setUpdated();
 playlist->save(ins);
 delete playlist;

 qry->next();

 }*/
//}
