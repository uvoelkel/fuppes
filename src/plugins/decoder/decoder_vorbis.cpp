/*
 * This file is part of fuppes
 *
 * (c) 2005-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
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

#include <string.h>
#ifndef TREMOR
#include <vorbis/vorbisfile.h>
#else
#include <tremor/ivorbisfile.h>
#endif

class VorbisData
{
    public:
        FILE* fd;
        OggVorbis_File vorbisFile;
        ENDIANESS outEndianess;
        Endianness endianness;
        vorbis_info* vorbisInfo;
};

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptDecoder;
    strcpy(plugin->name, "vorbis");
}

extern "C" int fuppes_open_file(const char* filename, void** data)
{
    *data = new VorbisData();
    VorbisData* plugin = (VorbisData*)*data;

    plugin->outEndianess = E_LITTLE_ENDIAN;

    int ret = ov_fopen(filename, &plugin->vorbisFile);
    if (0 > ret) {
		delete plugin;
		return 1;
	}

    /*
    plugin->fd = fopen(filename, "rb");
    if (NULL == plugin->fd) {
        delete plugin;
        return 1;
    }

#ifdef _WIN32
    _setmode(_fileno(plugin->fd), _O_BINARY);
#endif

    if (0 > ov_open(plugin->fd, &plugin->vorbisFile, NULL, 0)) {
        delete plugin;
        return 1;
    }
    */

    plugin->vorbisInfo = ov_info(&plugin->vorbisFile, -1);
    return 0;
}

extern "C" int fuppes_read_metadata(Database::AbstractItem& item, void** data)
{
    VorbisData* plugin = (VorbisData*)*data;

    item.details->a_channels = plugin->vorbisInfo->channels;
    //item.details->a_bitrate = plugin->vorbisInfo->bitrate_nominal;
    item.details->a_samplerate = plugin->vorbisInfo->rate;
    return 0;
}

extern "C" size_t fuppes_get_buffersize(void** data)
{
    return 0;
}

extern "C" int fuppes_set_endianness(Endianness endianness, void** data)
{
    VorbisData* plugin = (VorbisData*)*data;
    plugin->endianness = endianness;
    return 0;
}

extern "C" int fuppes_set_out_endianess(ENDIANESS endianess, void** data)
{
    VorbisData* plugin = (VorbisData*)*data;
    plugin->outEndianess = endianess;
    return 0;
}

extern "C" fuppes_int64_t fuppes_get_total_samples(void** data)
{
    VorbisData* plugin = (VorbisData*)*data;
    return ov_pcm_total(&plugin->vorbisFile, -1);
}


extern "C" int fuppes_decode_interleaved(unsigned char* buffer, size_t size, size_t* bytesConsumed, void** data)
{
    VorbisData* plugin = (VorbisData*)*data;

    int bitstream = 0;
    long consumed = ov_read(&plugin->vorbisFile, (char*)buffer, size, plugin->outEndianess, 2, 1, &bitstream);

    if (consumed == 0) { // EOF
        return 0;
    }
    else if (consumed < 0) { // error in the stream

        if (consumed == OV_HOLE) {
            //plugin->cb.log(plugin, 2, __FILE__, __LINE__, "OV_HOLE");
        }
        else if (consumed == OV_EBADLINK) {
            fprintf(stderr, "OV_EBADLINK\n");
        }
        else {
            fprintf(stderr, "unknown stream error\n");
        }
        return -1;
    }
    else {
        if (bitstream != 0) {
            return -1;
        }

        *bytesConsumed = consumed;

        // calc samples and return
        int samplesRead = consumed / plugin->vorbisInfo->channels / sizeof(short int);
        return samplesRead;
    }
}


extern "C" void fuppes_close_file(void** data)
{
    VorbisData* plugin = (VorbisData*)*data;
    ov_clear(&plugin->vorbisFile);
    delete plugin;
}

extern "C" void fuppes_unregister_plugin()
{
}
