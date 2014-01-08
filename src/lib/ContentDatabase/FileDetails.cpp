/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            FileDetails.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "FileDetails.h"

//#include "../SharedConfig.h"
#include "../SharedLog.h"
//#include "../Transcoding/TranscodingMgr.h"
//#include "../DeviceSettings/DeviceIdentificationMgr.h"

#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

CFileDetails::CFileDetails(Plugin::Manager& pluginManager) :
        m_pluginManager(pluginManager)
{
    m_baseSettings = NULL;
}

void CFileDetails::setBaseDevice(CDeviceSettings* baseDevice)
{
    m_baseSettings = baseDevice;
}

object_type_t CFileDetails::GetObjectType(std::string p_sFileName)
{
    assert(NULL != m_baseSettings);
    string sExt = fuppes::File(p_sFileName).ext();
    return m_baseSettings->ObjectType(sExt);
}

/*
 std::string CFileDetails::GetObjectTypeAsStr(OBJECT_TYPE p_nObjectType)
 {
 assert((p_nObjectType > OBJECT_TYPE_UNKNOWN && p_nObjectType < CONTAINER_MAX) ||
 (p_nObjectType >= ITEM && p_nObjectType < ITEM_MAX));

 if(p_nObjectType < CONTAINER_MAX) {
 return GetContainerTypeAsStr(p_nObjectType);
 }

 switch(p_nObjectType) {

 case ITEM:
 return "object.item";
 case ITEM_IMAGE_ITEM :
 return "object.item.imageItem";
 case ITEM_IMAGE_ITEM_PHOTO :
 return "object.item.imageItem.photo";

 case ITEM_AUDIO_ITEM :
 return "object.item.audioItem";
 case ITEM_AUDIO_ITEM_MUSIC_TRACK :
 return "object.item.audioItem.musicTrack";
 case ITEM_AUDIO_ITEM_AUDIO_BROADCAST :
 return "object.item.audioItem.audioBroadcast";

 case ITEM_VIDEO_ITEM :
 return "object.item.videoItem";
 case ITEM_VIDEO_ITEM_MOVIE :
 return "object.item.videoItem.movie";
 case ITEM_VIDEO_ITEM_VIDEO_BROADCAST :
 return "object.item.videoItem.videoBroadcast";
 case ITEM_VIDEO_ITEM_MUSIC_VIDEO_CLIP:
 return "object.item.videoItem.musicVideoClip";

 default: {
 cout << "unknown object type: " << p_nObjectType << ". I'm going to crash now. Sorry!" << endl;
 assert(true == false);
 }
 }
 }


 std::string CFileDetails::GetContainerTypeAsStr(OBJECT_TYPE p_nContainerType)
 {

 //return typeToString(p_nContainerType);



 switch(p_nContainerType) {

 case CONTAINER:
 return "object.container";
 case CONTAINER_STORAGEFOLDER:
 return "object.container.storageFolder";

 case CONTAINER_PERSON :
 return "object.container.person";
 case CONTAINER_PERSON_MUSICARTIST :
 return "object.container.person.musicArtist";

 case CONTAINER_PLAYLISTCONTAINER :
 return "object.container.playlistContainer";

 case CONTAINER_ALBUM :
 return "object.container.album";
 case CONTAINER_ALBUM_MUSICALBUM :
 return "object.container.album.musicAlbum";
 case CONTAINER_ALBUM_PHOTOALBUM :
 return "object.container.album.photoAlbum";

 case CONTAINER_GENRE :
 return "object.container.genre";
 case CONTAINER_GENRE_MUSICGENRE :
 return "object.container.genre.musicGenre";
 case CONTAINER_GENRE_MOVIEGENRE :
 return "object.container.genre.movieGenre";

 default: {
 cout << "container type: " << p_nContainerType << ". I'm going to crash now. Sorry!" << endl;
 assert(true == false);
 }
 }

 }
 */

bool CFileDetails::IsSupportedFileExtension(std::string p_sFileExtension)
{
    p_sFileExtension = ToLower(p_sFileExtension);
    return m_baseSettings->Exists(p_sFileExtension);
}

bool CFileDetails::isAlbumArtFile(const std::string fileName)
{
    string name = ToLower(fileName);
    if(name.compare("cover.jpg") == 0 || name.compare("cover.png") == 0 || name.compare(".folder.jpg") == 0 || name.compare(".folder.png") == 0 || name.compare("folder.jpg") == 0 || name.compare("folder.png") == 0 || name.compare("front.jpg") == 0
            || name.compare("front.png") == 0)
        return true;

    return false;
}

std::string CFileDetails::getAlbumArtFiles()
{

	if (!m_albumArtFiles.empty()) {
		return m_albumArtFiles;
	}

    std::vector<std::string> ext;
    ext.push_back("jpg");
    ext.push_back("jpeg");
    ext.push_back("png");

    std::vector<std::string> file;
    file.push_back("cover");
    file.push_back(".folder");
    file.push_back("folder");
    file.push_back("front");
    file.push_back(".front");

    std::vector<std::string>::iterator iterExt;
    std::vector<std::string>::iterator iterFile;
    for(iterExt = ext.begin(); iterExt != ext.end(); iterExt++) {

        for(iterFile = file.begin(); iterFile != file.end(); iterFile++) {

            if(m_albumArtFiles.length() > 0)
            	m_albumArtFiles += ",";
            m_albumArtFiles += "'" + *iterFile + "." + *iterExt + "'";
        }
    }

    return m_albumArtFiles;
}

//bool CFileDetails::getMusicTrackDetails(std::string p_sFileName, AudioItem* audioItem)
bool CFileDetails::getMusicTrackDetails(std::string p_sFileName, Database::AbstractItem& item)
{
    string sExt = item.extension; //fuppes::File(p_sFileName).ext();
    if(!m_baseSettings->FileSettings(sExt)->ExtractMetadata()) {
        return false;
    }

    // passing wav files to taglib is pointless
    if(0 == sExt.compare("wav")) {
        return false;
    }

    //Plugins::MetadataPlugin* plugin = m_pluginMgr.createMetadataPlugin("taglib");
    Plugin::Metadata* plugin = m_pluginManager.createMetadataPlugin("taglib");
    if(NULL == plugin) {
        //cout << "TAGLIB NOT FOUND" << endl;
        return false;
    }

    bool result = false;
    if(plugin->openFile(p_sFileName)) {
        result = plugin->readMetadata(item); //audioItem->metadata());
        plugin->closeFile();
    }

    // get the image dimensions
    /*if(audioItem->hasImage() && (audioItem->imageWidth() == 0 || audioItem->imageHeight() == 0)) {
     // TODO: get image width and height
     }*/

    delete plugin;
    return result;
}

bool CFileDetails::getImageDetails(std::string p_sFileName, Database::AbstractItem& item)
{

    /*
     string sExt = fuppes::File(p_sFileName).ext();
     if(!CDeviceIdentificationMgr::Shared()->DefaultDevice()->FileSettings(sExt)->ExtractMetadata())
     return false;
     */

	/*

    Plugin::Metadata* image;
    bool result = false;

    std::string plugins[] = { "exiv2", "magickWand", "simage", "" };

    for(int i = 0; plugins[i].length() > 0; i++) {

        image = m_pluginManager.createMetadataPlugin(plugins[i]);
        if (NULL == image) {
            continue;
        }

        if(image->openFile(p_sFileName)) {
            result = image->readData2(item);
            image->closeFile();
        }
        delete image;
        image = NULL;

        if (result) {
            return true;
        }
    }
*/
    return false;
}

bool CFileDetails::getVideoDetails(std::string p_sFileName, Database::AbstractItem& item)
{
    string sExt = item.extension; //fuppes::File(p_sFileName).ext();
    if(!m_baseSettings->FileSettings(sExt)->ExtractMetadata()) {
        return false;
    }

    Plugin::Metadata* video = m_pluginManager.createMetadataPlugin("libavformat");
    if(NULL == video) {
        return false;
    }

    bool result = false;
    if(video->openFile(p_sFileName)) {
        result = video->readMetadata(item);
        video->closeFile();
    }

    delete video;
    return result;
}
