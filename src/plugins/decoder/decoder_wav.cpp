/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fuppes_plugin.h>
#include <fuppes_decoder.h>
#include <fuppes_database_objects.h>

#ifdef WIN32
#include <sys/file.h>
#endif

#include <iostream>

class WavData
{
    public:
        FILE* fd;
        Endianness endianness;
};

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptDecoder;
    strcpy(plugin->name, "wav");
}

extern "C" int fuppes_open_file(const char* filename, void** data)
{
    *data = new WavData();
    WavData* plugin = (WavData*)*data;

    plugin->fd = fopen(filename, "rb");
    if (NULL == plugin->fd) {
        delete plugin;
        return 1;
    }

#ifdef WIN32
    _setmode(_fileno(plugin->fd), _O_BINARY);
#endif

    unsigned char header[44];
    size_t size = fread(header, 1, 44, plugin->fd);
    if (44 != size) {
        goto error;
    }

    if ('R' == header[0] && 'I' == header[1] && 'F' == header[2] && 'F' == header[3]) {

        plugin->endianness = eLittleEndian;




    }
    else if ('R' == header[0] && 'I' == header[1] && 'F' == header[2] && 'X' == header[3]) {

        plugin->endianness = eBigEndian;
        std::cerr << "RIFX WAV (big endian) is not supported" << std::endl;
        goto error;
    }
    else {
        goto error;
    }

    return 0;

    error:
        fclose(plugin->fd);
        delete plugin;
        return 1;
}

extern "C" int fuppes_read_metadata(Database::AbstractItem& item, void** data)
{
    return 1;
}

extern "C" int fuppes_set_out_endianess(ENDIANESS endianess, void** data)
{
    return 1;
}

extern "C" fuppes_int64_t fuppes_get_total_samples(void** data)
{
    return 0;
}

extern "C" int fuppes_decode_interleaved(unsigned char* buffer, size_t size, size_t* bytesConsumed, void** data)
{
    WavData* plugin = (WavData*)*data;

    int bits = 16;
    int channels = 2;

    unsigned int samples = size / (bits/8) / channels;
    size_t samplesSize = samples * (bits/8) * channels;

    //std::cout << "SIZE: " << size << " SAMPLES: " << samples << " SIZE: " << samplesSize << std::endl;

	*bytesConsumed = fread(buffer, 1, samplesSize, plugin->fd);
	if (*bytesConsumed < samplesSize && 0 == feof(plugin->fd)) {
		return -1;
	}

	unsigned int samplesRead = *bytesConsumed / (bits/8) / channels;
	return samplesRead;
}

extern "C" void fuppes_close_file(void** data)
{
    WavData* plugin = (WavData*)*data;
    fclose(plugin->fd);
    delete plugin;
    *data = NULL;
}

extern "C" void fuppes_unregister_plugin()
{
}
