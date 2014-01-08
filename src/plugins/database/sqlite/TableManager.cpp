/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include "TableManager.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "PreparedStatement.h"

using namespace Sqlite;

#include <iostream>

TableManager::TableManager(Database::AbstractConnection& connection) :
        Database::AbstractTableManager(connection)
{

}

TableManager::~TableManager()
{

}

bool TableManager::exist()
{
    PreparedStatement statement(m_connection);

    statement.prepare("select count(*) as COUNT from sqlite_master where name = ?");
    statement.bindText(0, "FUPPES_DB_INFO");

    if(false == statement.next()) {
        return false;
    }

    fuppes_int64_t count = sqlite3_column_int64(statement.m_statement, 0);
    return (1 == count);
}

bool TableManager::isVersion(int version)
{
    PreparedStatement statement(m_connection);
    statement.prepare("select version from FUPPES_DB_INFO");
    if(false == statement.next()) {
        return false;
    }

    fuppes_int64_t dbversion = sqlite3_column_int64(statement.m_statement, 0);
    return (dbversion == version);
}

bool TableManager::create(int version)
{
    PreparedStatement qry(m_connection);

    qry.prepare("CREATE TABLE OBJECTS ( "
            "  ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "  OBJECT_ID BIGINT NOT NULL, "
            "  PARENT_ID BIGINT NOT NULL, "
            "  DETAIL_ID BIGINT NOT NULL, "
            "  TYPE INTEGER NOT NULL, "
            "  PATH TEXT NOT NULL, "
            "  FILE_NAME TEXT DEFAULT NULL, "
            "  TITLE TEXT DEFAULT NULL COLLATE NOCASE, "
            "  EXT TEXT DEFAULT NULL, "
            "  MD5 TEXT DEFAULT NULL, "
            "  REF_ID BIGINT DEFAULT 0, "
            "  DEVICE TEXT DEFAULT NULL, "
            "  VCONTAINER_TYPE INTEGER DEFAULT 0, "
            "  VCONTAINER_PATH TEXT DEFAULT NULL, "
            "  VREF_ID BIGINT DEFAULT 0, "
            "  VISIBLE INTEGER DEFAULT 1, "
            "  SIZE BIGINT DEFAULT 0, "
            "  MODIFIED_AT INTEGER, "
            "  UPDATED_AT INTEGER, "
            "  unique(OBJECT_ID, DEVICE) "
            ") ");
    qry.execute();

    qry.prepare("CREATE TABLE OBJECT_DETAILS ( "
            "  ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "  PUBLISHER TEXT DEFAULT NULL, "
            "  DATE TEXT DEFAULT NULL, "
            "  DESCRIPTION TEXT DEFAULT NULL, "
            "  LONG_DESCRIPTION TEXT DEFAULT NULL, "
            "  AV_GENRE TEXT DEFAULT NULL, "
            "  AV_LANGUAGE TEXT DEFAULT NULL, "
            "  AV_ARTIST TEXT DEFAULT NULL, "
            "  AV_ALBUM TEXT DEFAULT NULL, "
            "  AV_CONTRIBUTOR TEXT DEFAULT NULL, "
            "  AV_PRODUCER TEXT DEFAULT NULL, "
            "  A_TRACK_NUMBER UNSIGNED INTEGER, "
            "  V_ACTORS TEXT DEFAULT NULL, "
            "  V_DIRECTOR TEXT DEFAULT NULL, "
            "  V_SERIES_TITLE TEXT DEFAULT NULL, "
            "  V_PROGRAM_TITLE TEXT DEFAULT NULL, "
            "  V_EPISODE_NR UNSIGNED INTEGER, "
            "  V_EPISODE_COUNT UNSIGNED INTEGER, "
            "  V_HAS_SUBTITLES_FILE INTEGER DEFAULT 0, "
            "  V_BITRATE INTEGER, "
            "  AV_DURATION INTEGER, "
            "  A_CHANNELS INTEGER, "
            "  A_BITRATE INTEGER, "
            "  A_BITS_PER_SAMPLE INTEGER, "
            "  A_SAMPLERATE INTEGER, "
            "  IV_WIDTH INTEGER, "
            "  IV_HEIGHT INTEGER, "
            "  IV_COLOR_DEPTH INTEGER, "
            "  A_YEAR INTEGER, "
            "  A_COMPOSER TEXT DEFAULT NULL, "
            "  AUDIO_CODEC TEXT DEFAULT NULL, "
            "  VIDEO_CODEC TEXT DEFAULT NULL, "
            "  ALBUM_ART_ID INTEGER, "
            "  ALBUM_ART_EXT TEXT DEFAULT NULL, "
            "  ALBUM_ART_MIME_TYPE TEXT DEFAULT NULL, "
            "  ALBUM_ART_WIDTH INTEGER, "
            "  ALBUM_ART_HEIGHT INTEGER, "
            "  STREAM_MIME_TYPE TEXT DEFAULT NULL, "
            "  SOURCE INT DEFAULT 0 ) ");
    qry.execute();

    qry.prepare("CREATE INDEX IDX_OBJECTS_OBJECT_ID ON OBJECTS(OBJECT_ID)");
    qry.execute();
    qry.prepare("CREATE INDEX IDX_OBJECTS_PARENT_ID ON OBJECTS(PARENT_ID)");
    qry.execute();
    qry.prepare("CREATE INDEX IDX_OBJECTS_DETAIL_ID ON OBJECTS(DETAIL_ID)");
    qry.execute();
    qry.prepare("CREATE INDEX IDX_OBJECTS_PATH ON OBJECTS(PATH)");
    qry.execute();
    qry.prepare("CREATE INDEX IDX_OBJECTS_FILE_NAME ON OBJECTS(FILE_NAME)");
    qry.execute();
    qry.prepare("CREATE INDEX IDX_OBJECTS_TITLE ON OBJECTS(TITLE)");
    qry.execute();
    qry.prepare("CREATE INDEX IDX_OBJECT_DETAILS_ID ON OBJECT_DETAILS(ID)");
    qry.execute();

    qry.prepare("CREATE TABLE FUPPES_DB_INFO (VERSION INTEGER NOT NULL )");
    qry.execute();


    qry.prepare("insert into FUPPES_DB_INFO (VERSION) values (?)");
    qry.bindInt64(0, version);
    qry.execute();

    return true;
}

bool TableManager::empty()
{
    PreparedStatement qry(m_connection);

    qry.prepare("DELETE FROM OBJECTS");
    qry.execute();

    qry.prepare("DELETE FROM OBJECT_DETAILS");
    qry.execute();

    return true;
}
