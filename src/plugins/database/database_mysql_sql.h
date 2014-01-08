/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            database_mysql_sql.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#include "../../include/fuppes_db_connection_plugin.h"


static const fuppes_sql mysql_sql[] = {

  {SQL_UNKNOWN, ""},
  
  {SQL_COUNT_CHILD_OBJECTS,
  "select count(*) as COUNT "
  "from OBJECTS  "
  "where "
	"PARENT_ID = %OBJECT_ID% and "
	"VISIBLE = 1 and "
	"%DEVICE%"
  },
  
  {SQL_GET_CHILD_OBJECTS,
  "select "
  "  o.OBJECT_ID, o.TYPE, o.PATH, o.FILE_NAME, o.TITLE, "
  "  d.IV_HEIGHT, d.IV_WIDTH, d.DATE, d.AV_DURATION, "
  "  d.A_ALBUM, d.A_ARTIST, d.A_GENRE, d.A_TRACK_NO, "
	"  d.A_BITRATE, d.A_SAMPLERATE, d.A_BITS_PER_SAMPLE, d.A_CHANNELS, d.AV_DURATION, "
  "  d.SIZE, d.A_CODEC, d.V_CODEC, d.V_BITRATE, d.DLNA_PROFILE, d.ALBUM_ART_ID, d.ALBUM_ART_EXT "
  "from "
  "  OBJECTS o "
  "  left join OBJECT_DETAILS d on (d.ID = o.DETAIL_ID) "
  "where "
  "  o.PARENT_ID = %OBJECT_ID% and "
	"  o.%DEVICE% and "
	"  o.VISIBLE = 1 "
  "order by "
  },

  {SQL_GET_OBJECT_TYPE,
  "select TYPE from OBJECTS "
  "where OBJECT_ID = %OBJECT_ID% and %DEVICE%"
  },

  {SQL_GET_OBJECT_DETAILS,
  "select "
  "  o.OBJECT_ID, o.PARENT_ID, o.TITLE, o.TYPE, o.DEVICE, "
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
  "d.* "
  "from "
  "  OBJECTS o "
  "  left join OBJECT_DETAILS d on (d.ID = o.DETAIL_ID) "
  "where "
  "  o.OBJECT_ID = %OBJECT_ID% and "
  "  o.%DEVICE% "
  },    
/*
  {SQL_GET_ALBUM_ART_DETAILS,  
  "select "
  "  IV_HEIGHT, "
  "  IV_WIDTH, "
  "  ALBUM_ART_EXT "
  "from "
  "  OBJECT_DETAILS d "
  "where "
  "  d.ID = %OBJECT_ID% "
  },
*/
  {SQL_SEARCH_PART_SELECT_FIELDS,
  "select * "
  },

  {SQL_SEARCH_PART_SELECT_COUNT,
  "select count(*) as COUNT "
  },
  
  {SQL_SEARCH_PART_FROM,
  "from "
  "  OBJECTS o "
  "  left join OBJECT_DETAILS d on (d.ID = o.DETAIL_ID) "
  "where "
  "  o.%DEVICE% "
  },
  
  {SQL_SEARCH_GET_CHILDREN_OBJECT_IDS,
  "select OBJECT_ID from OBJECTS "
  "where "
  "  PARENT_ID in (%OBJECT_ID%) and "
  "  %DEVICE% "
  },

  
  {SQL_TABLES_EXIST,
  "show table status"
  },

  
  {SQL_CREATE_TABLE_DB_INFO,
  "CREATE TABLE FUPPES_DB_INFO ( VERSION INTEGER NOT NULL )"
  },

  {SQL_SET_DB_INFO,
  "insert into FUPPES_DB_INFO (VERSION) values (%OBJECT_ID%)"
  },


  {SQL_CREATE_TABLE_OBJECTS,
    "CREATE TABLE OBJECTS ( "
	  "  ID INTEGER PRIMARY KEY AUTO_INCREMENT, "
	  "  OBJECT_ID BIGINT NOT NULL, "
	  "  PARENT_ID BIGINT NOT NULL, "
	  "  DETAIL_ID BIGINT NOT NULL, "
	  "  TYPE INTEGER NOT NULL, "
	  "  PATH TEXT NOT NULL, "
	  "  FILE_NAME TEXT DEFAULT NULL, "
	  "  TITLE TEXT DEFAULT NULL, "
    "  EXT TEXT DEFAULT NULL, "
	  "  MD5 TEXT DEFAULT NULL, "
	  "  MIME_TYPE TEXT DEFAULT NULL, "
	  "  REF_ID BIGINT DEFAULT 0, "
    "  DEVICE TEXT DEFAULT NULL, "
    "  VCONTAINER_TYPE INTEGER DEFAULT 0, "
    "  VCONTAINER_PATH TEXT DEFAULT NULL, "
   	"  VREF_ID BIGINT DEFAULT 0, "
	  "  VISIBLE INTEGER DEFAULT 1, "
    "  CHANGED_AT INTEGER, "
    "  UPDATED_AT INTEGER ) "
    "ENGINE=MyISAM  DEFAULT CHARSET=utf8;"
  },

  {SQL_CREATE_TABLE_OBJECT_DETAILS,
    "CREATE TABLE OBJECT_DETAILS ( "
    "  ID INTEGER PRIMARY KEY AUTO_INCREMENT, "
    "  AV_DURATION INTEGER, "
    "  A_ALBUM TEXT, "
    "  A_ARTIST TEXT, "
    "  A_CHANNELS INTEGER, " 
    "  A_DESCRIPTION TEXT, "
    "  A_GENRE TEXT, "
    "  A_COMPOSER TEXT, "
    "  A_SAMPLERATE INTEGER, "
    "  A_BITS_PER_SAMPLE INTEGER, "
    "  A_BITRATE INTEGER, "
    "  A_TRACK_NO INTEGER, "
    "  DATE TEXT, "
    "  IV_HEIGHT INTEGER, " 
    "  IV_WIDTH INTEGER, "
    "  A_CODEC TEXT, " 
    "  V_CODEC TEXT, "
    "  V_BITRATE INTEGER, "
    "  ALBUM_ART_ID INTEGER, "
    "  ALBUM_ART_EXT TEXT, "
    "  SIZE BIGINT DEFAULT 0, "
    "  DLNA_PROFILE TEXT DEFAULT NULL, "
    "  DLNA_MIME_TYPE TEXT DEFAULT NULL, "
    "  SOURCE INT DEFAULT 0 ) "
    "ENGINE=MyISAM  DEFAULT CHARSET=utf8;"
  },
  
  {SQL_CREATE_INDICES,
    ""
  },

  
  {SQL_GET_OBJECT_TYPE_COUNT,
    "select TYPE, count(*) as VALUE from OBJECTS group by TYPE"
  },
  

  
};

 
 
 
 
 
