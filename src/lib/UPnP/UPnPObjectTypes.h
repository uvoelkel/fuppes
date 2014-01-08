
#ifndef _UPNP_UPNPOBJECTTYPES_H
#define _UPNP_UPNPOBJECTTYPES_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_upnp_object_types.h>

#include <string>
#include "../Common/Exception.h"

typedef object_type_t OBJECT_TYPE;

namespace UPnP
{
    typedef OBJECT_TYPE objectType_t;


inline std::string typeToString(OBJECT_TYPE type) 
{
  switch(type) {

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
          
          
    default:
      throw fuppes::Exception(__FILE__, __LINE__, "unknown object type %d", type);
  }

  return "unknown";
}

inline OBJECT_TYPE stringToType(std::string type) 
{
  if(type.compare("object.container") == 0)
    return CONTAINER;
  else if(type.compare("object.container.storageFolder") == 0)
    return CONTAINER_STORAGEFOLDER;
  
  return OBJECT_TYPE_UNKNOWN;
}

}

#endif // _UPNP_UPNPOBJECTTYPES_H
