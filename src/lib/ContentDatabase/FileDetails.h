/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            FileDetails.h
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

#ifndef _FILEDETAILS_H
#define _FILEDETAILS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_plugin.h>
#include <fuppes_database_objects.h>
#include "../Common/Common.h"
#include "../UPnP/UPnPObjectTypes.h"
#include "../Plugin/Manager.h"
#include "../DeviceIdentification/DeviceSettings.h"
#include <string>

/*

 Property Name                   NS             Data Type             M-Val      Reference
 res                            DIDL-Lite      xsd:anyURI            NO         Appendix B.2.1
 res@protocolInfo               DIDL-Lite      xsd:string            NO         Appendix B.2.1.1
 res@size                       DIDL-Lite      xsd:unsignedLong      NO         Appendix B.2.1.3
 res@duration                   DIDL-Lite      xsd:string            NO         Appendix B.2.1.4
 res@bitrate                    DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.6
 res@bitsPerSample              DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.7
 res@sampleFrequency            DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.8
 res@nrAudioChannels            DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.9
 res@resolution                 DIDL-Lite      xsd:string            NO         Appendix B.2.1.10
 res@colorDepth                  DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.11

 

 dc:title
 dc:creator
 upnp:artist
 upnp:actor
 upnp:author
 upnp:producer
 upnp:director
 dc:publisher
 dc:contributor
 upnp:genre
 upnp:album
 upnp:playlist
 upnp:albumArtURI
 upnp:artistDiscographyURI
 upnp:lyricsURI
 
 dc:description
 upnp:longDescription
 upnp:icon
 upnp:region
 dc:date
 dc:language



 upnp:originalTrackNumber

 */

struct FileType_t
{
        std::string sExt;
        OBJECT_TYPE nObjectType;
        std::string sMimeType;
        std::string sDLNA;
};

/*
class BaseItem
{
    public:
        BaseItem()
        {
            init_metadata(&m_metadata);
        }

        ~BaseItem()
        {
            free_metadata(&m_metadata);
        }

        struct metadata_t* metadata()
        {
            return &m_metadata;
        }

        std::string path()
        {
            return TrimWhiteSpace(m_metadata.path);
        }
        std::string fileName()
        {
            return TrimWhiteSpace(m_metadata.file_name);
        }
        std::string mimeType()
        {
            return TrimWhiteSpace(m_metadata.mime_type);
        }

        std::string title()
        {
            return TrimWhiteSpace(m_metadata.title);
        }
        std::string publisher()
        {
            return TrimWhiteSpace(m_metadata.publisher);
        }
        std::string date()
        {
            return TrimWhiteSpace(m_metadata.date);
        }
        std::string description()
        {
            return TrimWhiteSpace(m_metadata.description);
        }
        std::string longDescription()
        {
            return TrimWhiteSpace(m_metadata.long_description);
        }

    protected:
        struct metadata_t m_metadata;
};
*/

/*
 audioItem:item Properties
 Table C-6:
 Property Name                           NS                 R/O      Remarks
 upnp:genre                              upnp               O
 dc:description                          dc                 O
 upnp:longDescription                    upnp               O
 dc:publisher                            dc                 O
 dc:language                             dc                 O
 dc:relation                             dc                 O
 dc:rights                               dc                 O


 Table C-7:       musicTrack:audioItem Properties
 Property Name                           NS                 R/O      Remarks
 upnp:artist                             upnp               O
 upnp:album                              upnp               O
 upnp:originalTrackNumber              upnp                O
 upnp:playlist                         upnp                O
 upnp:storageMedium                    upnp                O
 dc:contributor                        dc                  O
 dc:date                               dc                  O


 audioBroadcast:audioItem Properties
 Table C-8:
 Property Name                         NS                  R/O      Remarks
 upnp:region                           upnp                O
 upnp:radioCallSign                    upnp                O
 upnp:radioStationID                   upnp                O
 upnp:radioBand                        upnp                O
 upnp:channelNr                        upnp                O
 upnp:signalStrength                   upnp                O
 upnp:signalLocked                     upnp                O
 upnp:tuned                            upnp                O
 upnp:recordable                       upnp                O


 audioBook:audioItem Properties
 Table C-9:
 Property Name                         NS                  R/O      Remarks
 upnp:storageMedium                    upnp                O
 upnp:producer                         upnp                O
 dc:contributor                        dc                  O
 dc:date                               dc                  O


 */

/*
 class AudioItem: public BaseItem
 {
 public:
 // item:audioItem
 std::string   genre() { return m_metadata.genre; }
 std::string   language() { return m_metadata.language; }


 // item:audioItem:musicTrack
 std::string   artist() { return m_metadata.artist; }
 std::string   album() { return m_metadata.album; }
 unsigned int  originalTrackNumber() { return m_metadata.track_number; }
 std::string   contributor() { return m_metadata.contributor; }


 // item:audioItem:audioBroadcast
 // we don't support any of the additional broadcast properties


 // item:audioItem:audioBook
 std::string   producer() { return m_metadata.producer; }


 // res
 fuppes_off_t  size() { return m_metadata.size; }
 unsigned int  durationMs() { return m_metadata.duration_ms; }
 unsigned int  nrAudioChannels() { return m_metadata.nr_audio_channels; }
 unsigned int  bitrate() { return m_metadata.audio_bitrate; }
 unsigned int  bitsPerSample() { return m_metadata.audio_bits_per_sample; }
 unsigned int  sampleFrequency() { return m_metadata.audio_sample_frequency; }

 // other
 unsigned int  year() { return m_metadata.year; }
 std::string   composer() { return m_metadata.composer; }
 std::string   audioCodec() { return m_metadata.audio_codec; }
 bool          hasImage() { return (m_metadata.has_image == 1); }
 std::string   imageMimeType() { return m_metadata.image_mime_type; }
 int           imageWidth() { return m_metadata.image_width; }
 int           imageHeight() { return m_metadata.image_height; }
 };
 */

/*
 imageItem:item Properties
 Table C-4:
 Property Name                     NS              R/O     Remarks
 upnp:longDescription              upnp            O
 upnp:storageMedium                upnp            O
 upnp:rating                       upnp            O
 dc:description                        dc                O
 dc:publisher                          dc                O
 dc:date                               dc                O
 dc:rights                             dc                O

 photo:imageItem Properties
 Table C-5:
 Property Name                         NS                R/O      Remarks
 upnp:album                            upnp              O

 */

/*
class ImageItem: public BaseItem
{
    public:
        // item:imageItem:photo
        std::string album()
        {
            return m_metadata.album;
        }

        // res
        fuppes_off_t size()
        {
            return m_metadata.size;
        }
        unsigned int width()
        {
            return m_metadata.width;
        }
        unsigned int height()
        {
            return m_metadata.height;
        }
        unsigned int colorDepth()
        {
            return m_metadata.color_depth;
        }
};

*/

/*

 Table C-10: videoItem:item Properties
 Property Name                          NS                  R/O      Remarks
 upnp:genre                             upnp                O
 upnp:longDescription                   upnp                O
 upnp:producer                          upnp                O
 upnp:rating                            upnp                O
 upnp:actor                             upnp                O
 upnp:director                          upnp                O
 dc:description                         dc                  O
 dc:publisher                           dc                  O
 dc:language                            dc                  O
 dc:relation                            dc                  O
 upnp:playbackCount                     upnp                O
 upnp:lastPlaybackTime                  upnp                O
 upnp:lastPlaybackPosition              upnp                O
 upnp:recordedDayOfWeek                 upnp                O
 upnp:srsRecordScheduleID               upnp                O


 Table C-11: movie:videoItem Properties
 Property Name                          NS                  R/O      Remarks
 upnp:storageMedium                     upnp                O
 upnp:DVDRegionCode                     upnp                O
 upnp:channelName                       upnp                O
 upnp:scheduledStartTime                upnp                O
 upnp:scheduledEndTime                  upnp                O
 upnp:scheduledDuration                 upnp                O
 upnp:programTitle                      upnp                O        e.g. "The Pez Disepenser"
 upnp:seriesTitle                       upnp                O        e.g. "Seinfeld - Season 3"
 upnp:episodeCount                     upnp               O          e.g. "23"
 upnp:episodeNr                        upnp               O          e.g. "14"


 Table C-12: videoBroadcast:videoItem Properties
 Property Name                         NS                 R/O       Remarks
 upnp:icon                             upnp               O
 upnp:region                           upnp               O
 upnp:channelNr                        upnp               O
 upnp:signalStrength                   upnp               O
 upnp:signalLocked                     upnp               O
 upnp:tuned                            upnp               O
 upnp:recordable                       upnp               O
 upnp:callSign                         upnp               O
 upnp:price                            upnp               O
 upnp:payPerView                       upnp               O


 Table C-13: musicVideoClip:videoItem Properties
 Property Name                         NS                 R/O       Remarks
 upnp:artist                           upnp               O
 upnp:storageMedium                    upnp               O
 upnp:album                            upnp               O
 upnp:scheduledStartTime               upnp               O
 upnp:scheduledStopTime                upnp               O
 upnp:director                         upnp               O
 dc:contributor                        dc                 O
 dc:date



 res@size                       DIDL-Lite      xsd:unsignedLong      NO         Appendix B.2.1.3
 res@duration                   DIDL-Lite      xsd:string            NO         Appendix B.2.1.4
 res@nrAudioChannels            DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.9
 res@resolution                 DIDL-Lite      xsd:string            NO         Appendix B.2.1.10
 res@colorDepth                  DIDL-Lite      xsd:unsignedInt       NO         Appendix B.2.1.11
 
 */

/*
class VideoItem: public BaseItem
{
    public:
        // item:videoItem
        std::string genre()
        {
            return TrimWhiteSpace(m_metadata.genre);
        }
        // the upnp property is actually actor and multivalued
        // but we return a single string with all actors separated by comma
        std::string actors()
        {
            return m_metadata.actors;
        }
        // producer and director are also multivalue but I really don't think
        // that we are going to get more than one value
        std::string producer()
        {
            return m_metadata.producer;
        }
        std::string director()
        {
            return m_metadata.director;
        }

        std::string language()
        {
            return m_metadata.language;
        }

        // item:videoItem:movie
        std::string seriesTitle()
        {
            return m_metadata.series_title;
        }     // e.g. "Seinfeld - Season 3"
        std::string programTitle()
        {
            return m_metadata.program_title;
        }    // e.g. "The Pez Dispenser"
        unsigned int episodeNr()
        {
            return m_metadata.episode_nr;
        }       // e.g. "14"
        unsigned int episodeCount()
        {
            return m_metadata.episode_count;
        }    // e.g. "23"

        // item:videoItem:videoBroadcast
        // we don't support any of the additional broadcast properties

        // item:videoItem:musicVideoClip
        std::string artist()
        {
            return m_metadata.artist;
        }
        std::string album()
        {
            return m_metadata.album;
        }
        std::string contributor()
        {
            return m_metadata.contributor;
        }

        // res
        fuppes_off_t size()
        {
            return m_metadata.size;
        }
        unsigned int durationMs()
        {
            return m_metadata.duration_ms;
        }
        unsigned int nrAudioChannels()
        {
            return m_metadata.nr_audio_channels;
        }
        unsigned int width()
        {
            return m_metadata.width;
        }
        unsigned int height()
        {
            return m_metadata.height;
        }
        unsigned int colorDepth()
        {
            return m_metadata.color_depth;
        }
        unsigned int videoBitrate()
        {
            return m_metadata.video_bitrate;
        }
        */

        /*
         unsigned int		audio_bitrate;
         unsigned int    audio_bits_per_sample;
         unsigned int    audio_sample_frequency;
         */
/*
        // other
        std::string audioCodec()
        {
            return m_metadata.audio_codec;
        }
        std::string videoCodec()
        {
            return m_metadata.video_codec;
        }
};
*/

class CFileDetails
{
    public:
        CFileDetails(Plugin::Manager& pluginManager);
        void setBaseDevice(CDeviceSettings* baseDevice);
        CDeviceSettings* getBaseDevice()
        {
            return m_baseSettings;
        }

        object_type_t GetObjectType(std::string p_sFileName);
        //std::string GetObjectTypeAsStr(OBJECT_TYPE p_nObjectType);
        //std::string GetContainerTypeAsStr(OBJECT_TYPE p_nContainerType);

        bool getMusicTrackDetails(std::string p_sFileName, Database::AbstractItem& item);
        bool getImageDetails(std::string p_sFileName, Database::AbstractItem& item);
        bool getVideoDetails(std::string p_sFileName, Database::AbstractItem& item);

        bool IsSupportedFileExtension(std::string p_sFileExtension);
        bool isAlbumArtFile(const std::string fileName);
        std::string getAlbumArtFiles();

    private:
        Plugin::Manager& m_pluginManager;
        CDeviceSettings* m_baseSettings;

        std::string			m_albumArtFiles;
};

#endif // _FILEDETAILS_H
