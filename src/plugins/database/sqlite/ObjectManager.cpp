/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "ObjectManager.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace Sqlite;

#include <iostream>

ObjectManager::ObjectManager(Sqlite::Connection& connection) :
        Database::AbstractObjectManager(connection)
{
    m_findChildren = NULL;

    m_insertItem = NULL;
    m_updateItem = NULL;

    m_insertDetails = NULL;
    m_updateDetails = NULL;
}

ObjectManager::~ObjectManager()
{
    if(NULL != m_findChildren) {
        delete m_findChildren;
    }

    if(NULL != m_insertItem) {
        delete m_insertItem;
    }

    if(NULL != m_updateItem) {
        delete m_updateItem;
    }

    if(NULL != m_insertDetails) {
        delete m_insertDetails;
    }

    if(NULL != m_updateDetails) {
        delete m_updateDetails;
    }
}

bool ObjectManager::getMaxObjectId(object_id_t& result)
{
    PreparedStatement statement(m_connection);
    std::string sql = "select max(OBJECT_ID) as MAX_ID from OBJECTS where DEVICE is NULL";

    if(false == statement.prepare(sql) || false == statement.next()) {
        return false;
    }

    result = sqlite3_column_int64(statement.m_statement, 0);
    return true;
}

bool ObjectManager::getAllContainerPaths(Database::AbstractQueryResult& result)
{
    std::string sql = "select o.PATH "
            "from OBJECTS o "
            "where "
            "o.VISIBLE = 1 and "
            "(o.TYPE >= ? and o.TYPE < ?) and "
            "o.DEVICE is NULL ";

    PreparedStatement statement(m_connection);
    statement.prepare(sql);

    statement.bindInt64(0, CONTAINER);
    statement.bindInt64(1, CONTAINER_MAX);

    return statement.select(result);
}

bool ObjectManager::findOne(const object_id_t objectId, const std::string device, Database::AbstractItem& item, bool withDetails)
{
    PreparedStatement statement(m_connection);

    std::string sql = (withDetails ? sqlSelectItemWithDetails : sqlSelectItemWithoutDetails);

    sql += "where "
            "  o.OBJECT_ID = ? and ";

    if(device.empty()) {
        sql += "o.DEVICE is NULL ";
    }
    else {
        sql += "o.DEVICE = ?";
    }

    if(false == statement.prepare(sql)) {
        return false;
    }

    statement.bindInt64(0, objectId);
    if(!device.empty()) {
        statement.bindText(1, device);
    }

    int ret = SQLITE_BUSY;
    while (true) {

        ret = sqlite3_step(statement.m_statement);
        if(SQLITE_ROW == ret) {
            if(false == assignItemValues(statement.m_statement, item, withDetails)) {
                return false;
            }
            continue;
        }

        break;
    }
    return (SQLITE_DONE == ret);
}

bool ObjectManager::findOneByFilename(const std::string path, const std::string filename, Database::AbstractItem& item, bool withDetails)
{
    PreparedStatement statement(m_connection);

    std::string sql = (withDetails ? sqlSelectItemWithDetails : sqlSelectItemWithoutDetails);

    sql += "where "
            "  o.REF_ID = 0 and "
            "  o.DEVICE is NULL and "
            "  o.PATH = ? ";

    if(filename.empty())
        sql += " and o.FILE_NAME is NULL ";
    else
        sql += " and o.FILE_NAME = ? ";

    if(false == statement.prepare(sql)) {
        return false;
    }

    statement.bindText(0, path);
    if(!filename.empty()) {
        statement.bindText(1, filename);
    }

    int ret = SQLITE_BUSY;
    while (true) {

        ret = sqlite3_step(statement.m_statement);
        if(SQLITE_ROW == ret) {
            if(false == assignItemValues(statement.m_statement, item, withDetails)) {
                return false;
            }
            continue;
        }

        break;
    }
    return (SQLITE_DONE == ret);
}

bool ObjectManager::countChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, Database::AbstractQueryResult& result)
{
    std::string sql = "select count(*) as COUNT "
            "from OBJECTS o "
            "where "
            "o.PARENT_ID = ? and "
            "o.VISIBLE = 1 and "
            "(o.TYPE < ? or (o.TYPE in (" + objectTypes + ") and o.EXT in (" + extensions + "))) and ";

    if(device.empty()) {
        sql += "o.DEVICE is NULL ";
    }
    else {
        sql += "o.DEVICE = ?";
    }

    PreparedStatement statement(m_connection);
    statement.prepare(sql);

    statement.bindInt64(0, objectId);
    statement.bindInt64(1, CONTAINER_MAX);

    if(!device.empty()) {
        statement.bindText(2, device);
    }

    return statement.select(result);
}

bool ObjectManager::findChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, const std::string orderBy, const int offset, const int limit, Database::AbstractItem& item)
{
    std::string sql = sqlSelectItemWithDetails + "where "
            "  o.PARENT_ID = ? and "
            "  o.VISIBLE = 1 and "
            "  (o.TYPE < ? or (o.TYPE in (" + objectTypes + ") and o.EXT in (" + extensions + "))) and ";

    if(device.empty()) {
        sql += "o.DEVICE is NULL ";
    }
    else {
        sql += "o.DEVICE = ? ";
    }

    sql += "order by " + orderBy;

    if(0 < limit || 0 < offset) {
        sql += " limit ?, ?";
    }

    m_findChildren = new PreparedStatement(m_connection);

    int idx = 0;
    m_findChildren->prepare(sql);
    m_findChildren->bindInt64(idx++, objectId);
    m_findChildren->bindInt64(idx++, CONTAINER_MAX);

    if(!device.empty()) {
        m_findChildren->bindText(idx++, device);
    }

    if(0 < limit || 0 < offset) {
        m_findChildren->bindInt64(idx++, offset);
        m_findChildren->bindInt64(idx++, ((0 == limit) ? -1 : limit));
    }

    return nextChild(item);
}

bool ObjectManager::nextChild(Database::AbstractItem& item)
{
    if(false == m_findChildren->next()) {
        return false;
    }

    return assignItemValues(m_findChildren->m_statement, item, true);
}

/*
 bool ObjectManager::findItems(const Database::AbstractObjectManager::FindWhat what, Database::AbstractItem& item, va_list args)
 {
 if(NULL != m_findItems) {
 delete m_findItems;
 }
 m_findItems = new PreparedStatement(m_connection);

 std::string sql;

 switch (what) {


 case Database::AbstractObjectManager::UpdatableItems:
 m_findItems->prepare(sqlSelectItemWithDetails + " where o.TYPE > ? and (o.UPDATED_AT is NULL or o.UPDATED_AT < o.MODIFIED_AT) and o.DEVICE is NULL and o.REF_ID = 0");
 m_findItems->bindInt64(0, ITEM);
 break;


 case Database::AbstractObjectManager::AlbumArtImages:
 m_findItems->prepare(sqlSelectItemWithDetails + " where o.TYPE >= ? and o.TYPE < ? and "
 "o.DEVICE is NULL and o.REF_ID = 0 and o.VISIBLE = 1 and "
 "lower(o.FILE_NAME) in (?) order by o.FILE_NAME, o.PARENT_ID");

 m_findItems->bindInt64(0, ITEM_IMAGE_ITEM);
 m_findItems->bindInt64(1, ITEM_IMAGE_ITEM_MAX);
 m_findItems->bindText(2, va_arg(args, char*));
 break;


 case Database::AbstractObjectManager::AudioItems:
 m_findItems->prepare(sqlSelectItemWithDetails + " where o.TYPE >= ? and o.TYPE < ? and o.PARENT_ID = ? and o.DEVICE is NULL");

 m_findItems->bindInt64(0, ITEM_AUDIO_ITEM);
 m_findItems->bindInt64(1, ITEM_AUDIO_ITEM_MAX);
 m_findItems->bindInt64(2, va_arg(args, object_id_t));
 break;



 case Database::AbstractObjectManager::VideoItemsWithoutThumbnails:
 m_findItems->prepare(sqlSelectItemWithDetails + " where o.TYPE >= ? and o.TYPE < ? and o.DEVICE is NULL and o.REF_ID = 0 and "
 "o.DETAIL_ID in (select ID from OBJECT_DETAILS where ALBUM_ART_ID = 0 and ALBUM_ART_EXT is NULL)");

 m_findItems->bindInt64(0, ITEM_VIDEO_ITEM);
 m_findItems->bindInt64(1, ITEM_VIDEO_ITEM_MAX);
 break;

 case Database::AbstractObjectManager::VideoThumbnail:

 m_findItems->prepare(sqlSelectItemWithDetails + " where o.TYPE >= ? and o.TYPE < ? and "
 "o.PATH = ? and o.FILE_NAME like ? and "
 "o.DEVICE is NULL and o.REF_ID = 0");

 m_findItems->bindInt64(0, ITEM_IMAGE_ITEM);
 m_findItems->bindInt64(1, ITEM_IMAGE_ITEM_MAX);
 m_findItems->bindText(2, va_arg(args, char*));
 m_findItems->bindText(3, va_arg(args, char*) + std::string(".%"));
 break;

 default:
 return false;
 }

 return findNext(item);
 }

 bool ObjectManager::findNext(Database::AbstractItem& item)
 {
 if(false == m_findItems->next()) {
 return false;
 }

 return assignItemValues(m_findItems->m_statement, item, true);
 }

 */

bool ObjectManager::persist(Database::AbstractItem& item)
{
    if(0 == item.id) {
        return insert(item);
    }
    else {
        return update(item);
    }
}

bool ObjectManager::insert(Database::AbstractItem& item)
{
    if(NULL == m_insertItem) {

        std::string sql = "insert into OBJECTS ("
                "OBJECT_ID, "
                "PARENT_ID, "
                "DETAIL_ID, "
                "TYPE, "
                "TITLE, "
                "PATH, "
                "FILE_NAME, "
                "EXT, "
                "REF_ID, "
                "DEVICE, "
                "VCONTAINER_TYPE, "
                "VCONTAINER_PATH, "
                "VREF_ID, "
                "VISIBLE, "
                "SIZE, "
                "MODIFIED_AT, "
                "UPDATED_AT "
                ") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

        m_insertItem = new PreparedStatement(m_connection);
        m_insertItem->prepare(sql);
    }

    int idx = 0;
    m_insertItem->clear();
    bindItemToStatement(item, m_insertItem);

    if(false == m_insertItem->execute()) {
        std::cout << "ObjectManager::insert Item :: ERROR" << std::endl;
        return false;
    }

    item.id = m_insertItem->getLastInsertId();
    return true;
}

bool ObjectManager::update(Database::AbstractItem& item)
{
    if(NULL == m_updateItem) {

        std::string sql = "update OBJECTS set "
                "OBJECT_ID = ?, "
                "PARENT_ID = ?, "
                "DETAIL_ID = ?, "
                "TYPE = ?, "
                "TITLE = ?, "
                "PATH = ?, "
                "FILE_NAME = ?, "
                "EXT = ?, "
                "REF_ID = ?, "
                "DEVICE = ?, "
                "VCONTAINER_TYPE = ?, "
                "VCONTAINER_PATH = ?, "
                "VREF_ID = ?, "
                "VISIBLE = ?, "
                "SIZE = ?, "
                "MODIFIED_AT = ?, "
                "UPDATED_AT = ? "
                "where ID = ?";
        m_updateItem = new PreparedStatement(m_connection);
        m_updateItem->prepare(sql);
    }


    m_updateItem->clear();
    int idx = bindItemToStatement(item, m_updateItem);
    m_updateItem->bindInt64(idx++, item.id);

    //std::cout << "ObjectManager::update Item" << std::endl;

    return m_updateItem->execute();
}

int ObjectManager::bindItemToStatement(Database::AbstractItem& item, Sqlite::PreparedStatement* statement)
{
    int idx = 0;

    statement->bindInt64(idx++, item.objectId);
    statement->bindInt64(idx++, item.parentId);
    statement->bindInt64(idx++, item.detailId);
    statement->bindInt64(idx++, item.type);
    statement->bindText(idx++, item.title);
    statement->bindText(idx++, item.path);
    item.filename.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, item.filename);
    item.extension.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, item.extension);
    statement->bindInt64(idx++, item.refId);
    item.device.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, item.device);
    statement->bindInt64(idx++, item.vcontainerType);
    item.vcontainerPath.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, item.vcontainerPath);
    statement->bindInt64(idx++, item.vrefId);
    item.visible ? statement->bindInt64(idx++, 1) : statement->bindInt64(idx++, 0);
    statement->bindInt64(idx++, item.size);
    statement->bindInt64(idx++, item.modified);
    statement->bindInt64(idx++, item.updated); // 0 == item.updated ? statement->bindNull(idx++) :

    return idx;
}

bool ObjectManager::assignItemValues(sqlite3_stmt* statement, Database::AbstractItem& item, bool withDetails) // static
{
    item.clear();

    int idx = 0;

    item.id = sqlite3_column_int64(statement, 0);
    item.objectId = sqlite3_column_int64(statement, 1);
    item.parentId = sqlite3_column_int64(statement, 2);
    item.detailId = sqlite3_column_int64(statement, 3);
    item.type = (object_type_t)sqlite3_column_int(statement, 4);

    if(SQLITE_NULL != sqlite3_column_type(statement, 16)) {
        item.path = (const char*)sqlite3_column_text(statement, 16);
    }

    if(SQLITE_NULL != sqlite3_column_type(statement, 17)) {
        item.filename = (const char*)sqlite3_column_text(statement, 17);
    }

    item.title = (const char*)sqlite3_column_text(statement, 5);
    if(SQLITE_NULL != sqlite3_column_type(statement, 6)) {
        item.extension = (const char*)sqlite3_column_text(statement, 6);
    }

    item.refId = sqlite3_column_int64(statement, 7);
    if(SQLITE_NULL != sqlite3_column_type(statement, 8)) {
        item.device = (const char*)sqlite3_column_text(statement, 8);
    }

    item.vcontainerType = sqlite3_column_int(statement, 9);
    if(SQLITE_NULL != sqlite3_column_type(statement, 10)) {
        item.vcontainerPath = (const char*)sqlite3_column_text(statement, 10);
    }
    item.vrefId = sqlite3_column_int64(statement, 11);
    item.visible = (1 == sqlite3_column_int(statement, 12));
    item.size = sqlite3_column_int64(statement, 13);
    item.modified = sqlite3_column_int(statement, 14);
    if(SQLITE_NULL != sqlite3_column_type(statement, 15)) {
        item.updated = sqlite3_column_int(statement, 15);
    }

    if(false == withDetails) {
        return true;
    }

    if(NULL == item.details) {
        return false;
    }

    idx = 18;

    Database::AbstractItemDetails* details = item.details;

    details->id = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->publisher = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->date = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->description = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->longDescription = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";

    details->av_genre = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->av_language = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->av_artist = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->av_album = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->av_contributor = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->av_producer = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->av_duration = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;

    details->iv_width = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->iv_height = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->iv_depth = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;

    details->a_trackNumber = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->a_channels = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->a_bitrate = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->a_bitsPerSample = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->a_samplerate = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;

    details->a_year = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->a_composer = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";

    details->v_actors = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->v_director = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->v_seriesTitle = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->v_programTitle = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->v_episodeNumber = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->v_episodeCount = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->v_hasSubtitles = (1 == sqlite3_column_int64(statement, idx++));
    details->v_bitrate = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;

    details->audioCodec = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->videoCodec = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";

    details->albumArtId = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->albumArtExtension = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->albumArtMimeType = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";
    details->albumArtWidth = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->albumArtHeight = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;

    details->source = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? sqlite3_column_int64(statement, idx - 1) : 0;
    details->streamMimeType = (SQLITE_NULL != sqlite3_column_type(statement, idx++)) ? (const char*)sqlite3_column_text(statement, idx - 1) : "";

    return true;
}

bool ObjectManager::persist(Database::AbstractItemDetails& details)
{
    if(0 == details.id) {
        return insert(details);
    }
    else {
        return update(details);
    }
}

bool ObjectManager::insert(Database::AbstractItemDetails& details)
{
    if(NULL == m_insertDetails) {

        std::string sql = "insert into OBJECT_DETAILS ("
                "PUBLISHER, "
                "DATE, "
                "DESCRIPTION, "
                "LONG_DESCRIPTION, "
                "AV_GENRE, "
                "AV_LANGUAGE, "
                "AV_ARTIST, "
                "AV_ALBUM, "
                "AV_CONTRIBUTOR, "
                "AV_PRODUCER, "

                "AV_DURATION, "
                "IV_WIDTH, "
                "IV_HEIGHT, "
                "IV_COLOR_DEPTH, "
                "A_TRACK_NUMBER, "
                "A_CHANNELS, "
                "A_BITRATE, "
                "A_BITS_PER_SAMPLE, "
                "A_SAMPLERATE, "
                "A_YEAR, "

                "A_COMPOSER, "
                "V_ACTORS, "
                "V_DIRECTOR, "
                "V_SERIES_TITLE, "
                "V_PROGRAM_TITLE, "
                "V_EPISODE_NR, "
                "V_EPISODE_COUNT, "
                "V_HAS_SUBTITLES_FILE, "
                "V_BITRATE, "
                "AUDIO_CODEC, "

                "VIDEO_CODEC, "
                "ALBUM_ART_ID, "
                "ALBUM_ART_EXT, "
                "ALBUM_ART_MIME_TYPE, "
                "ALBUM_ART_WIDTH, "
                "ALBUM_ART_HEIGHT, "
                "SOURCE, "
                "STREAM_MIME_TYPE "
                ") values ("
                "?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                "?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                "?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                "?, ?, ?, ?, ?, ?, ?, ?"
                ")";

        m_insertDetails = new PreparedStatement(m_connection);
        m_insertDetails->prepare(sql);
    }

    m_insertDetails->clear();
    bindDetailsToStatement(details, m_insertDetails);

    if(false == m_insertDetails->execute()) {
        std::cout << "ObjectManager::insert Details :: ERROR" << std::endl;
        return false;
    }

    details.id = m_insertDetails->getLastInsertId();
    return true;
}

bool ObjectManager::update(Database::AbstractItemDetails& details)
{
    if(NULL == m_updateDetails) {

        std::string sql = "update OBJECT_DETAILS set "
                "PUBLISHER = ?, "
                "DATE = ?, "
                "DESCRIPTION = ?, "
                "LONG_DESCRIPTION = ?, "
                "AV_GENRE = ?, "
                "AV_LANGUAGE = ?, "
                "AV_ARTIST = ?, "
                "AV_ALBUM = ?, "
                "AV_CONTRIBUTOR = ?, "
                "AV_PRODUCER = ?, "
                "AV_DURATION = ?, "
                "IV_WIDTH = ?, "
                "IV_HEIGHT = ?, "
                "IV_COLOR_DEPTH = ?, "
                "A_TRACK_NUMBER = ?, "
                "A_CHANNELS = ?, "
                "A_BITRATE = ?, "
                "A_BITS_PER_SAMPLE = ?, "
                "A_SAMPLERATE = ?, "
                "A_YEAR = ?, "
                "A_COMPOSER = ?, "
                "V_ACTORS = ?, "
                "V_DIRECTOR = ?, "
                "V_SERIES_TITLE = ?, "
                "V_PROGRAM_TITLE = ?, "
                "V_EPISODE_NR = ?, "
                "V_EPISODE_COUNT = ?, "
                "V_HAS_SUBTITLES_FILE = ?, "
                "V_BITRATE = ?, "
                "AUDIO_CODEC = ?, "
                "VIDEO_CODEC = ?, "
                "ALBUM_ART_ID = ?, "
                "ALBUM_ART_EXT = ?, "
                "ALBUM_ART_MIME_TYPE = ?, "
                "ALBUM_ART_WIDTH = ?, "
                "ALBUM_ART_HEIGHT = ?, "
                "SOURCE = ?, "
                "STREAM_MIME_TYPE = ? "
                "where ID = ?";
        m_updateDetails = new PreparedStatement(m_connection);
        m_updateDetails->prepare(sql);
    }

    m_updateDetails->clear();
    int idx = bindDetailsToStatement(details, m_updateDetails);
    m_updateDetails->bindInt64(idx++, details.id);

    return m_updateDetails->execute();
}

int ObjectManager::bindDetailsToStatement(Database::AbstractItemDetails& details, Sqlite::PreparedStatement* statement)
{
    int idx = 0;

    details.publisher.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.publisher);
    details.date.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.date);
    details.description.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.description);
    details.longDescription.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.longDescription);

    details.av_genre.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.av_genre);
    details.av_language.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.av_language);
    details.av_artist.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.av_artist);
    details.av_album.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.av_album);
    details.av_contributor.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.av_contributor);
    details.av_producer.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.av_producer);
    statement->bindInt64(idx++, details.av_duration);

    statement->bindInt64(idx++, details.iv_width);
    statement->bindInt64(idx++, details.iv_height);
    statement->bindInt64(idx++, details.iv_depth);

    statement->bindInt64(idx++, details.a_trackNumber);
    statement->bindInt64(idx++, details.a_channels);
    statement->bindInt64(idx++, details.a_bitrate);
    statement->bindInt64(idx++, details.a_bitsPerSample);
    statement->bindInt64(idx++, details.a_samplerate);

    statement->bindInt64(idx++, details.a_year);
    details.a_composer.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.a_composer);

    details.v_actors.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.v_actors);
    details.v_director.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.v_director);
    details.v_seriesTitle.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.v_seriesTitle);
    details.v_programTitle.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.v_programTitle);
    statement->bindInt64(idx++, details.v_episodeNumber);
    statement->bindInt64(idx++, details.v_episodeCount);
    statement->bindInt64(idx++, (details.v_hasSubtitles ? 1 : 0));
    statement->bindInt64(idx++, details.v_bitrate);

    details.audioCodec.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.audioCodec);
    details.videoCodec.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.videoCodec);

    statement->bindInt64(idx++, details.albumArtId);
    details.albumArtExtension.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.albumArtExtension);
    details.albumArtMimeType.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.albumArtMimeType);
    statement->bindInt64(idx++, details.albumArtWidth);
    statement->bindInt64(idx++, details.albumArtHeight);

    statement->bindInt64(idx++, details.source);
    details.streamMimeType.empty() ? statement->bindNull(idx++) : statement->bindText(idx++, details.streamMimeType);

    return idx;
}
