/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/***************************************************************************
 *            fuppes_plugin_types.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2008-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FUPPES_PLUGIN_TYPES_H
#define _FUPPES_PLUGIN_TYPES_H

#include "fuppes_types.h"

typedef enum PluginType_t {
    ptUnknown,
    ptDatabase,
    ptMetadata,
    ptItemSource,
    ptDecoder,
    ptEncoder,
    ptTranscoder

} PluginType_t;

typedef enum PLUGIN_TYPE {
	PT_NONE,
	PT_METADATA,
	PT_AUDIO_DECODER,
	PT_AUDIO_ENCODER,
	PT_TRANSCODER,
	PT_THREADED_TRANSCODER,
	//PT_DATABASE_CONNECTION,
  PT_ITEM_SOURCE
} PLUGIN_TYPE;

typedef struct {
	char		ext[10];
	void*		next;
} file_ext;


// stores a list for key/value pairs
// it's used to represent a single dataset
// e.g. the description of a shared directory
// idx, type, path
typedef struct {
	char*		key;
	char*		value;

	void*		next;
} arg_list_t;

// stores a list of arg_list_t instances
// if the args store the description of a single shared dir
// this one holds the list of all shared dirs
typedef struct {
	arg_list_t* arg;

	void* next;
} result_set_t;




//typedef int (*ctrl_action_t)(const char* action, arg_list_t* args, arg_list_t* result);


typedef void (*log_t)(void* plugin, int level, const char* file, int line, const char* format, ...);

typedef struct {
	log_t					log;
	//ctrl_action_t	ctrl;
} plugin_callbacks;

typedef enum {

  sf_none       = 0,
  sf_numbered   = 1 << 0,     // items are numbered
  sf_reverse    = 1 << 1,     // items are imported in reverse order

  // the blip.tv plugin uses "sf_numbered | sf_reverse"
  // items are imported in reverse order and numbered
  // the oldest item is 1 the oldest but one is 2 and so on

} source_flag_t;
typedef int source_flags_t;


typedef struct {
    PluginType_t                    type;
    char                            name[200];
    char                            author[1000];


    char                            plugin_name[200];
    char                            plugin_author[1000];
    char                            plugin_description[2000];
	PLUGIN_TYPE                     plugin_type;
	char                            plugin_version[100];

	char                            library_version[100];

	void*							user_data;
	plugin_callbacks                cb;

    int                             plugin_capabilities;

    // source plugin fields
    char                            source_base_name[1000];
    source_flags_t                  source_flags;
} plugin_info;



// METADATA

typedef enum METADATA_TYPE {
	MD_NONE,
	MD_AUDIO,
	MD_VIDEO,
	MD_IMAGE
} METADATA_TYPE;

typedef enum METADATA_PLUGIN_CAPABILITIES {
	MDC_UNKNOWN		= 0,
	MDC_METADATA	= 1 << 0,   // can extract metadata
	MDC_IMAGE			= 1 << 1    // can extract embedded images
} METADATA_PLUGIN_CAPABILITIES;


struct metadata_t {
	METADATA_TYPE		type;

	// baseItem
  char						path[1000];
  char						file_name[1000];
  char						mime_type[100];

	char						title[1000];
	char						publisher[1000];
	char						date[1000];
	char						description[1000];
	char						long_description[1000];

	// audioItem
	char						genre[1000];
  // description
  // long_description
	char						language[1000];

	// audioItem::musicTrack
	char						artist[1000];
	char						album[1000];
	unsigned int		track_number;
	char						contributor[1000];

  // item:audioItem:audioBook
  char						producer[1000];



  // item:imageItem
  // description
  // long_description

  // item:imageItem:photo
  // album



  // item:videoItem
  // genre
  // description
  // long_description
  char            actors[1000];
  // producer
  char            director[1000];
  // language

  // item:videoItem:movie
  char            series_title[1000];                 // e.g. "Seinfeld - Season 3"
  char            program_title[1000];                // e.g. "The Pez Dispenser"
  unsigned int    episode_nr;                   // e.g. "14"
  unsigned int    episode_count;                // e.g. "23"

  // item:videoItem:musicVideoClip
  // artist
  // album
  // contributor



  // res
	fuppes_off_t		size;
	unsigned int		duration_ms;
	unsigned int		nr_audio_channels;
	unsigned int		audio_bitrate;
  unsigned int    audio_bits_per_sample;
  unsigned int    audio_sample_frequency;
	unsigned int		video_bitrate;
  unsigned int    width;
  unsigned int    height;
  unsigned int    color_depth;


	// other
	unsigned int		year;
  char            composer[1000];
	char						audio_codec[100];
	char						video_codec[100];
	int							has_image;
	char						image_mime_type[100];
  int             image_width;
  int             image_height;
};


// AUDIO

typedef enum ENDIANESS {
  E_LITTLE_ENDIAN = 0,
  E_BIG_ENDIAN = 1
} ENDIANESS;


typedef struct {
    int				channels;
    int				samplerate;
    int    			bitrate;
    int				num_samples;
} audio_settings_t;



#endif // _FUPPES_PLUGIN_TYPES_H

#ifdef __cplusplus
}
#endif
