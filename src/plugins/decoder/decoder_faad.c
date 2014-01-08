/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            decoder_faad.c
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

#include "../../include/fuppes_plugin.h"


#ifdef __cplusplus
extern "C" {
#endif
		
#include <string.h>
#ifndef TREMOR
#include <vorbis/vorbisfile.h>
#else
#include <tremor/ivorbisfile.h>
#endif
		
typedef struct vorbisData_t {
	FILE*						fd;
	OggVorbis_File	vorbisFile;
	ENDIANESS				outEndianess;
	vorbis_info* 		vorbisInfo;
} vorbisData_t;
		
void register_fuppes_plugin(plugin_info* plugin)
{
	strcpy(plugin->plugin_name, "faad");
	strcpy(info->plugin_author, "Ulrich Voelkel");
	plugin->plugin_type = PT_AUDIO_DECODER;
}

int fuppes_decoder_file_open(plugin_info* plugin, const char* fileName, audio_settings_t* settings)
{	
	plugin->user_data = malloc(sizeof(struct vorbisData_t));
	vorbisData_t* data = (vorbisData_t*)plugin->user_data;
		
	data->fd = fopen(fileName, "rb");
	if(data->fd == NULL) {
		free(plugin->user_data);
		return 1;
	}
	
	#ifdef _WIN32
  _setmode(_fileno(data->fd), _O_BINARY);
  #endif
		
	if(ov_open(data->fd, &data->vorbisFile, NULL, 0) < 0) {
		plugin->log(0, __FILE__, __LINE__, "Input does not appear to be an Ogg bitstream.\n");
		free(plugin->user_data);
    return 1;
	}
	
  data->vorbisInfo			= ov_info(&data->vorbisFile, -1);
	settings->channels		= data->vorbisInfo->channels;
	settings->samplerate	= data->vorbisInfo->rate;	
	//pAudioDetails->nNumPcmSamples = m_OvPcmTotal(&m_VorbisFile, -1)
	
	return 0;
}

int fuppes_decoder_set_out_endianess(plugin_info* plugin, ENDIANESS endianess)
{
	vorbisData_t* data = (vorbisData_t*)plugin->user_data;
	data->outEndianess = endianess;
	return 0;
}

int fuppes_decoder_get_out_buffer_size(plugin_info* plugin) // __attribute__((unused)))
{
	return FAAD_MIN_STREAMSIZE * FAAD_MAX_CHANNELS;
}

int fuppes_decoder_total_samples(plugin_info* plugin)
{
	vorbisData_t* data = (vorbisData_t*)plugin->user_data;
	return ov_pcm_total(&data->vorbisFile, -1);  
}

int fuppes_decoder_decode_interleaved(plugin_info* plugin, char* pcmOut, int bufferSize, int* bytesRead)
{
	vorbisData_t* data = (vorbisData_t*)plugin->user_data;
	
	int bitstream = 0; 
  int bytesConsumed = ov_read(&data->vorbisFile, pcmOut, bufferSize, data->outEndianess, 2, 1, &bitstream);
	
  if(bytesConsumed == 0) { // EOF
    return -1;
  }
  else if(bytesConsumed < 0) { // error in the stream

    if(bytesConsumed == OV_HOLE) {
      plugin->log(2, __FILE__, __LINE__, "OV_HOLE");
		}
    else if(bytesConsumed == OV_EBADLINK) {
      //CSharedLog::Shared()->Log(L_EXT,"OV_EBADLINK", __FILE__, __LINE__);
			fprintf(stderr, "OV_EBADLINK\n");
		}
    else {
      //CSharedLog::Shared()->Log(L_EXT,"unknown stream error", __FILE__, __LINE__);      
			fprintf(stderr, "unknown stream error\n");
    }    
    return -1;
  }
  else {
    if(bitstream != 0) {
      return -1;
		}
    
    *bytesRead = bytesConsumed;
    
    // calc samples and return
    int samplesRead = bytesConsumed / data->vorbisInfo->channels / sizeof(short int);
    return samplesRead;
  }
}

void fuppes_decoder_file_close(plugin_info* plugin)
{
	vorbisData_t* data = (vorbisData_t*)plugin->user_data;
	
	ov_clear(&data->vorbisFile);
	free(plugin->user_data);
	plugin->user_data = NULL;
}
		
void unregister_fuppes_plugin(plugin_info* info)
{
}

		
#ifdef __cplusplus
}
#endif
