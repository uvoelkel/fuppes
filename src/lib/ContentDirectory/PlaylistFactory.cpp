/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PlaylistFactory.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2006-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
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
 
#include "PlaylistFactory.h"
#include <sstream>

#include "DatabaseConnection.h"
#include "../UPnP/UPnPObjectTypes.h"

#include "../Common/Common.h"
#include "../Common/XMLParser.h"
//#include "../SharedConfig.h"
//#include "../Fuppes.h"

#include <libxml/xmlwriter.h>
#include <iostream>

using namespace std;


PlaylistFactory::PlaylistFactory(std::string httpServerUrl)
{
  m_httpServerUrl = httpServerUrl;
}


std::string PlaylistFactory::BuildPlaylist(std::string objectId, std::string ext)
{
  if(ext.compare("pls") == 0)
    return BuildPLS(objectId);
  else if(ext.compare("m3u") == 0)
    return BuildM3U(objectId);
  else if(ext.compare("wpl") == 0)
    return BuildWPL(objectId);
  else if(ext.compare("xspf") == 0)
    return BuildXSPF(objectId);

  return "";
}

// todo: audio details
std::string PlaylistFactory::BuildPLS(std::string p_sObjectId)
{
  std::stringstream sResult;  
  CSQLResult*    pRes      = NULL;
  unsigned int      nObjectId = HexToInt(p_sObjectId);
	SQLQuery qry;
  OBJECT_TYPE       nObjectType = OBJECT_TYPE_UNKNOWN;
  
	int nNumber = 0;
  sResult << "[playlist]\r\n";

  string sql = ""; //qry.build(SQL_GET_CHILD_OBJECTS, nObjectId);
  sql += " A_TRACK_NO ";
  qry.select(sql);
	while(!qry.eof()) {    
    pRes = qry.result();
		
    char szItemId[11];         
    unsigned int nItemId = pRes->asInt("OBJECT_ID");
    sprintf(szItemId, "%010X", nItemId);
    
    nObjectType = (OBJECT_TYPE)pRes->asInt("TYPE");
    switch(nObjectType)
    {
      case ITEM_AUDIO_ITEM:
      case ITEM_AUDIO_ITEM_MUSIC_TRACK:        
        sResult << "File" << nNumber + 1 << "=";
        sResult << "http://" << m_httpServerUrl << "/MediaServer/AudioItems/" <<
                   szItemId << "." << ExtractFileExt(pRes->asString("FILE_NAME")) << "\r\n";      
        sResult << "Title" << nNumber + 1 << "=" << TruncateFileExt(pRes->asString("FILE_NAME")) << "\r\n";
        nNumber++;
        break;
      
      case ITEM_AUDIO_ITEM_AUDIO_BROADCAST:
      case ITEM_VIDEO_ITEM_VIDEO_BROADCAST:
        sResult << "File" << nNumber + 1 << "=";
        sResult << pRes->asString("FILE_NAME") << "\r\n";
        nNumber++;
        break;
				
			default:
			  break;
    }
    
		qry.next();
  }  
  
  sResult << "NumberOfEntries=" << nNumber << "\r\n" <<
             "Version=2\r\n";
  return sResult.str();  
}

std::string PlaylistFactory::BuildM3U(std::string p_sObjectId)
{
  std::stringstream sResult;  
  CSQLResult*    pRes      = NULL;
  unsigned int      nObjectId = HexToInt(p_sObjectId);
	SQLQuery qry;
	OBJECT_TYPE       nObjectType = OBJECT_TYPE_UNKNOWN;

  bool extM3u = true;

  if(extM3u) {
    sResult << "#EXTM3U\r\n";
  }

  
  // get the information out of the database and place every item in the file
  string sql = ""; //qry.build(SQL_GET_CHILD_OBJECTS, nObjectId);
  sql += " A_TRACK_NO ";
  qry.select(sql);
  while(!qry.eof()) {
    pRes = qry.result();
		
    char szItemId[11];         
    unsigned int nItemId = pRes->asInt("OBJECT_ID");
    sprintf(szItemId, "%010X", nItemId);
    
    nObjectType = (OBJECT_TYPE)pRes->asInt("TYPE");
    switch(nObjectType)
    {
      case ITEM_AUDIO_ITEM:
      case ITEM_AUDIO_ITEM_MUSIC_TRACK:

        if(extM3u) {
          // #EXTINF:seconds,title
          sResult << "#EXTINF:" << (pRes->asInt("AV_DURATION") / 1000) << "," <<
            pRes->asString("A_ARTIST") << " - " << pRes->asString("TITLE") << "\r\n";
        }
        
        sResult << "http://" << m_httpServerUrl << "/MediaServer/AudioItems/" <<
                   szItemId << "." << ExtractFileExt(pRes->asString("FILE_NAME")) << "\r\n";      
        break;
				
			default:
			  break;
    }

    qry.next();
  }
		
  return sResult.str();  
}

/*
<?wpl version="1.0"?>
<smil>
    <head>
        <meta name="Generator" content="Microsoft Windows Media Player -- 11.0.5721.5145"/>
        <meta name="AverageRating" content="33"/>
        <meta name="TotalDuration" content="1102"/>
        <meta name="ItemCount" content="3"/>
        <author/>
        <title>Bach Organ Works</title>
    </head>
    <body>
        <seq>
            <media src="\\server\vol\music\Classical\Bach\OrganWorks\cd03\track01.mp3"/>
            <media src="\\server\vol\music\Classical\Bach\OrganWorks\cd03\track02.mp3"/>
            <media src="\\server\vol\music\Classical\Bach\OrganWorks\cd03\track03.mp3"/>
        </seq>
    </body>
</smil>
*/

// TODO use the XML API to do this instead
std::string PlaylistFactory::BuildWPL(std::string p_sObjectId)
{
  stringstream ssResult, ssMedia;
  CSQLResult* pRes = NULL;
  unsigned int      nObjectId = HexToInt(p_sObjectId);
	SQLQuery qry;
	OBJECT_TYPE       nObjectType = OBJECT_TYPE_UNKNOWN;

  // generate header
  ssResult << "<?wpl version=\"1.0\"?><smil><head>"
            << "<meta name=\"Generator\" content=\"Microsoft Windows Media Player -- 11.0.5721.5145\"/>";

  // for every object in the database, add it to the XML
  string sql = ""; //qry.build(SQL_GET_CHILD_OBJECTS, nObjectId);
  sql += " A_TRACK_NO ";
  qry.select(sql); 
  int itemCount = 0;
  while(!qry.eof()) {
    pRes = qry.result();
		
    char szItemId[11];         
    unsigned int nItemId = pRes->asInt("OBJECT_ID");
    // 010X says => print uppercase hex number with a min of ten characters and left padded with zeroes
    sprintf(szItemId, "%010X", nItemId);
    
    nObjectType = (OBJECT_TYPE)pRes->asInt("TYPE");
    switch(nObjectType)
    {
      case ITEM_AUDIO_ITEM:
      case ITEM_AUDIO_ITEM_MUSIC_TRACK:        
        ssMedia << "<media src=\"" 
                << "http://" << m_httpServerUrl
                << "/MediaServer/AudioItems/" << szItemId 
                << "." << ExtractFileExt(pRes->asString("FILE_NAME")) 
                << "\"/>\r\n";      
        break;
				
			default:
			  break;
    }
    ++itemCount;
		qry.next();
  }

  // finish the XML file
  ssResult  << "<meta name=\"AverageRating\" content=\"0\"/>"
            << "<meta name=\"ItemCount\" content=\"" << itemCount << "\"/>"
            << "</head><body><seq>"
            << ssMedia
            << "</seq></body></smil>";

  return ssResult.str();
}



/*
<?xml version="1.0" encoding="UTF-8"?>
<playlist version="1" xmlns="http://xspf.org/ns/0/">
    <trackList>
        <track>
            <location>http://example.com/song_1.mp3</location>

            <!-- artist or band name -->
            <creator>Led Zeppelin</creator>

            <!-- album title -->
            <album>Houses of the Holy</album>

            <!-- name of the song -->
            <title>No Quarter</title>

            <!-- comment on the song -->
            <annotation>I love this song</annotation>

            <!-- song length, in milliseconds -->
            <duration>271066</duration>

            <!-- album art -->
            <image>http://images.amazon.com/images/P/B000002J0B.01.MZZZZZZZ.jpg</image>

            <!-- if this is a deep link, URL of the original web page -->
            <info>http://example.com</info>

        </track>
    </trackList>
</playlist>
*/  
 

std::string PlaylistFactory::BuildXSPF(std::string p_sObjectId)
{
  unsigned int  nObjectId = HexToInt(p_sObjectId);
	SQLQuery qry;

  xmlTextWriterPtr writer;
	xmlBufferPtr buf;
	
	buf    = xmlBufferCreate();   
	writer = xmlNewTextWriterMemory(buf, 0);    
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  
  // playlist
  xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "playlist", BAD_CAST "http://xspf.org/ns/0/");

  // trackList
  xmlTextWriterStartElement(writer, BAD_CAST "trackList");

  
  string sql = ""; //qry.build(SQL_GET_CHILD_OBJECTS, nObjectId);
  sql += " A_TRACK_NO ";
  qry.select(sql); 

  string location;
  while(!qry.eof()) {

    OBJECT_TYPE type = (OBJECT_TYPE)qry.result()->asInt("TYPE");
    location = "http://" + m_httpServerUrl;

    if(type >= ITEM_AUDIO_ITEM && type < ITEM_AUDIO_ITEM_MAX) {
      location += "/MediaServer/AudioItems/";
    }
    else if(type >= ITEM_IMAGE_ITEM && type < ITEM_IMAGE_ITEM_MAX) {
      ///MediaServer/ImageItems/
      //cout << "request image from audio file " << sPath << endl;
      //audioFile = true;
    }
    else if(type >= ITEM_VIDEO_ITEM && type < ITEM_VIDEO_ITEM_MAX) {
      //cout << "request image from video file " << sPath << endl;
      //videoFile = true;
      ///MediaServer/VideoItems/
    }
    else {
      qry.next();
      continue;
    }

    char szObjId[11];    
    unsigned int nObjId = qry.result()->asUInt("OBJECT_ID");
    sprintf(szObjId, "%010X", nObjId);

    location += szObjId;
    location += "." + ExtractFileExt(qry.result()->asString("FILE_NAME"));


    
    
    // track
    xmlTextWriterStartElement(writer, BAD_CAST "track");
    
      // location
      xmlTextWriterStartElement(writer, BAD_CAST "location");
      xmlTextWriterWriteString(writer, BAD_CAST location.c_str());      
      xmlTextWriterEndElement(writer);

      // artist/creator
      if(!qry.result()->isNull("A_ARTIST")) {
        xmlTextWriterStartElement(writer, BAD_CAST "creator");
        xmlTextWriterWriteString(writer, BAD_CAST qry.result()->asString("A_ARTIST").c_str());      
        xmlTextWriterEndElement(writer);        
      }

      // album
      if(!qry.result()->isNull("A_ALBUM")) {
        xmlTextWriterStartElement(writer, BAD_CAST "album");
        xmlTextWriterWriteString(writer, BAD_CAST qry.result()->asString("A_ALBUM").c_str());      
        xmlTextWriterEndElement(writer);        
      }

      // title
      if(!qry.result()->isNull("TITLE")) {
        xmlTextWriterStartElement(writer, BAD_CAST "title");
        xmlTextWriterWriteString(writer, BAD_CAST qry.result()->asString("TITLE").c_str());      
        xmlTextWriterEndElement(writer);        
      }

      // comment/annotation

      // duration (in ms)
      if(!qry.result()->isNull("AV_DURATION")) {
        xmlTextWriterStartElement(writer, BAD_CAST "duration");
        xmlTextWriterWriteString(writer, BAD_CAST qry.result()->asString("AV_DURATION").c_str());
        xmlTextWriterEndElement(writer);
      }


      // albumArt/image
      if(!qry.result()->isNull("ALBUM_ART_ID")) {
        xmlTextWriterStartElement(writer, BAD_CAST "image");

        char szAlbumId[11];
        sprintf(szAlbumId, "%010X",  qry.result()->asUInt("ALBUM_ART_ID"));
        
        string url = "http://" + m_httpServerUrl + "/MediaServer/ImageItems/";
        url += szAlbumId + qry.result()->asString("ALBUM_ART_ID") + "." + qry.result()->asString("ALBUM_ART_EXT");
        
        xmlTextWriterWriteString(writer, BAD_CAST url.c_str());
        xmlTextWriterEndElement(writer);
      }
    
    
    xmlTextWriterEndElement(writer); // track
    
    qry.next();
  }

  xmlTextWriterEndElement(writer); // trackList
  xmlTextWriterEndElement(writer); // playlist
  xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);

  string result = (const char*)buf->content;
	xmlBufferFree(buf);  

  return result;  
}
