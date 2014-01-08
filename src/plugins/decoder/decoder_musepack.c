/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            decoder_musepack.c
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

/*
 * some parts are taken from the libmpcdec examples
 * Copyright (c) 2005-2009, The Musepack Development Team
 */

#include "../../include/fuppes_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
		
#include <string.h>
#ifdef MUSEPACK2
#include <mpc/mpcdec.h>
#else
#include <mpcdec/mpcdec.h>
#endif
//#include <mpc/mpcdec.h>

// 1.2.x defines TRUE & FALSE
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
		
typedef struct musepackData_t {

#ifndef MUSEPACK2
	mpc_decoder 		decoder;
#else
	mpc_demux       *decoder;
#endif

	mpc_reader   		reader;
	mpc_streaminfo	streamInfo;	
	
	FILE* file;
  long size;
  mpc_bool_t seekable;
	
	ENDIANESS				outEndianess;
}musepackData_t;



/* callback functions for the mpc_reader */
#ifndef MUSEPACK2
mpc_int32_t read_impl(void *data, void *ptr, mpc_int32_t size)
{
  musepackData_t* d = (musepackData_t*)data;
#else
mpc_int32_t read_impl(mpc_reader *data, void *ptr, mpc_int32_t size)
{
  musepackData_t* d = (musepackData_t*)data->data;	
#endif
  return fread(ptr, 1, size, d->file);
}

#ifndef MUSEPACK2
mpc_bool_t seek_impl(void *data, mpc_int32_t offset)
{
	musepackData_t* d = (musepackData_t*)data;
#else
mpc_bool_t seek_impl(mpc_reader *data, mpc_int32_t offset)
{
	musepackData_t* d = (musepackData_t*)data->data;
#endif
	return d->seekable ? !fseek(d->file, offset, SEEK_SET) : FALSE;
}

#ifndef MUSEPACK2
mpc_int32_t tell_impl(void *data)
{
	musepackData_t* d = (musepackData_t*)data;
#else
mpc_int32_t tell_impl(mpc_reader *data)
{
	musepackData_t* d = (musepackData_t*)data->data;
#endif
  return ftell(d->file);
}

#ifndef MUSEPACK2
mpc_int32_t get_size_impl(void *data)
{
	musepackData_t* d = (musepackData_t*)data;
#else
mpc_int32_t get_size_impl(mpc_reader *data)
{
	musepackData_t* d = (musepackData_t*)data->data;
#endif
	return d->size;
}

#ifndef MUSEPACK2
mpc_bool_t canseek_impl(void *data)
{
	musepackData_t* d = (musepackData_t*)data;
#else
mpc_bool_t canseek_impl(mpc_reader *data)
{
	musepackData_t* d = (musepackData_t*)data->data;
#endif
	return d->seekable;
}


#ifdef MPC_FIXED_POINT
static int shift_signed(MPC_SAMPLE_FORMAT val, int shift)
{
  if (shift > 0)
    val <<= shift;
  else if (shift < 0)  
    val >>= -shift;
  return (int)val;
}
#endif

void convertLE32to16(musepackData_t* data, MPC_SAMPLE_FORMAT* sample_buffer, char *xmms_buffer, unsigned int status, unsigned int* nBytesConsumed)
{
	unsigned int m_bps = 16; //output on 16 bits
  unsigned n;
  int clip_min = -1 << (m_bps - 1),
  clip_max = (1 << (m_bps - 1)) - 1, float_scale = 1 << (m_bps - 1);
  
  for (n = 0; n < 2 * status; n++) {
    int val;
    
    #ifdef MPC_FIXED_POINT
    val = shift_signed(sample_buffer[n],
    m_bps - MPC_FIXED_POINT_SCALE_SHIFT);  
    #else
    val = (int)(sample_buffer[n] * float_scale);  
    #endif
  
    if (val < clip_min)
      val = clip_min;
    else if (val > clip_max)  
      val = clip_max;

    unsigned shift = 0;
    if(data->outEndianess == E_BIG_ENDIAN) {
      shift = 8;
    }
    
    unsigned offset = 0;
    do {
      //xmms_buffer[n * 2 + (shift / 8)] = (unsigned char)((val >> shift) & 0xFF);
      
      if(data->outEndianess == E_LITTLE_ENDIAN) {
        xmms_buffer[n * 2 + offset] = (unsigned char)((val >> shift) & 0xFF);
      }
      else if(data->outEndianess == E_BIG_ENDIAN) {
        if(offset % 2 == 0) 
          xmms_buffer[n * 2 + offset] = (unsigned char)((val >> shift) & 0xFF);
        else
          xmms_buffer[n * 2 + offset] = (unsigned char)(val & 0xFF);
      }
      
      shift += 8;
      offset++;
    } while (shift < m_bps);
  }
  
  *nBytesConsumed = n;
}

		
void register_fuppes_plugin(plugin_info* plugin)
{
	strcpy(plugin->plugin_name, "musepack");
	strcpy(plugin->plugin_author, "Ulrich Voelkel");
	plugin->plugin_type = PT_AUDIO_DECODER;
}

int fuppes_decoder_file_open(plugin_info* plugin, const char* fileName, audio_settings_t* settings)
{	
	plugin->user_data = malloc(sizeof(struct musepackData_t));
	musepackData_t* data = (musepackData_t*)plugin->user_data;

	data->file = fopen(fileName, "rb");
  if(data->file == 0) {
		plugin->cb.log(plugin, 0, __FILE__, __LINE__, "error opening %s.\n", fileName);
		free(plugin->user_data);
    return 1;
  }

  /* initialize our reader_data tag the reader will carry around with it */
  data->seekable = TRUE;
  fseek(data->file, 0, SEEK_END);
  data->size = ftell(data->file);
  fseek(data->file, 0, SEEK_SET);

  /* set up an mpc_reader linked to our function implementations */  
  data->reader.read = read_impl;
  data->reader.seek = seek_impl;
  data->reader.tell = tell_impl;
  data->reader.get_size = get_size_impl;
  data->reader.canseek = canseek_impl;
  data->reader.data = data;

  /* read file's streaminfo data */  
	mpc_streaminfo_init(&data->streamInfo);
#ifndef MUSEPACK2
  if(mpc_streaminfo_read(&data->streamInfo, &data->reader) != ERROR_CODE_OK) {
#else
  if(mpc_streaminfo_read(&data->streamInfo, &data->reader) != MPC_STATUS_OK) {
#endif
		plugin->cb.log(plugin, 0, __FILE__, __LINE__, "Not a valid musepack file: %s.\n", fileName);
		fclose(data->file);
		free(plugin->user_data);
    return 1;
  }

  /* instantiate a decoder with our file reader */
  mpc_decoder_setup(&data->decoder, &data->reader);
  if(!mpc_decoder_initialize(&data->decoder, &data->streamInfo)) {
		plugin->cb.log(plugin, 0, __FILE__, __LINE__, "error initializing decoder: %s.\n", fileName);
		fclose(data->file);
		free(plugin->user_data);
    return 1;
  }  
  
	//settings->channels		= data->vorbisInfo->channels;
	//settings->samplerate	= data->vorbisInfo->rate;	

	return 0;
}

int fuppes_decoder_set_out_endianess(plugin_info* plugin, ENDIANESS endianess)
{
	musepackData_t* data = (musepackData_t*)plugin->user_data;
	data->outEndianess = endianess;
	return 0;
}

int fuppes_decoder_get_out_buffer_size(plugin_info* plugin __attribute__((unused)))
{
	return MPC_DECODER_BUFFER_LENGTH * 4;
}

int fuppes_decoder_total_samples(plugin_info* plugin)
{
	musepackData_t* data = (musepackData_t*)plugin->user_data;
	return mpc_streaminfo_get_length_samples(&data->streamInfo);
}

int fuppes_decoder_decode_interleaved(plugin_info* plugin, char* pcmOut, int bufferSize, int* bytesRead)
{
	musepackData_t* data = (musepackData_t*)plugin->user_data;
	
	MPC_SAMPLE_FORMAT sampleBuffer[MPC_DECODER_BUFFER_LENGTH];
  if(bufferSize < MPC_DECODER_BUFFER_LENGTH) {
		plugin->cb.log(plugin, 0, __FILE__, __LINE__, "buffer size too small for mpc decoding\n");
    return -1;
  }

  unsigned status = mpc_decoder_decode(&data->decoder, sampleBuffer, 0, 0);
  if (status == (unsigned)(-1)) {
    //decode error
		plugin->cb.log(plugin, 0, __FILE__, __LINE__, "decoder error\n");
    return -1;
  }
  else if (status == 0) {  // EOF
    return -1;    
  }
  else {                   // status>0
    // todo: bytes read
    unsigned int nBytesConsumed = 0;
    convertLE32to16(data, sampleBuffer, pcmOut, status, &nBytesConsumed);
    *bytesRead = nBytesConsumed;
    return status;
  }
}

void fuppes_decoder_file_close(plugin_info* plugin)
{
	musepackData_t* data = (musepackData_t*)plugin->user_data;
	
	fclose(data->file);
	free(plugin->user_data);
	plugin->user_data = NULL;
}
		
void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
}

		
#ifdef __cplusplus
}
#endif
