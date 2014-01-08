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

/*
  WRITE_U32 and WRITE_U16 taken from oggdec.c
  from the vorbis-tools-1.1.1 package (GPLv2)
*/

#define WRITE_U32(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);\
                          *((buf)+2) = (unsigned char)(((x)>>16)&0xff);\
                          *((buf)+3) = (unsigned char)(((x)>>24)&0xff);

#define WRITE_U16(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);


class Wav
{
	public:
		Wav() {
			samplerate = 0;
			channels = 0;
			numSamples = 0;

			first = true;
			firstBuffer = false;

			buffer = NULL;
		}

		~Wav() {
			if (firstBuffer) {
				free(buffer);
			}
		}

		int	 samplerate;
		int	 channels;
		int	 numSamples;

		bool first;
		bool firstBuffer;

		unsigned char* buffer;

		void createHeader() {

			/*
			  some lines taken from oggdec.c
			  from the vorbis-tools-1.1.1 package (GPLv2)
			*/
			int bits = 16;
			//unsigned int size = -1;
			//int channels    = settings->channels;
			//int samplerate  = settings->samplerate;
			int bytespersec = channels * samplerate * bits / 8;
			int align       = channels * bits / 8;
			int samplesize  = bits;

			memset(buffer, 0, 44);

			unsigned int size = (unsigned int)(numSamples*bits/8*channels+44);
			memcpy(buffer, "RIFF", 4);
			WRITE_U32(buffer+4, size-8);
			memcpy(buffer+8, "WAVE", 4);
			memcpy(buffer+12, "fmt ", 4);
			WRITE_U32(buffer+16, 16);
			WRITE_U16(buffer+20, 1); /* format */
			WRITE_U16(buffer+22, channels);
			WRITE_U32(buffer+24, samplerate);
			WRITE_U32(buffer+28, bytespersec);
			WRITE_U16(buffer+32, align);
			WRITE_U16(buffer+34, samplesize);
			memcpy(buffer+36, "data", 4);
			WRITE_U32(buffer+40, size - 44);
		}
};


extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptEncoder;
    strcpy(plugin->name, "wav");
}

extern "C" int fuppes_encoder_setup(Transcoding::AbstractItem& item, void** data)
{
    if (NULL == item.getMetadata().details) {
    	return 1;
    }

	*data = new Wav();
	Wav* plugin = (Wav*)*data;

    plugin->channels = item.getMetadata().details->a_channels;
	plugin->samplerate = item.getMetadata().details->a_samplerate;
	plugin->numSamples = item.totalSamples;

	return 0;
}

extern "C" fuppes_off_t fuppes_encoder_guess_content_length(fuppes_int64_t samples, void** data)
{
    if(samples == 0) {
      return 0;
    }

    Wav* settings = (Wav*)*data;
    return ((16 * samples * settings->channels) / 8) + 44;
}

extern "C" size_t fuppes_encoder_interleaved(unsigned char* buffer, size_t size, int samples, void** data)
{
	Wav* plugin = (Wav*)*data;

	if (plugin->first) {
		plugin->buffer = (unsigned char*)malloc(size + 44);
		plugin->createHeader();
		memcpy(&plugin->buffer[44], buffer, size);
		plugin->first = false;
		plugin->firstBuffer = true;
		return (size + 44);
	}

	if (plugin->firstBuffer) {
		free(plugin->buffer);
		plugin->firstBuffer = false;
	}

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
    Wav* plugin = (Wav*)*data;
    return plugin->buffer;
}

extern "C" int fuppes_encoder_teardown(void** data)
{
	if (NULL == *data) {
	    return 1;
	}

    Wav* plugin = (Wav*)*data;
	delete plugin;
	return 0;
}

extern "C" void fuppes_unregister_plugin()
{
}

