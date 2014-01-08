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

class Pcm
{
	public:
		unsigned char* buffer;
};

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptEncoder;
    strcpy(plugin->name, "pcm");
}

extern "C" int fuppes_encoder_setup(Transcoding::AbstractItem& item, void** data)
{
	*data = new Pcm();
	return 0;
}

extern "C" size_t fuppes_encoder_interleaved(unsigned char* buffer, size_t size, int samples, void** data)
{
	Pcm* plugin = (Pcm*)*data;
	plugin->buffer = buffer;
	return size;
}

extern "C" size_t fuppes_encoder_flush(void** data)
{
    return 0;
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
    Pcm* plugin = (Pcm*)*data;
    return plugin->buffer;
}

extern "C" int fuppes_encoder_teardown(void** data)
{
	if (NULL == *data) {
	    return 1;
	}

    Pcm* plugin = (Pcm*)*data;
	delete plugin;
	return 0;
}

extern "C" void fuppes_unregister_plugin()
{
}



