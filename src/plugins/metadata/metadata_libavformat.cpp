/*
 * This file is part of fuppes
 *
 * (c) 2008-2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <fuppes_plugin.h>
#include <fuppes_database_objects.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>

#ifdef __cplusplus
}
#endif


//#ifndef AV_METADATA_IGNORE_SUFFIX
//#define AV_METADATA_IGNORE_SUFFIX AV_DICT_IGNORE_SUFFIX
//#endif

#include <string.h>
#include <stdarg.h>


#include <iostream>

extern "C" void fuppes_register_plugin(plugin_info* plugin)
{
    plugin->type = ptMetadata;
    strcpy(plugin->name, "libavformat");

    av_log_set_level(AV_LOG_QUIET);
    av_register_all();
}

extern "C" int fuppes_open_file(const char* filename, void** data)
{
    AVFormatContext** ctx = (AVFormatContext**)data;
    *ctx = NULL; //avformat_alloc_context();

	if(0 != avformat_open_input(ctx, filename, NULL, NULL)) {
        return -1;
    }

    if(avformat_find_stream_info(*ctx, NULL) < 0) {
        avformat_close_input(ctx);
        return -1;
    }

    return 0;
}

extern "C" int fuppes_read_metadata(Database::AbstractItem& item, void** data)
{
	AVFormatContext* ctx = (AVFormatContext*)*data;

	// duration
	if (AV_NOPTS_VALUE != ctx->duration) {
		int sec = ctx->duration / AV_TIME_BASE;
		int us = ctx->duration % AV_TIME_BASE;
		item.details->av_duration = sec;
	}

	// bitrate
	if (0 != ctx->bit_rate) {
		item.details->v_bitrate = ctx->bit_rate;
	}


	for (int i = 0; i < ctx->nb_streams; i++) {

		AVStream* stream = ctx->streams[i];
		AVCodec* codec = avcodec_find_decoder(stream->codec->codec_id);
		if (NULL == codec) {
			continue;
		}

		switch (stream->codec->codec_type) {
			case AVMEDIA_TYPE_VIDEO:
				item.details->iv_width = stream->codec->width;
				item.details->iv_height = stream->codec->height;
				item.details->videoCodec = codec->name;
				break;
			case AVMEDIA_TYPE_AUDIO:
				item.details->audioCodec = codec->name;
				break;
			case AVMEDIA_TYPE_DATA:
				break;
			case AVMEDIA_TYPE_SUBTITLE:
				break;
			default:
				break;
		}
	}


    // metadata
    if (NULL == ctx->metadata) {
    	return 0;
    }

    // http://multimedia.cx/eggs/supplying-ffmpeg-with-metadata/
    AVDictionaryEntry* tag = NULL;

	/*	 while((tag = av_dict_get(ctx->metadata, "", tag, AV_METADATA_IGNORE_SUFFIX))) {
	 printf("%s => %s\n", tag->key, tag->value);
	 }	 */

	if(NULL != (tag = av_dict_get(ctx->metadata, "title", NULL, AV_DICT_IGNORE_SUFFIX))) {
		item.title = tag->value;
	}

	if(NULL != (tag = av_dict_get(ctx->metadata, "genre", NULL, AV_DICT_IGNORE_SUFFIX))) {
		item.details->av_genre = tag->value;
	}

	return 0;
}

extern "C" void fuppes_close_file(void** data)
{
    avformat_close_input((AVFormatContext**)data);
}


extern "C" void fuppes_unregister_plugin()
{
}




/*

    int fuppes_metadata_read2(plugin_info* plugin, Database::AbstractItem& item)
    {
        AVFormatContext* ctx = (AVFormatContext*)plugin->user_data;

        // duration
        if (((AVFormatContext*)plugin->user_data)->duration != AV_NOPTS_VALUE) {
            int secs, us;
            secs = ((AVFormatContext*)plugin->user_data)->duration / AV_TIME_BASE;
            us = ((AVFormatContext*)plugin->user_data)->duration % AV_TIME_BASE;
            //metadata->duration_ms = secs * 1000;
            item.details->av_duration = secs;
        }
        // bitrate
        if (((AVFormatContext*)plugin->user_data)->bit_rate)
            item.details->v_bitrate = ((AVFormatContext*)plugin->user_data)->bit_rate; // metadata->video_bitrate = ((AVFormatContext*)plugin->user_data)->bit_rate;

        // filesize
        //pVideoItem->nSize = pFormatCtx->file_size;

        //char codec_name[128];
        //char buf1[32];
        int i;
        for (i = 0; i < ((AVFormatContext*)plugin->user_data)->nb_streams; i++) {

            AVStream* pStream = ((AVFormatContext*)plugin->user_data)->streams[i];
            AVCodec* pCodec;

            pCodec = avcodec_find_decoder(pStream->codec->codec_id);
            if (!pCodec) {
                continue;
            }

            //strncpy(codec_name, (char*)pCodec->name, sizeof(codec_name));

            //printf("codec %s*\n", pCodec->name);

            switch (pStream->codec->codec_type) {
                case AVMEDIA_TYPE_VIDEO: //CODEC_TYPE_VIDEO:
                    //metadata->type		= MD_VIDEO;
                    //metadata->width 	= pStream->codec->width;
                    //metadata->height	= pStream->codec->height;
                    //set_value(metadata->video_codec, sizeof(metadata->video_codec), pCodec->name);
                    item.details->iv_width = pStream->codec->width;
                    item.details->iv_height = pStream->codec->height;
                    item.details->videoCodec = pCodec->name;
                    break;
                case AVMEDIA_TYPE_AUDIO:                        //CODEC_TYPE_AUDIO:
                    //set_value(metadata->audio_codec, sizeof(metadata->audio_codec), pCodec->name);
                    item.details->audioCodec = pCodec->name;
                    break;
                case AVMEDIA_TYPE_DATA: //CODEC_TYPE_DATA:
                    break;
                case AVMEDIA_TYPE_SUBTITLE: //CODEC_TYPE_SUBTITLE:
                    break;
                default:
                    break;
            } // switch (codec_type)

        }	// for

        // metadata
        // here is a nice list of possible metadata keys:
        // http://multimedia.cx/eggs/supplying-ffmpeg-with-metadata/
        if (ctx->metadata) {

#if LIBAVFORMAT_VERSION_MAJOR < 53
            // convert tag names to generic ffmpeg names
            av_metadata_conv(ctx, NULL, ctx->iformat->metadata_conv);

            AVMetadataTag* tag = NULL;
#elif LIBAVFORMAT_VERSION_MAJOR >= 53
            AVDictionaryEntry* tag = NULL;
#endif



            // title
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "title", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "title", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: TITLE: %s\n", tag->value);
                item.title = tag->value;
                //set_value(metadata->title, sizeof(metadata->title), tag->value);
            }

            // genre
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "genre", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "genre", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: GENRE: %s\n", tag->value);
                item.details->av_genre = tag->value;
                //set_value(metadata->genre, sizeof(metadata->genre), tag->value);
            }

            // track
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "track", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "track", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: TRACK: %s\n", tag->value);
                //set_value(&metadata->genre, tag->value);
            }

            // comment
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "comment", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "comment", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: comment: %s\n", tag->value);
                //set_value(metadata->description, sizeof(metadata->description), tag->value);
            }

            // composer
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "composer", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "composer", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: composer: %s\n", tag->value);
                //set_value(metadata->composer, sizeof(metadata->composer), tag->value);
            }

            // date
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "date", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "date", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: date: %s\n", tag->value);
                //set_value(metadata->date, sizeof(metadata->date), tag->value);
            }

            // language
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "language", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "language", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: language: %s\n", tag->value);
                //set_value(metadata->language, sizeof(metadata->language), tag->value);
            }

            // performer
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "performer", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "performer", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: performer: %s\n", tag->value);
                //set_value(&metadata->language, tag->value);
            }

            // publisher
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "publisher", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "publisher", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: publisher: %s\n", tag->value);
                //set_value(metadata->publisher, sizeof(metadata->publisher), tag->value);
            }

            // show
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "show", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "show", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: show: %s\n", tag->value);
                //set_value(metadata->series_title, sizeof(metadata->series_title), tag->value);
            }

            // episode_id
#if LIBAVFORMAT_VERSION_MAJOR < 53
            if ((tag = av_metadata_get(ctx->metadata, "episode_id", NULL, AV_METADATA_IGNORE_SUFFIX)))
            #elif LIBAVFORMAT_VERSION_MAJOR >= 53
            if((tag = av_dict_get(ctx->metadata, "episode_id", NULL, AV_METADATA_IGNORE_SUFFIX)))
#endif
            {
                //printf("libavformat metadata: episode_id: %s\n", tag->value);
                //set_value(metadata->program_title, sizeof(metadata->program_title), tag->value);
            }

        }

        return 0;
    }

    void fuppes_metadata_file_close(plugin_info* plugin)
    {
#if LIBAVFORMAT_VERSION_MAJOR < 53
        av_close_input_file((AVFormatContext*)plugin->user_data);
#elif LIBAVFORMAT_VERSION_MAJOR >= 53
        avformat_close_input((AVFormatContext**)&plugin->user_data);
#endif

        plugin->user_data = NULL;
    }

    void unregister_fuppes_plugin(plugin_info* plugin __attribute__((unused)))
    {
    }

#ifdef __cplusplus
}
#endif
*/
