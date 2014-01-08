/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            DeviceConfigFile.cpp
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "DeviceConfigFile.h"

#include "DeviceSettings.h"
#include "IdentificationMgr.h"

#include "../Common/XMLParser.h"
#include "../Common/File.h"
#include "../Log/Log.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace fuppes;

CDeviceConfigFile::CDeviceConfigFile()
{
}

CDeviceConfigFile::~CDeviceConfigFile()
{
}

//int CDeviceConfigFile::SetupDevice(PathFinder* pathFinder, CDeviceSettings* pSettings, string deviceName)
int CDeviceConfigFile::setupDevice(const std::string filename, CDeviceSettings* pSettings, const std::string deviceName)
{
    assert(pSettings != NULL);

    int isError = SETUP_DEVICE_SUCCESS;
    if(filename.empty()) {
        isError = SETUP_DEVICE_NOT_FOUND;
    }
    else {
        CXMLDocument deviceConfig;
        if(deviceConfig.LoadFromFile(filename)) {
            // actually parse the files. This may fail and therefore it should be error checked.
            ParseDeviceSettings(deviceConfig.RootNode(), pSettings);
        }
        else {
            isError = SETUP_DEVICE_LOAD_FAILED;
        }
    }

    return isError;
}

OBJECT_TYPE ParseObjectType(std::string p_sObjectType)
{
    if(p_sObjectType.compare("AUDIO_ITEM") == 0) {
        return ITEM_AUDIO_ITEM;
    }
    else if(p_sObjectType.compare("AUDIO_ITEM_MUSIC_TRACK") == 0) {
        return ITEM_AUDIO_ITEM_MUSIC_TRACK;
    }

    else if(p_sObjectType.compare("IMAGE_ITEM") == 0) {
        return ITEM_IMAGE_ITEM;
    }
    else if(p_sObjectType.compare("IMAGE_ITEM_PHOTO") == 0) {
        return ITEM_IMAGE_ITEM_PHOTO;
    }

    else if(p_sObjectType.compare("VIDEO_ITEM") == 0) {
        return ITEM_VIDEO_ITEM;
    }
    else if(p_sObjectType.compare("VIDEO_ITEM_MOVIE") == 0) {
        return ITEM_VIDEO_ITEM_MOVIE;
    }

    else if(p_sObjectType.compare("PLAYLIST") == 0) {
        return CONTAINER_PLAYLISTCONTAINER;
    }

    else
        return OBJECT_TYPE_UNKNOWN;
}

void CDeviceConfigFile::ParseDeviceSettings(CXMLNode* pDevice, CDeviceSettings* pSettings)
{
    assert(pDevice != NULL);
    assert(pSettings != NULL);
    // this better be a device and it will be the only device we look at
    assert(pDevice->Name().compare("device") == 0);

    // settings
    CXMLNode* pTmp;
    for(int j = 0; j < pDevice->ChildCount(); j++) {
        pTmp = pDevice->ChildNode(j);

        // playlist_style
        if(pTmp->Name().compare("playlist_style") == 0) {
            //pSettings->m_bShowPlaylistAsContainer = (pTmp->Value().compare("container") == 0);

            if(pTmp->Value().compare("container") == 0)
                pSettings->m_playlistStyle = CDeviceSettings::container;
            else if(pTmp->Value().compare("file") == 0)
                pSettings->m_playlistStyle = CDeviceSettings::file;
            else if(pTmp->Value().compare("pls") == 0)
                pSettings->m_playlistStyle = CDeviceSettings::pls;
            else if(pTmp->Value().compare("m3u") == 0)
                pSettings->m_playlistStyle = CDeviceSettings::m3u;
            else if(pTmp->Value().compare("wpl") == 0)
                pSettings->m_playlistStyle = CDeviceSettings::wpl;
            else if(pTmp->Value().compare("xspf") == 0)
                pSettings->m_playlistStyle = CDeviceSettings::xspf;
        }
        // max_file_name_length
        else if(pTmp->Name().compare("max_file_name_length") == 0) {
            pSettings->DisplaySettings()->nMaxFileNameLength = atoi(pTmp->Value().c_str());
        }
        // show_childcount_in_title
        else if(pTmp->Name().compare("show_childcount_in_title") == 0) {
            pSettings->DisplaySettings()->bShowChildCountInTitle = (pTmp->Value().compare("true") == 0);
        }
        // xbox360
        else if(pTmp->Name().compare("xbox360") == 0) {
            pSettings->m_bXBox360Support = (pTmp->Value().compare("true") == 0);
            // xbox implies	media_receiver_registrar
            if(pSettings->m_bXBox360Support)
                pSettings->MediaServerSettings()->UseXMSMediaReceiverRegistrar = true;
        }
        // media_receiver_registrar
        else if(pTmp->Name().compare("enable_xms_media_receiver_registrar") == 0) {
            pSettings->MediaServerSettings()->UseXMSMediaReceiverRegistrar = (pTmp->Value().compare("true") == 0);
        }
        // enable_url_base
        else if(pTmp->Name().compare("enable_url_base") == 0) {
            if(pTmp->Value().compare("true") == 0)
                pSettings->MediaServerSettings()->UseURLBase = true;
            else if(pTmp->Value().compare("false") == 0)
                pSettings->MediaServerSettings()->UseURLBase = false;
        }
        // dlna
        else if(pTmp->Name().compare("enable_dlna") == 0) {
            //pSettings->MediaServerSettings()->UseDLNA = (pTmp->Value().compare("true") == 0);
            log("config") << Log::Log::warning << "\"enable_dlna\" option is deprecated. use \"dlna_version\" instead";
        }
        // dlna version
        else if(pTmp->Name().compare("dlna_version") == 0) {
            if(pTmp->Value().compare("none") == 0)
                pSettings->MediaServerSettings()->DlnaVersion = CMediaServerSettings::dlna_none;
            else if(pTmp->Value().compare("1.0") == 0)
                pSettings->MediaServerSettings()->DlnaVersion = CMediaServerSettings::dlna_1_0;
            else if(pTmp->Value().compare("1.5") == 0)
                pSettings->MediaServerSettings()->DlnaVersion = CMediaServerSettings::dlna_1_5;
        }
        // transcoding_release_delay
        else if(pTmp->Name().compare("transcoding_release_delay") == 0) {
            pSettings->nDefaultReleaseDelay = atoi(pTmp->Value().c_str());
        }
        // show_device_icon
        else if(pTmp->Name().compare("show_device_icon") == 0) {
            pSettings->m_bEnableDeviceIcon = (pTmp->Value().compare("true") == 0);
        }
        // show_empty_resolution
        else if(pTmp->Name().compare("show_empty_resolution") == 0) {
            pSettings->m_bShowEmptyResolution = (pTmp->Value().compare("true") == 0);
        }

        // rewrite_urls
        else if(pTmp->Name().compare("rewrite_urls") == 0) {
            pSettings->MediaServerSettings()->rewriteUrls = (pTmp->Value().compare("true") == 0);
        }

        // file_settings
        else if(pTmp->Name().compare("file_settings") == 0) {

            // if the copy attribute is false we disable all file settings for the
            // device so only the settings contained in this config will be reenabled.
            // if true: a config simply inherits all file settings from base
            // if false: settings are still inherited but disabled by default
            if(pTmp->attribute("copy").compare("false") == 0) {

                FileSettingsIterator_t iter;
                iter = pSettings->m_FileSettings.begin();
                for(; iter != pSettings->m_FileSettings.end(); iter++) {
                    iter->second->bEnabled = false;
                }
            }

            for(int k = 0; k < pTmp->ChildCount(); k++) {
                if(pTmp->ChildNode(k)->Name().compare("file") != 0)
                    continue;

                ParseFileSettings(pTmp->ChildNode(k), pSettings);
            }
        }
        // description_values
        else if(pTmp->Name().compare("description_values") == 0) {
            ParseDescriptionValues(pTmp, pSettings);
        }

    }
}

void CDeviceConfigFile::ParseFileSettings(CXMLNode* pFileSettings, CDeviceSettings* pDevSet)
{
    assert(pFileSettings != NULL);
    assert(pDevSet != NULL);

    int i;
    CFileSettings* pFileSet;
    CXMLNode* pTmp;

    StringList extList = String::split(ToLower(pFileSettings->Attribute("ext")), ",");
    if(extList.size() == 0)
        return;

    pFileSet = pDevSet->FileSettings(extList.at(0));
    pFileSet->sExt = extList.at(0);
    //pFileSet->bEnabled = true;

    if(pFileSettings->Attribute("extract_metadata").compare("false") == 0) {
        pFileSet->bExtractMetadata = false;
    }

    pFileSet->bEnabled = (pFileSettings->Attribute("enabled").compare("false") != 0);

    // cout << pDevSet->name() << " :: " << pFileSet->sExt << " " << (pFileSet->bEnabled ? "enabled" : "disabled") << endl;

    for(i = 0; i < pFileSettings->ChildCount(); i++) {

        pTmp = pFileSettings->ChildNode(i);

        if(pTmp->Name().compare("type") == 0) {
            pFileSet->nType = ParseObjectType(pTmp->Value());
        }
        /*
         else if(pTmp->Name().compare("ext") == 0) {
         pDevSet->AddExt(pFileSet, ToLower(pTmp->Value()));
         }*/
        else if(pTmp->Name().compare("mime_type") == 0) {
            pFileSet->sMimeType = pTmp->Value();
        }
        /*else if(pTmp->Name().compare("dlna") == 0) {
         pFileSet->sDLNA = pTmp->Value();
         }*/
        else if(pTmp->Name().compare("transcode") == 0) {
            ParseTranscodingSettings(pTmp, pFileSet);
        }
        else if(pTmp->Name().compare("convert") == 0) {
            ParseImageSettings(pTmp, pFileSet);
        }

    }

    // copy settings for other listed file extensions
    for(unsigned int i = 1; i < extList.size(); i++) {
        pDevSet->AddExt(pFileSet, extList.at(i));
    }

}

void CDeviceConfigFile::ParseTranscodingSettings(CXMLNode* pTCNode, CFileSettings* pFileSet)
{
    assert(pTCNode != NULL);
    assert(pFileSet != NULL);

    // delete existing (inherited) settings
    // if transcoding disabled
    if(pTCNode->Attribute("enabled").compare("true") != 0) {
        if(pFileSet->pTranscodingSettings) {
            delete pFileSet->pTranscodingSettings;
            pFileSet->pTranscodingSettings = NULL;
            return;
        }
    }

    // create new settings
    if(!pFileSet->pTranscodingSettings) {
        pFileSet->pTranscodingSettings = new CTranscodingSettings();
    }

    bool bExt = false;

    // read transcoding settings
    CXMLNode* pTmp;
    for(int i = 0; i < pTCNode->ChildCount(); i++) {
        pTmp = pTCNode->ChildNode(i);

        if(pTmp->Name().compare("ext") == 0) {
            pFileSet->pTranscodingSettings->sExt = pTmp->Value();
            bExt = true;
        }
        else if(pTmp->Name().compare("mime_type") == 0) {
            pFileSet->pTranscodingSettings->sMimeType = pTmp->Value();
        }
        /*else if(pTmp->Name().compare("dlna") == 0) {
         pFileSet->pTranscodingSettings->sDLNA = pTmp->Value();
         }*/
        else if(pTmp->Name().compare("http_encoding") == 0) {
            if(pTmp->Value().compare("chunked") == 0) {
                pFileSet->pTranscodingSettings->nTranscodingResponse = RESPONSE_CHUNKED;
            }
            else if(pTmp->Value().compare("stream") == 0) {
                pFileSet->pTranscodingSettings->nTranscodingResponse = RESPONSE_STREAM;
            }
        }
        else if(pTmp->Name().compare("out_params") == 0) {
            pFileSet->pTranscodingSettings->sOutParams = pTmp->Value();
        }
        else if(pTmp->Name().compare("encoder") == 0) {

            pFileSet->pTranscodingSettings->m_encoderName = ToLower(pTmp->Value());
            /*if(pTmp->Value().compare("lame") == 0)
             pFileSet->pTranscodingSettings->nEncoderType = ET_LAME;
             else if(pTmp->Value().compare("twolame") == 0)
             pFileSet->pTranscodingSettings->nEncoderType = ET_TWOLAME;
             else if(pTmp->Value().compare("wav") == 0) {
             pFileSet->pTranscodingSettings->nEncoderType = ET_WAV;
             pFileSet->pTranscodingSettings->nAudioSampleRate = 44100;
             pFileSet->pTranscodingSettings->nAudioBitRate = 176400;
             }
             else if(pTmp->Value().compare("pcm") == 0) {
             pFileSet->pTranscodingSettings->nEncoderType = ET_PCM;
             pFileSet->pTranscodingSettings->nAudioSampleRate = 44100;
             pFileSet->pTranscodingSettings->nAudioBitRate = 176400;
             }
             else
             pFileSet->pTranscodingSettings->nEncoderType = ET_NONE;*/

        }
        else if(pTmp->Name().compare("decoder") == 0) {

            pFileSet->pTranscodingSettings->m_decoderName = ToLower(pTmp->Value());
            /*
             if(pTmp->Value().compare("vorbis") == 0)
             pFileSet->pTranscodingSettings->nDecoderType = DT_OGG_VORBIS;
             else if(pTmp->Value().compare("flac") == 0)
             pFileSet->pTranscodingSettings->nDecoderType = DT_FLAC;
             else if(pTmp->Value().compare("mpc") == 0)
             pFileSet->pTranscodingSettings->nDecoderType = DT_MUSEPACK;
             else if(pTmp->Value().compare("faad") == 0)
             pFileSet->pTranscodingSettings->nDecoderType = DT_FAAD;
             else if(pTmp->Value().compare("mad") == 0)
             pFileSet->pTranscodingSettings->nDecoderType = DT_MAD;
             else
             pFileSet->pTranscodingSettings->nDecoderType = DT_NONE;
             */
        }
        else if(pTmp->Name().compare("transcoder") == 0) {

            pFileSet->pTranscodingSettings->m_transcoderName = ToLower(pTmp->Value());

            /*if(pTmp->Value().compare("ffmpeg") == 0) {
             pFileSet->pTranscodingSettings->nTranscoderType = TTYP_FFMPEG;
             }
             else
             if(pTmp->Value().compare("external") == 0) {
             pFileSet->pTranscodingSettings->nTranscoderType = TTYP_EXTERNAL_CMD;
             }
             else {
             pFileSet->pTranscodingSettings->nTranscoderType = TTYP_NONE;
             }
             */

        }

        /*else if(pTmp->Name().compare("video_codec") == 0) {
         pFileSet->pTranscodingSettings->sVCodecCondition = pTmp->Attribute("vcodec");
         pFileSet->pTranscodingSettings->sVCodec = pTmp->Value();
         }
         else if(pTmp->Name().compare("video_bitrate") == 0) {
         pFileSet->pTranscodingSettings->nVideoBitRate = atoi(pTmp->Value().c_str());
         }
         else if(pTmp->Name().compare("audio_codec") == 0) {
         pFileSet->pTranscodingSettings->sACodecCondition = pTmp->Attribute("acodec");
         pFileSet->pTranscodingSettings->sACodec = pTmp->Value();
         }
         else if(pTmp->Name().compare("ffmpeg_params") == 0) {
         pFileSet->pTranscodingSettings->sFFmpegParams = pTmp->Value();
         }*/
        else if(pTmp->Name().compare("cmd") == 0) {
            pFileSet->pTranscodingSettings->sExternalCmd = pTmp->Value();
        }
        else if((pTmp->Name().compare("bitrate") == 0) || pTmp->Name().compare("audio_bitrate") == 0) {
            pFileSet->pTranscodingSettings->nAudioBitRate = atoi(pTmp->Value().c_str());
        }
        else if((pTmp->Name().compare("samplerate") == 0) || pTmp->Name().compare("audio_samplerate") == 0) {
            pFileSet->pTranscodingSettings->nAudioSampleRate = atoi(pTmp->Value().c_str());
        }
        else if(pTmp->Name().compare("lame_quality") == 0) {
            pFileSet->pTranscodingSettings->nLameQuality = atoi(pTmp->Value().c_str());
        }

    }

    // no de/encoders selected but extension available => rename file
    if((pFileSet->pTranscodingSettings->m_decoderName.empty()) && (pFileSet->pTranscodingSettings->m_encoderName.empty())
            && (pFileSet->pTranscodingSettings->m_transcoderName.empty()) && bExt) {

        pFileSet->pTranscodingSettings->nTranscodingType = TT_RENAME;
    }

    // de- and encoder selected => threaded de/encoder
    else if(!pFileSet->pTranscodingSettings->m_decoderName.empty() && !pFileSet->pTranscodingSettings->m_encoderName.empty()) {
        pFileSet->pTranscodingSettings->nTranscodingType = TT_THREADED_DECODER_ENCODER;
    }
    // transcoder selected => threaded transcoder
    else if(!pFileSet->pTranscodingSettings->m_transcoderName.empty()) {
        pFileSet->pTranscodingSettings->nTranscodingType = TT_THREADED_TRANSCODER;
    }
    else {
        pFileSet->pTranscodingSettings->nTranscodingType = TT_NONE;
    }
}

void CDeviceConfigFile::ParseImageSettings(CXMLNode* pISNode, CFileSettings* pFileSet)
{
    assert(pISNode != NULL);
    assert(pFileSet != NULL);

    // if transcoding disabled ...
    if(pISNode->Attribute("enabled").compare("true") != 0) {
        // ... delete existing (inherited) settings
        if(pFileSet->pImageSettings) {
            delete pFileSet->pImageSettings;
            pFileSet->pImageSettings = NULL;
        }
        return;
    }

    // create new settings
    if(!pFileSet->pImageSettings) {
        pFileSet->pImageSettings = new CImageSettings();
        pFileSet->pImageSettings->bEnabled = (pISNode->Attribute("enabled").compare("true") == 0);
    }

    // read image settings
    CXMLNode* pTmp;
    for(int i = 0; i < pISNode->ChildCount(); i++) {
        pTmp = pISNode->ChildNode(i);

        if(pTmp->Name().compare("ext") == 0) {
            pFileSet->pImageSettings->sExt = pTmp->Value();
        }
        else if(pTmp->Name().compare("mime_type") == 0) {
            pFileSet->pImageSettings->sMimeType = pTmp->Value();
        }
        else if(pTmp->Name().compare("dcraw") == 0) {
            if(pTmp->Attribute("enabled").compare("false") != 0) {
                pFileSet->pImageSettings->bDcraw = true;
            }
            pFileSet->pImageSettings->sDcrawParams = pTmp->Value();
        }
        /*else if(pTmp->Name().compare("scale") == 0) {
         for(j = 0; j < pTmp->ChildCount(); j++) {
         pChild = pTmp->ChildNode(j);*/

        else if(pTmp->Name().compare("height") == 0) {
            pFileSet->pImageSettings->nHeight = atoi(pTmp->Value().c_str());
        }
        else if(pTmp->Name().compare("width") == 0) {
            pFileSet->pImageSettings->nWidth = atoi(pTmp->Value().c_str());
        }
        else if(pTmp->Name().compare("greater") == 0) {
            pFileSet->pImageSettings->bGreater = (pTmp->Value().compare("true") == 0);
        }
        else if(pTmp->Name().compare("less") == 0) {
            pFileSet->pImageSettings->bLess = (pTmp->Value().compare("true") == 0);
        }
        //}
        //}
    }
}

void CDeviceConfigFile::ParseDescriptionValues(CXMLNode* pDescrValues, CDeviceSettings* pDevSet)
{
    assert(pDescrValues != NULL);
    assert(pDevSet != NULL);

    string nodeName;
    for(int i = 0; i < pDescrValues->ChildCount(); i++) {
        // friendly_name
        nodeName = pDescrValues->ChildNode(i)->Name();
        if(nodeName.compare("friendly_name") == 0) {
            pDevSet->MediaServerSettings()->FriendlyName = pDescrValues->ChildNode(i)->Value();
        }
        // manufacturer
        else if(nodeName.compare("manufacturer") == 0) {
            pDevSet->MediaServerSettings()->Manufacturer = pDescrValues->ChildNode(i)->Value();
        }
        // manufacturer_url
        else if(nodeName.compare("manufacturer_url") == 0) {
            pDevSet->MediaServerSettings()->ManufacturerURL = pDescrValues->ChildNode(i)->Value();
        }
        // model_name
        else if(nodeName.compare("model_name") == 0) {
            pDevSet->MediaServerSettings()->ModelName = pDescrValues->ChildNode(i)->Value();
        }
        // model_number
        else if(nodeName.compare("model_number") == 0) {
            pDevSet->MediaServerSettings()->ModelNumber = pDescrValues->ChildNode(i)->Value();
        }
        // model_url
        else if(nodeName.compare("model_url") == 0) {
            pDevSet->MediaServerSettings()->ModelURL = pDescrValues->ChildNode(i)->Value();
        }
        // model_description
        else if(nodeName.compare("model_description") == 0) {
            pDevSet->MediaServerSettings()->ModelDescription = pDescrValues->ChildNode(i)->Value();

            if(pDescrValues->ChildNode(i)->Attribute("enabled").compare("true") == 0)
                pDevSet->MediaServerSettings()->UseModelDescription = true;
            else if(pDescrValues->ChildNode(i)->Attribute("enabled").compare("false") == 0)
                pDevSet->MediaServerSettings()->UseModelDescription = false;
        }
        // serial_number
        else if(nodeName.compare("serial_number") == 0) {
            pDevSet->MediaServerSettings()->SerialNumber = pDescrValues->ChildNode(i)->Value();

            if(pDescrValues->ChildNode(i)->Attribute("enabled").compare("true") == 0)
                pDevSet->MediaServerSettings()->UseSerialNumber = true;
            else if(pDescrValues->ChildNode(i)->Attribute("enabled").compare("false") == 0)
                pDevSet->MediaServerSettings()->UseSerialNumber = false;
        }
        // upc
        else if(nodeName.compare("upc") == 0) {
            pDevSet->MediaServerSettings()->UPC = pDescrValues->ChildNode(i)->Value();

            if(pDescrValues->ChildNode(i)->Attribute("enabled").compare("true") == 0)
                pDevSet->MediaServerSettings()->UseUPC = true;
            else if(pDescrValues->ChildNode(i)->Attribute("enabled").compare("false") == 0)
                pDevSet->MediaServerSettings()->UseUPC = false;
        }
    } // for
}
