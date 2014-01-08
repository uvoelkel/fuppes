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

#include <iostream>

#include <FLAC/stream_decoder.h>

typedef struct flacData_t
{
        ENDIANESS outEndianess;
        Endianness endianness;

        FLAC__StreamDecoder *decoder;
        FLAC__uint64 total_samples;

        int channels;
        int bitrate;

        char *buffer;
        int buffer_size;
        int samplerate;
        int position;
        int duration;

        unsigned char* pcm;
        int eof;
        int samples_read;
        int bytes_consumed;

} flacData_t;

FLAC__StreamDecoderWriteStatus FLAC_StreamDecoderWriteCallback(const FLAC__StreamDecoder *decoder,
        const FLAC__Frame* frame,
        const FLAC__int32* const buffer[],
        void* client_data)
{

    flacData_t* data = (flacData_t*)client_data;

    if (frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
        data->pcm = (unsigned char*)buffer;
        data->samples_read = frame->header.blocksize;
    }

    else if (frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER) {

        data->samples_read = frame->header.blocksize;

        int i;
        int j;
        int k = 0;

        for (j = 0; j < frame->header.blocksize; j++) {

            for (i = 0; i < data->channels; i++) {

                FLAC__uint16 sample = buffer[i][j];

                if (data->outEndianess == E_LITTLE_ENDIAN) {
                    data->pcm[k++] = sample;
                    data->pcm[k++] = sample >> 8;
                }
                else if (data->outEndianess == E_BIG_ENDIAN) {
                    data->pcm[k++] = sample >> 8;
                    data->pcm[k++] = sample;
                }
            }
        }

        data->bytes_consumed = k;
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

void FLAC_StreamDecoderMetadataCallback(const FLAC__StreamDecoder* decoder,
        const FLAC__StreamMetadata* metadata,
        void* client_data)

{
    flacData_t* data = (flacData_t*)client_data;

    data->total_samples = metadata->data.stream_info.total_samples;
    data->channels = metadata->data.stream_info.channels;
    data->bitrate = metadata->data.stream_info.bits_per_sample;
    data->samplerate = metadata->data.stream_info.sample_rate;
}

void FLAC_StreamDecoderErrorCallback(const FLAC__StreamDecoder* decoder,
        FLAC__StreamDecoderErrorStatus status,
        void* client_data)

{
    flacData_t* data = (flacData_t*)client_data;

    /*cout << "FLACFileDecoderErrorCallback" << endl;
     fflush(stdout);*/
}

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptDecoder;
    strcpy(plugin->name, "flac");
}

extern "C" int fuppes_open_file(const char* filename, void** data)
{
    *data = malloc(sizeof(struct flacData_t));
    flacData_t* plugin = (flacData_t*)*data;

    plugin->decoder = NULL;
    plugin->buffer = NULL;
    plugin->buffer_size = 0;
    plugin->channels = 0;
    plugin->samplerate = -1;
    plugin->duration = -1;
    plugin->eof = 0;
    plugin->samples_read = 0;
    plugin->bytes_consumed = 0;
    plugin->outEndianess = E_LITTLE_ENDIAN;
    plugin->endianness = eLittleEndian;
    plugin->decoder = FLAC__stream_decoder_new();

    FLAC__stream_decoder_init_file(plugin->decoder,
            filename,
            FLAC_StreamDecoderWriteCallback,
            FLAC_StreamDecoderMetadataCallback,
            FLAC_StreamDecoderErrorCallback,
            plugin);

    if (!FLAC__stream_decoder_process_until_end_of_metadata(plugin->decoder)) {
        FLAC__stream_decoder_delete(plugin->decoder);
        free(plugin);
        *data = NULL;
        return 1;
    }

    plugin->eof = 1;
    return 0;
}

extern "C" int fuppes_read_metadata(Database::AbstractItem& item, void** data)
{
    flacData_t* plugin = (flacData_t*)*data;

    item.details->a_channels = plugin->channels;
    item.details->a_bitrate = plugin->bitrate;
    item.details->a_samplerate = plugin->samplerate;
    return 0;
}

extern "C" int fuppes_set_endianness(Endianness endianness, void** data)
{
	flacData_t* plugin = (flacData_t*)*data;
    plugin->endianness = endianness;
    return 0;
}

extern "C" int fuppes_set_out_endianess(ENDIANESS endianess, void** data)
{
    flacData_t* plugin = (flacData_t*)*data;
    plugin->outEndianess = endianess;
    return 0;
}

extern "C" fuppes_int64_t fuppes_get_total_samples(void** data)
{
    flacData_t* plugin = (flacData_t*)*data;
    return plugin->total_samples;
}

extern "C" int fuppes_decode_interleaved(unsigned char* buffer, size_t size, size_t* bytesConsumed, void** data)
{
    flacData_t* plugin = (flacData_t*)*data;
    plugin->pcm = buffer;

    if (plugin->eof == 0) {
        return 0;
    }

    if (FLAC__stream_decoder_get_state(plugin->decoder) == FLAC__STREAM_DECODER_END_OF_STREAM) {
        FLAC__stream_decoder_finish(plugin->decoder);
        plugin->eof = 0;
        return plugin->samples_read;
    }

    if (FLAC__stream_decoder_process_single(plugin->decoder)) {
        *bytesConsumed = plugin->bytes_consumed;
        return plugin->samples_read;
    }

    return -1;
}

extern "C" void fuppes_close_file(void** data)
{
    flacData_t* plugin = (flacData_t*)*data;
    if (NULL != plugin->decoder) {
        FLAC__stream_decoder_delete(plugin->decoder);
    }
    free(plugin);
}

extern "C" void fuppes_unregister_plugin()
{
}

/*
 int fuppes_decoder_set_out_endianess(plugin_info* plugin, ENDIANESS endianess)
 {
 flacData_t* data = (flacData_t*)plugin->user_data;
 data->outEndianess = endianess;
 return 0;
 }

 int fuppes_decoder_total_samples(plugin_info* plugin)
 {
 flacData_t* data = (flacData_t*)plugin->user_data;
 return data->total_samples;
 }

 int fuppes_decoder_decode_interleaved(plugin_info* plugin, char* pcmOut, int bufferSize, int* bytesRead)
 {
 flacData_t* data = (flacData_t*)plugin->user_data;

 if (data->eof == 0) {
 return -1;
 }

 data->pcm = pcmOut;

 #ifdef HAVE_FLAC_FILEDECODER
 if(FLAC__file_decoder_get_state(data->decoder) == FLAC__FILE_DECODER_END_OF_FILE) {
 FLAC__file_decoder_finish(data->decoder);
 #else
 if (FLAC__stream_decoder_get_state(data->decoder) == FLAC__STREAM_DECODER_END_OF_STREAM) {
 FLAC__stream_decoder_finish(data->decoder);
 #endif
 data->eof = 0;
 return data->samples_read;
 }

 #ifdef HAVE_FLAC_FILEDECODER
 if(FLAC__file_decoder_process_single(data->decoder)) {
 #else
 if (FLAC__stream_decoder_process_single(data->decoder)) {
 #endif
 *bytesRead = data->bytes_consumed;
 return data->samples_read;
 }

 return -1;
 }
 */
