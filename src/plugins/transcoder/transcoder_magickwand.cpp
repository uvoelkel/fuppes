/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            transcoder_magickwand.c
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
#include <wand/magick-wand.h>

#ifdef __cplusplus
extern "C" {
#endif
	
void register_fuppes_plugin(plugin_info* info)
{
	strcpy(info->plugin_name, "magickWand");
	strcpy(info->plugin_author, "Ulrich Voelkel");
	info->plugin_type = PT_TRANSCODER;
	
	//#ifdef WIN32
	MagickWandGenesis();
	//#endif
}
 
/*
<width>x<height>{+-}<xoffset>{+-}<yoffset>{%}{!}{<}{>}

%		Interpret width and height as a percentage of the current size.
!		Resize to width and height exactly, loosing original aspect ratio.
<		Resize only if the image is smaller than the geometry specification.
>		Resize only if the image is greater than the geometry specification.
*/ 

//int scaleImage(



int fuppes_transcoder_transcode_image_file(plugin_info* plugin,
	                                const char* inputFile,
		                              const char* outputFile,
																	int width, int height,
																	int less,	int greater)
{
/*
 	MagickBooleanType MagickReadImage(MagickWand *wand,const char *filename)
 
	
*/

	/*MagickWand* wand = NewMagickWand();
	
	MagickRelinquishMemory(wand);*/

	
	return -1;
}

int fuppes_transcoder_transcode_image_mem(plugin_info* plugin,
	                                const unsigned char** inBuffer,
																	size_t inSize,
		                              unsigned char** outBuffer,
																	size_t* outSize,
																	int width, int height,
																	int less,	int greater)
{
	MagickBooleanType status;
	MagickWand* wand = NewMagickWand();
	
	//printf("magickWand %d\n", 1);
	
	status = MagickReadImageBlob(wand, *inBuffer, inSize);
	if(status == MagickFalse) {
		plugin->cb.log(plugin, 0, __FILE__, __LINE__,
								"error reading image from buffer. size: %d", inSize);
		wand = DestroyMagickWand(wand);
		return -1;
	}

	//printf("magickWand %d\n", 2);
	
	MagickWand* wandOut;
	char geometry[123];
	sprintf(&geometry[0], "%dx%d", width, height);
	//char* geometry = "250x250";
	wandOut = MagickTransformImage(wand, "", geometry);
	wand = DestroyMagickWand(wand);
	if(wandOut == NULL) {		
		return -1;
	}

	//printf("magickWand %d\n", 3);
	
	//MagickBooleanType MagickSetFormat(MagickWand *wand,const char *format)
	
	size_t length;
	unsigned char* blob;
	blob = MagickGetImageBlob(wandOut, &length);
	*outBuffer = (unsigned char*)realloc(*outBuffer, length);
	memcpy(*outBuffer, blob, length);
	*outSize = length;
	MagickRelinquishMemory(blob);
	
	//printf("magickWand %d, %d\n", 4, *outSize);
	
	//MagickRelinquishMemory(wandOut);
	wandOut = DestroyMagickWand(wandOut);
	return 0;	
}

void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
{
	MagickWandTerminus();
}

#ifdef __cplusplus
}
#endif
