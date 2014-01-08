/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef _FUPPES_UPNP_OBJECT_TYPES_H
#define _FUPPES_UPNP_OBJECT_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum object_type_t
    {
        OBJECT_TYPE_UNKNOWN = 0,
        //ITEM = 1,

        CONTAINER = 1,
        CONTAINER_STORAGEFOLDER = 2,

        CONTAINER_PERSON = 10,
        CONTAINER_PERSON_MUSICARTIST = 11,
        CONTAINER_PERSON_MAX = 12,

        CONTAINER_ALBUM = 20,
        CONTAINER_ALBUM_MUSICALBUM = 21,
        CONTAINER_ALBUM_PHOTOALBUM = 22,
        CONTAINER_ALBUM_MAX = 23,

        CONTAINER_GENRE = 30,
        CONTAINER_GENRE_MUSICGENRE = 31,
        CONTAINER_GENRE_MOVIEGENRE = 32,
        CONTAINER_GENRE_MAX = 33,

        /*CONTAINER_CHANNEL_GROUP = 50,
         CONTAINER_CHANNEL_GROUP_AUDIO_CHANNEL_GROUP = 51,
         CONTAINER_CHANNEL_GROUP_VIDEO_CHANNEL_GROUP = 52,*/

        //CONTAINER_EPG_CONTAINER = 60,
        /*CONTAINER_STORAGE_SYSTEM = 70,
         CONTAINER_STORAGE_VOLUME = 80,
         CONTAINER_BOOKMARK_FOLDER = 90*/

        CONTAINER_MAX = 80,

        /* "object.container.playlistContainer" and "object.item.playlistItem”
         have the same OBJECT_TYPE in the database. the type of representation
         is selected from the device configuration when the object is requested */
        CONTAINER_PLAYLISTCONTAINER = 90,

        ITEM = 100,

        ITEM_IMAGE_ITEM = 110,
        ITEM_IMAGE_ITEM_PHOTO = 111,
        ITEM_IMAGE_ITEM_MAX = 112,

        ITEM_AUDIO_ITEM = 120,
        ITEM_AUDIO_ITEM_MUSIC_TRACK = 121,
        ITEM_AUDIO_ITEM_AUDIO_BROADCAST = 122,
        //ITEM_AUDIO_ITEM_AUDIO_BOOK      = 123,
        ITEM_AUDIO_ITEM_MAX = 123,

        ITEM_VIDEO_ITEM = 130,
        ITEM_VIDEO_ITEM_MOVIE = 131,
        ITEM_VIDEO_ITEM_VIDEO_BROADCAST = 132,
        ITEM_VIDEO_ITEM_MUSIC_VIDEO_CLIP = 133,
        ITEM_VIDEO_ITEM_MAX = 134,

        ITEM_TEXT_ITEM = 140,
        /*ITEM_BOOKMARK_ITEM = 150,

         ITEM_EPG_ITEM = 160,
         ITEM_EPG_ITEM_AUDIO_PROGRAM = 161,
         ITEM_EPG_ITEM_VIDEO_PROGRAM = 162,*/

        ITEM_MAX = 200,

        /* we use the values after ITEM_MAX for some special purpose values */

        /*
         when parsing a playlist and we find a stream url
         we set the type to "unknown broadcast" and try to
         detect the type (audio or video) of the stream later.
         this is no valid upnp object type!
         */
        ITEM_UNKNOWN_BROADCAST = 901

    } object_type_t;

    struct upnp_object_type_t
    {
        const char* str;
        const object_type_t type;
        const object_type_t max;
    };

    const struct upnp_object_type_t upnp_object_types[] = {

        { "object.container", CONTAINER, CONTAINER_MAX },
        { "object.container.storageFolder", CONTAINER_STORAGEFOLDER, CONTAINER_MAX },

        { "object.container.person", CONTAINER_PERSON, CONTAINER_PERSON_MAX },
        { "object.container.person.musicArtist", CONTAINER_PERSON_MUSICARTIST, CONTAINER_PERSON_MUSICARTIST },

        { "object.container.album", CONTAINER_ALBUM, CONTAINER_ALBUM_MAX },
        { "object.container.album.musicAlbum", CONTAINER_ALBUM_MUSICALBUM, CONTAINER_ALBUM_MUSICALBUM },
        { "object.container.album.photoAlbum", CONTAINER_ALBUM_PHOTOALBUM, CONTAINER_ALBUM_PHOTOALBUM },

        { "object.container.genre", CONTAINER_GENRE, CONTAINER_GENRE_MAX },
        { "object.container.genre.musicGenre", CONTAINER_GENRE_MUSICGENRE, CONTAINER_GENRE_MUSICGENRE },
        { "object.container.genre.movieGenre", CONTAINER_GENRE_MOVIEGENRE, CONTAINER_GENRE_MOVIEGENRE },


        { "object.item", ITEM, ITEM_MAX },

        { "object.item.imageItem", ITEM_IMAGE_ITEM, ITEM_IMAGE_ITEM_MAX },
        { "object.item.imageItem.photo", ITEM_IMAGE_ITEM_PHOTO, ITEM_IMAGE_ITEM_PHOTO },

        { "object.item.audioItem", ITEM_AUDIO_ITEM, ITEM_AUDIO_ITEM_MAX },
        { "object.item.audioItem.musicTrack", ITEM_AUDIO_ITEM_MUSIC_TRACK, ITEM_AUDIO_ITEM_MUSIC_TRACK },
        { "object.item.audioItem.audioBroadcast", ITEM_AUDIO_ITEM_AUDIO_BROADCAST, ITEM_AUDIO_ITEM_AUDIO_BROADCAST },

        { "object.item.videoItem", ITEM_VIDEO_ITEM, ITEM_VIDEO_ITEM_MAX },
        { "object.item.videoItem.movie", ITEM_VIDEO_ITEM_MOVIE, ITEM_VIDEO_ITEM_MOVIE },

        { 0, OBJECT_TYPE_UNKNOWN, OBJECT_TYPE_UNKNOWN }
    };

#ifdef __cplusplus
}
#endif

#endif
