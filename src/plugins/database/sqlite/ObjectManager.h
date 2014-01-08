/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _SQLITE_OBJECTMANAGER_H
#define _SQLITE_OBJECTMANAGER_H

#include <fuppes_database_plugin.h>
#include "Connection.h"
#include "PreparedStatement.h"
#include <sqlite3.h>


const std::string sqlSelectItemWithDetails = "select "
        "  o.ID, o.OBJECT_ID, o.PARENT_ID, o.DETAIL_ID, o.TYPE, o.TITLE, o.EXT, o.REF_ID, "
        "  o.DEVICE, o.VCONTAINER_TYPE, o.VCONTAINER_PATH, o.VREF_ID, "
        "  o.VISIBLE, o.SIZE, o.MODIFIED_AT, o.UPDATED_AT, "
        "CASE "
        "  WHEN o.DEVICE is NULL "
        "  THEN o.PATH "
        "  ELSE  (select PATH from OBJECTS where DEVICE is NULL and OBJECT_ID = o.VREF_ID) "
        "END AS PATH, "
        "CASE "
        "  WHEN o.DEVICE is NULL "
        "  THEN o.FILE_NAME "
        "  ELSE  (select FILE_NAME from OBJECTS where DEVICE is NULL and OBJECT_ID = o.VREF_ID) "
        "END AS FILE_NAME, "
        " d.id, d.PUBLISHER, d.DATE, d.DESCRIPTION, d.LONG_DESCRIPTION, "
        " d.AV_GENRE, d.AV_LANGUAGE, d.AV_ARTIST, d.AV_ALBUM, d.AV_CONTRIBUTOR, d.AV_PRODUCER, d.AV_DURATION,"
        " d.IV_WIDTH, d.IV_HEIGHT, d.IV_COLOR_DEPTH, "
        " d.A_TRACK_NUMBER, d.A_CHANNELS, d.A_BITRATE, d.A_BITS_PER_SAMPLE, d.A_SAMPLERATE, d.A_YEAR, d.A_COMPOSER, "
        " d.V_ACTORS, d.V_DIRECTOR, d.V_SERIES_TITLE, d.V_PROGRAM_TITLE, d.V_EPISODE_NR, d.V_EPISODE_COUNT, d.V_HAS_SUBTITLES_FILE, d.V_BITRATE, "
        " d.AUDIO_CODEC, d.VIDEO_CODEC, "
        " d.ALBUM_ART_ID, d.ALBUM_ART_EXT, d.ALBUM_ART_MIME_TYPE, d.ALBUM_ART_WIDTH, d.ALBUM_ART_HEIGHT, "
        " d.SOURCE, d.STREAM_MIME_TYPE "
        "from "
        "  OBJECTS o "
        "  left join OBJECT_DETAILS d on (d.ID = o.DETAIL_ID) ";

const std::string sqlSelectItemWithoutDetails = "select "
        "  o.ID, o.OBJECT_ID, o.PARENT_ID, o.DETAIL_ID, o.TYPE, o.TITLE, o.EXT, o.REF_ID, "
        "  o.DEVICE, o.VCONTAINER_TYPE, o.VCONTAINER_PATH, o.VREF_ID, "
        "  o.VISIBLE, o.SIZE, o.MODIFIED_AT, o.UPDATED_AT, "
        "CASE "
        "  WHEN o.DEVICE is NULL "
        "  THEN o.PATH "
        "  ELSE  (select PATH from OBJECTS where DEVICE is NULL and OBJECT_ID = o.VREF_ID) "
        "END AS PATH, "
        "CASE "
        "  WHEN o.DEVICE is NULL "
        "  THEN o.FILE_NAME "
        "  ELSE  (select FILE_NAME from OBJECTS where DEVICE is NULL and OBJECT_ID = o.VREF_ID) "
        "END AS FILE_NAME "
        "from "
        "  OBJECTS o ";


namespace Sqlite
{
    class ObjectManager: public Database::AbstractObjectManager
    {
        public:
            ObjectManager(Sqlite::Connection& connection);
            ~ObjectManager();

            bool getMaxObjectId(object_id_t& result);
            bool getAllContainerPaths(Database::AbstractQueryResult& result);

            bool findOne(const object_id_t objectId, const std::string device, Database::AbstractItem& item, bool withDetails);
            bool findOneByFilename(const std::string path, const std::string filename, Database::AbstractItem& item, bool withDetails);

            bool countChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, Database::AbstractQueryResult& result);
            bool findChildren(const object_id_t objectId, const std::string device, const std::string objectTypes, const std::string extensions, const std::string orderBy, const int offset, const int limit, Database::AbstractItem& result);
            bool nextChild(Database::AbstractItem& result);

            bool persist(Database::AbstractItem& item);
            bool persist(Database::AbstractItemDetails& details);

            static bool assignItemValues(sqlite3_stmt*, Database::AbstractItem& item, bool withDetails);

        private:
            Sqlite::PreparedStatement* m_findChildren;
            //Sqlite::PreparedStatement* m_findItems;

            Sqlite::PreparedStatement* m_insertItem;
            Sqlite::PreparedStatement* m_updateItem;

            Sqlite::PreparedStatement* m_insertDetails;
            Sqlite::PreparedStatement* m_updateDetails;


            bool insert(Database::AbstractItem& item);
            bool update(Database::AbstractItem& item);
            int bindItemToStatement(Database::AbstractItem& item, Sqlite::PreparedStatement* statement);

            bool insert(Database::AbstractItemDetails& details);
            bool update(Database::AbstractItemDetails& details);
            int bindDetailsToStatement(Database::AbstractItemDetails& details, Sqlite::PreparedStatement* statement);
    };
}

#endif // _SQLITE_OBJECTMANAGER_H
