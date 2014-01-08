/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * This file is part of fuppes
 *
 * (c) 2012 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "../../include/fuppes_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
		
void register_fuppes_plugin(plugin_info* plugin)
{
	strcpy(plugin->plugin_name, "opus");
	strcpy(plugin->plugin_author, "Ulrich Voelkel");
	plugin->plugin_type = PT_AUDIO_DECODER;
}

int fuppes_decoder_file_open(plugin_info* plugin, const char* fileName, audio_settings_t* settings)
{	
	return 1;
}

/*
int fuppes_decoder_set_out_endianess(plugin_info* plugin, ENDIANESS endianess)
{
	musepackData_t* data = (musepackData_t*)plugin->user_data;
	data->outEndianess = endianess;
	return 0;
}
*/

int fuppes_decoder_get_out_buffer_size(plugin_info* plugin __attribute__((unused)))
{
	return 0; //return MPC_DECODER_BUFFER_LENGTH * 4;
}

/*
int fuppes_decoder_total_samples(plugin_info* plugin)
{
	musepackData_t* data = (musepackData_t*)plugin->user_data;
	return mpc_streaminfo_get_length_samples(&data->streamInfo);
}
*/

int fuppes_decoder_decode_interleaved(plugin_info* plugin, char* pcmOut, int bufferSize, int* bytesRead)
{
  return -1;
}


void fuppes_decoder_file_close(plugin_info* plugin)
{

}

		
void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
}

		
#ifdef __cplusplus
}
#endif
