/***************************************************************************
 *            metadata_libmp4v2.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2008 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#include "../../include/fuppes_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <mp4v2/mp4v2.h>
	
void register_fuppes_plugin(plugin_info* info)
{
	strcpy(info->plugin_name, "mp4v2");
	strcpy(info->plugin_author, "Ulrich Voelkel");
	info->plugin_type = PT_METADATA;
}

int fuppes_metadata_file_open(plugin_info* plugin, const char* fileName)
{
	MP4FileHandle mp4file = MP4Read(fileName);
	if(!mp4file) {
		return -1;
	}	
	MP4Close(mp4file);

	plugin->user_data = malloc((strlen(fileName)+1) * sizeof(char));
	strcpy((char*)plugin->user_data, fileName);
	
	return 0;
}

int fuppes_metadata_read(plugin_info* plugin, metadata_t* metadata)
{
	MP4FileHandle mp4file = MP4Read((const char*)plugin->user_data);
	if(!mp4file) {
		return -1;
	}



  const MP4Tags* tags = MP4TagsAlloc();
  MP4TagsFetch(tags, mp4file);
  
	//char* value;
  
	// title
	/*MP4GetMetadataName(mp4file, &value);  
	if(value) {
		set_value(metadata->title, sizeof(metadata->title), value);
		free(value);
	}	*/
  if(tags->name)
    set_value(metadata->title, sizeof(metadata->title), tags->name);
  
	// duration
	u_int32_t scale = MP4GetTimeScale(mp4file);//scale is ticks in secs, used same value in duration.
	MP4Duration length = MP4GetDuration(mp4file);
  length = length / scale;
  metadata->duration_ms = length * 1000;
  
	// channels
	metadata->nr_audio_channels = MP4GetTrackAudioChannels(mp4file, 
																		MP4FindTrackId(mp4file, 0, MP4_AUDIO_TRACK_TYPE));
	// bitrate
	metadata->audio_bitrate = MP4GetTrackBitRate(mp4file, 
														MP4FindTrackId(mp4file, 0, MP4_AUDIO_TRACK_TYPE));
	metadata->audio_bits_per_sample = 0;

	// artist
	/*MP4GetMetadataArtist(mp4file, &value);
	if(value) {
		set_value(metadata->artist, sizeof(metadata->artist), value);
		free(value);
	}*/
  if(tags->artist)
    set_value(metadata->artist, sizeof(metadata->artist), tags->artist);
	
	// genre
	/*MP4GetMetadataGenre(mp4file, &value);
	if(value) {
		set_value(metadata->genre, sizeof(metadata->genre), value);
		free(value);
	}*/
  if(tags->genre)
	  set_value(metadata->genre, sizeof(metadata->genre), tags->genre);
  
	// Album
	/*MP4GetMetadataAlbum(mp4file, &value);
	if(value) {
		set_value(metadata->album, sizeof(metadata->album), value);
		free(value);
	}*/
  if(tags->album)
    set_value(metadata->album, sizeof(metadata->album), tags->album);
	
	// description/comment
	/*MP4GetMetadataComment(mp4file, &value);
	if(value) {
		set_value(metadata->description, sizeof(metadata->description), value);
		free(value);
	}*/
  if(tags->comments)
    set_value(metadata->description, sizeof(metadata->description), tags->comments);

	// track no.
	/*u_int16_t track, totaltracks;
	MP4GetMetadataTrack(mp4file, &track, &totaltracks);
	metadata->track_number = track;*/
  if(tags->track)
    metadata->track_number = tags->track->index;
  
	// date/year
	/*MP4GetMetadataYear(mp4file, &value);
	if(value)	{
		pMusicTrack->nYear = atoi(value);
		free(value);
	}*/


  MP4TagsFree(tags);
  
	MP4Close(mp4file);
	
	return 0;
}

void fuppes_metadata_file_close(plugin_info* plugin)
{
	free(plugin->user_data);
	plugin->user_data = NULL;
}

void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
}

#ifdef __cplusplus
}
#endif
