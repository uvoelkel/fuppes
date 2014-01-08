/*
 * This file is part of fuppes
 *
 * (c) 2005 - 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_plugin.h>
#include <fuppes_encoder.h>

#include <lame/lame.h>

#include <iostream>

class Lame
{
	public:
		lame_global_flags* flags;
		unsigned char buffer[LAME_MAXMP3BUFFER];

	    int bitrate;
	    int samplerate;
	    int channels;
};


extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptEncoder;
    strcpy(plugin->name, "lame");
}

extern "C" int fuppes_encoder_setup(Transcoding::AbstractItem& item, void** data)
{
    *data = new Lame();
    Lame* plugin = (Lame*)*data;

	plugin->flags = lame_init();
	if (NULL == plugin->flags) {
		delete plugin;
		plugin = NULL;
		return 1;
	}



	plugin->bitrate = 128;
	plugin->samplerate = 44100;
	plugin->channels = 2;

	lame_set_VBR(plugin->flags, vbr_off);

	// lame_set_compression_ratio

	// lame_set_brate

	// lame_set_quality // 0=best (very slow).  9=worst


	lame_set_mode(plugin->flags, STEREO);

	if (-1 == lame_init_params(plugin->flags)) {
		lame_close(plugin->flags);
		delete plugin;
		plugin = NULL;
		return 1;
	}

	return 0;
}

extern "C" size_t fuppes_encoder_interleaved(unsigned char* buffer, size_t size, int samples, void** data)
{
    Lame* plugin = (Lame*)*data;
    return lame_encode_buffer_interleaved(plugin->flags, (short int*)buffer, samples, plugin->buffer, LAME_MAXMP3BUFFER);
}

extern "C" size_t fuppes_encoder_flush(void** data)
{
    Lame* plugin = (Lame*)*data;
    return lame_encode_flush(plugin->flags, plugin->buffer, LAME_MAXMP3BUFFER);
}

extern "C" Endianness fuppes_get_endianness(void** data)
{
	#ifdef WORDS_BIGENDIAN
	return eBigEndian;
	#else
	return eLittleEndian;
	#endif
}

extern "C" unsigned char* fuppes_get_buffer(void** data)
{
    Lame* plugin = (Lame*)*data;
    return plugin->buffer;
}

extern "C" int fuppes_encoder_teardown(void** data)
{
	if (NULL == *data) {
	    return 1;
	}

    Lame* plugin = (Lame*)*data;
    lame_close(plugin->flags);
	delete plugin;
	plugin = NULL;
	return 0;
}

extern "C" void fuppes_unregister_plugin()
{
}
