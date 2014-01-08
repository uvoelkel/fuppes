/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_DATABASE_OBJECTS_H
#define _FUPPES_DATABASE_OBJECTS_H

#include <fuppes_types.h>
#include <fuppes_upnp_object_types.h>
#include <string>

namespace Database
{
    class AbstractItemSource;
    class AbstractItemDetails;

    class AbstractItem
    {
        public:
            virtual ~AbstractItem()
            {
            }

            virtual void clear() = 0;

            /**
             * unique id
             */
            fuppes_int64_t id;

            /**
             * unique object id
             */
            object_id_t objectId;

            /**
             * the item's parent id
             */
            object_id_t parentId;

            /**
             * the item's detail id
             */
            object_id_t detailId;

            /**
             * the type of the item
             */
            object_type_t type;

            /**
             * the source of this object (something like file, youtube, ...)
             */
            std::string source;



            /**
             * the absolute path of the item
             * in case of a file the path does NOT contain the filename
             *
             * this field is only used for local files
             */
            std::string path;

            /**
             * the item's filename including the file extension
             * empty for containers/directories
             *
             * this field is only used for local files
             */
            std::string filename;

            /**
             * the file extension (without a dot)
             * empty for containers/directories
             *
             * this field is only used for local files
             */
            std::string extension;




            /**
             * the title
             * usually the file/directory name or some value extracted from the item (e.g. id3 tag)
             */
            std::string title;

            /**
             * the ref id
             * each individual object has a refId of 0
             * if an object is inserted multiple types (because they are in the search path and also included via playlists)
             * the first entry has a refId of 0. and all following a ref id with the objectId of the first.
             */
            object_id_t refId;

            /**
             * the name of the virtual folder layout this item is part of
             * empty for all items that are not part of any virtual folder stuff
             */
            std::string device;

            /**
             * the type of the virtual container (e.g. genre, album, artist)
             */
            int vcontainerType;

            /**
             * the full virtual path this object is part of ( e.g. folder | genre | artist)
             */
            std::string vcontainerPath;

            /**
             * the objectId of the original, non-virtual item (device == empty and refId == 0)
             * 0 for all others
             */
            object_id_t vrefId;

            /**
             * visible
             */
            bool visible;

            /**
             * the filesize in bytes
             *
             * this field is only used for local files
             */
            fuppes_off_t size;

            /**
             * last file modification time
             * this is either the time of insertion for new items
             * or the time of the last file modification (e.g. metadata change detected by fam)
             */
            time_t modified;

            /**
             * the time the file was updated
             * if updated < modified the file will be checked and the details created/updated
             */
            time_t updated;

            /**
             * the details or NULL
             */
            AbstractItemDetails* details;


            //std::list<AbstractItemSource>   sources;
    };

    class AbstractItemSource
    {
        public:
            virtual ~AbstractItemSource()
            {
            }

            virtual void clear() = 0;

            /**
             * unique id
             */
            fuppes_int64_t id;

            /**
             * object id
             */
            object_id_t objectId;

            /**
             * the absolute path of the item
             * in case of a local file the path is empty
             */
            std::string path;

            /**
             * the original (non transcoded) mime/type of the source
             */
            std::string mimeType;

            /**
             * visible
             */
            bool visible;

            /**
             * the filesize in bytes
             */
            fuppes_off_t size;



            // image / video properties
            int iv_width;
            int iv_height;
            int iv_depth;

            // audio properties
            int a_channels;
            int a_bitrate;
            int a_bitsPerSample;
            int a_samplerate;

            // video properties
            int v_bitrate;

            // codecs
            std::string audioCodec;
            std::string videoCodec;
    };

    class AbstractItemDetails
    {
        public:
            virtual ~AbstractItemDetails()
            {
            }

            virtual void clear() = 0;

            /**
             * unique id
             */
            fuppes_int64_t id;

            std::string publisher;
            std::string date;
            std::string description;
            std::string longDescription;

            // audio / video properties
            std::string av_genre;
            std::string av_language;
            std::string av_artist;
            std::string av_album;
            std::string av_contributor;
            std::string av_producer;
            int av_duration;

            // image / video properties
            int iv_width;
            int iv_height;
            int iv_depth;

            // audio properties
            int a_trackNumber;
            int a_channels;
            int a_bitrate;
            int a_bitsPerSample;
            int a_samplerate;
//            fuppes_int64_t a_totalSamples;
            int a_year;
            std::string a_composer;

            // video properties
            std::string v_actors;
            std::string v_director;
            std::string v_seriesTitle;
            std::string v_programTitle;
            int v_episodeNumber;
            int v_episodeCount;
            bool v_hasSubtitles;
            int v_bitrate;

            // codecs
            std::string audioCodec;
            std::string videoCodec;

            // album art
            object_id_t albumArtId;
            std::string albumArtExtension;
            std::string albumArtMimeType;
            int albumArtWidth;
            int albumArtHeight;

            // other sources
            int source;
            std::string streamMimeType;

    };
}

#endif // _FUPPES_DATABASE_OBJECTS_H
