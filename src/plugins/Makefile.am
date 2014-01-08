include $(top_srcdir)/PreRules.am

libdir = @libdir@/fuppes


if !BUILD_TESTS_ONLY


AM_CPPFLAGS = \
    -I$(top_srcdir)/include
lib_LTLIBRARIES =


##
##  DATABASE PLUGINS
##

## sqlite3

lib_LTLIBRARIES += libdatabase_sqlite3.la

libdatabase_sqlite3_la_SOURCES = \
	database/sqlite/QueryBuilder.h \
	database/sqlite/QueryBuilder.cpp \
	database/sqlite/Connection.h \
	database/sqlite/Connection.cpp \
	database/sqlite/TableManager.h \
	database/sqlite/TableManager.cpp \
	database/sqlite/ObjectManager.h \
	database/sqlite/ObjectManager.cpp \
	database/sqlite/SortCriteria.h \
	database/sqlite/SortCriteria.cpp \
	database/sqlite/SearchCriteria.h \
	database/sqlite/SearchCriteria.cpp \
	database/sqlite/PreparedStatement.h \
	database/sqlite/PreparedStatement.cpp \
	database/sqlite/database_sqlite3.cpp

libdatabase_sqlite3_la_CXXFLAGS = \
	$(SQLITE3_CFLAGS)
	
libdatabase_sqlite3_la_LDFLAGS = \
	$(SQLITE3_LIBS)


## MySQL

#if MYSQL_DATABASE_PLUGIN
#lib_LTLIBRARIES += libdatabase_mysql.la

#libdatabase_mysql_la_SOURCES = \
#	database/mysql/QueryBuilder.h \
#	database/mysql/QueryBuilder.cpp \
#	database/mysql/Connection.h \
#	database/mysql/Connection.cpp \
#	database/mysql/TableManager.h \
#	database/mysql/TableManager.cpp \
#	database/mysql/ObjectManager.h \
#	database/mysql/ObjectManager.cpp \
#	database/mysql/SortCriteria.h \
#	database/mysql/SortCriteria.cpp \
#	database/mysql/SearchCriteria.h \
#	database/mysql/SearchCriteria.cpp \
#	database/mysql/PreparedStatement.h \
#	database/mysql/PreparedStatement.cpp \
#	database/mysql/database_mysql.cpp

#libdatabase_mysql_la_CXXFLAGS = \
#	$(MYSQL_CFLAGS) \
#	$(SQLITE3_CFLAGS)
	
#libdatabase_mysql_la_LDFLAGS = \
#	$(MYSQL_LIBS) \
#	$(SQLITE3_LIBS)
#endif


##
##  METADATA PLUGINS
##

## libavformat metadata extraction plugin

if LIBAVFORMAT_METADATA_PLUGIN
lib_LTLIBRARIES += libmetadata_libavformat.la
libmetadata_libavformat_la_SOURCES = metadata/metadata_libavformat.cpp
libmetadata_libavformat_la_CXXFLAGS = \
    -D__STDC_CONSTANT_MACROS \
	$(LIBAVFORMAT_CFLAGS) \
	$(LIBAVCODEC_CFLAGS) \
	$(LIBAVUTIL_CFLAGS)
	
libmetadata_libavformat_la_LDFLAGS = \
	$(LIBAVFORMAT_LIBS) \
	$(LIBAVCODEC_LIBS) \
	$(LIBAVUTIL_LIBS)
endif


## taglib metadata plugin
if TAGLIB_METADATA_PLUGIN
lib_LTLIBRARIES += libmetadata_taglib.la
libmetadata_taglib_la_SOURCES = metadata/metadata_taglib.cpp
libmetadata_taglib_la_CXXFLAGS = $(TAGLIB_CFLAGS)
libmetadata_taglib_la_LDFLAGS = $(TAGLIB_LIBS)
endif

## simage metadata plugin
#if SIMAGE_METADATA_PLUGIN
#lib_LTLIBRARIES += libmetadata_simage.la
#libmetadata_simage_la_SOURCES = metadata/metadata_simage.cpp
#libmetadata_simage_la_CXXFLAGS = $(SIMAGE_CFLAGS)	
#libmetadata_simage_la_LDFLAGS = $(SIMAGE_LIBS)
#endif

## magickWand metadata plugin
#if MAGICK_WAND_METADATA_PLUGIN
#lib_LTLIBRARIES += libmetadata_magickwand.la
#libmetadata_magickwand_la_SOURCES = metadata/metadata_magickwand.cpp
#libmetadata_magickwand_la_CXXFLAGS = $(IMAGEMAGICK_WAND_CFLAGS)	
#libmetadata_magickwand_la_LDFLAGS = $(IMAGEMAGICK_WAND_LIBS)
#endif

## exiv2 metadata plugin
#if EXIV2_METADATA_PLUGIN
#lib_LTLIBRARIES += libmetadata_exiv2.la
#libmetadata_exiv2_la_SOURCES = metadata/metadata_exiv2.cpp
#libmetadata_exiv2_la_CXXFLAGS = $(EXIV2_CFLAGS)	
#libmetadata_exiv2_la_LDFLAGS = $(EXIV2_LIBS)
#endif

## libmp4v2 metadata plugin
#if MP4V2_METADATA_PLUGIN
#lib_LTLIBRARIES += libmetadata_libmp4v2.la
#libmetadata_libmp4v2_la_SOURCES = metadata/metadata_libmp4v2.cpp
#libmetadata_libmp4v2_la_CXXFLAGS = $(MP4V2_CFLAGS)	
#libmetadata_libmp4v2_la_LDFLAGS = $(MP4V2_LIBS)
#endif


## ffmpegthumbnailer metadata plugin
if FFMPEGTHUMBNAILER_METADATA_PLUGIN
lib_LTLIBRARIES += libmetadata_ffmpegthumbnailer.la
libmetadata_ffmpegthumbnailer_la_SOURCES = metadata/metadata_ffmpegthumbnailer.cpp
libmetadata_ffmpegthumbnailer_la_CXXFLAGS = \
	$(FFMPEGTHUMBNAILER_CFLAGS) \
	$(LIBAVFORMAT_CFLAGS) \
	$(LIBAVCODEC_CFLAGS) \
	$(LIBAVUTIL_CFLAGS) \
	-D__STDC_CONSTANT_MACROS
libmetadata_ffmpegthumbnailer_la_LDFLAGS = \
	$(FFMPEGTHUMBNAILER_LIBS) \
	$(LIBAVFORMAT_LIBS) \
	$(LIBAVCODEC_LIBS) \
	$(LIBAVUTIL_LIBS)
endif


##
##  TRANSCODER PLUGINS
##


## ffmpeg transcoder
#if FFMPEG_TRANSCODER_PLUGIN

#lib_LTLIBRARIES += libtranscoder_ffmpeg.la

#libtranscoder_ffmpeg_la_SOURCES = \
#	transcoder_ffmpeg.cpp \
#	ffmpeg/cmdutils.cpp \
#	ffmpeg/cmdutils.h \
#	ffmpeg/ffmpeg.cpp \
#	ffmpeg/ffmpeg.h

#libtranscoder_ffmpeg_la_CXXFLAGS = \
#	$(LIBAVFORMAT_CFLAGS) \
#	$(LIBAVCODEC_CFLAGS) \
#	$(LIBAVUTIL_CFLAGS) \
#	$(LIBSWSCALE_CFLAGS) \
#	-D__STDC_CONSTANT_MACROS
	
#libtranscoder_ffmpeg_la_LDFLAGS = \
#	$(LIBAVFORMAT_LIBS) \
#	$(LIBAVCODEC_LIBS) \
#	$(LIBAVUTIL_LIBS) \
#	$(LIBSWSCALE_LIBS)

#endif


## MagickWand transcoder
#if MAGICK_WAND_TRANSCODER_PLUGIN
#lib_LTLIBRARIES += libtranscoder_magickwand.la
#libtranscoder_magickwand_la_SOURCES = transcoder/transcoder_magickwand.cpp
#libtranscoder_magickwand_la_CXXFLAGS = $(IMAGEMAGICK_WAND_CFLAGS)	
#libtranscoder_magickwand_la_LDFLAGS = $(IMAGEMAGICK_WAND_LIBS)
#endif



##
##  AUDIO DECODER PLUGINS
##

## wav decoder
lib_LTLIBRARIES += libdecoder_wav.la
libdecoder_wav_la_SOURCES = decoder/decoder_wav.cpp
libdecoder_wav_la_CXXFLAGS =	
libdecoder_wav_la_LDFLAGS =

## vorbis decoder
if VORBIS_DECODER_PLUGIN
lib_LTLIBRARIES += libdecoder_vorbis.la
libdecoder_vorbis_la_SOURCES = decoder/decoder_vorbis.cpp
libdecoder_vorbis_la_CXXFLAGS = $(VORBISFILE_CFLAGS)	
libdecoder_vorbis_la_LDFLAGS = $(VORBISFILE_LIBS)
endif

## flac decoder
if FLAC_DECODER_PLUGIN
lib_LTLIBRARIES += libdecoder_flac.la
libdecoder_flac_la_SOURCES = decoder/decoder_flac.cpp
libdecoder_flac_la_CXXFLAGS = $(FLAC_CFLAGS)	
libdecoder_flac_la_LDFLAGS = $(FLAC_LIBS)
endif


## musepack decoder
#if MUSEPACK_DECODER_PLUGIN
#lib_LTLIBRARIES += libdecoder_musepack.la
#libdecoder_musepack_la_SOURCES = decoder/decoder_musepack.c
#libdecoder_musepack_la_CFLAGS = $(MUSEPACK_CFLAGS)	
#libdecoder_musepack_la_LDFLAGS = $(MUSEPACK_LIBS)
#endif

## twolame decoder
#if TWOLAME_ENCODER_PLUGIN
#lib_LTLIBRARIES += libencoder_twolame.la
#libencoder_twolame_la_SOURCES = encoder_twolame.c
#libencoder_twolame_la_CFLAGS = $(TWOLAME_CFLAGS)	
#libencoder_twolame_la_LDFLAGS = $(TWOLAME_LIBS)
#endif


## opus decoder
#if OPUS_DECODER_PLUGIN
#lib_LTLIBRARIES += libdecoder_opus.la
#libdecoder_opus_la_SOURCES = decoder/decoder_opus.cpp
#libdecoder_opus_la_CXXFLAGS = $(OPUS_CFLAGS)	
#libdecoder_opus_la_LDFLAGS = $(OPUS_LIBS)
#endif


##
##  AUDIO ENCODER PLUGINS
##

## pcm encoder
lib_LTLIBRARIES += libencoder_pcm.la
libencoder_pcm_la_SOURCES = encoder/encoder_pcm.cpp
libencoder_pcm_la_CXXFLAGS =	
libencoder_pcm_la_LDFLAGS =

## wav encoder
lib_LTLIBRARIES += libencoder_wav.la
libencoder_wav_la_SOURCES = encoder/encoder_wav.cpp
libencoder_wav_la_CXXFLAGS =	
libencoder_wav_la_LDFLAGS =

## lame  encoder
if LAME_ENCODER_PLUGIN
lib_LTLIBRARIES += libencoder_lame.la
libencoder_lame_la_SOURCES = encoder/encoder_lame.cpp
libencoder_lame_la_CXXFLAGS = $(LAME_H_DIR)
libencoder_lame_la_LDFLAGS = $(LAME_LIBS)
endif


##
##  DATA SOURCE PLUGINS
##

#lib_LTLIBRARIES += libsource_itunes.la
#libsource_itunes_la_SOURCES = source/source_itunes.c
#libsource_itunes_la_CFLAGS = $(LIBXML_CFLAGS)
#libsource_itunes_la_LDFLAGS = $(LIBXML_LIBS)

#if SOURCE_PLUGIN_DVB
#lib_LTLIBRARIES += libsource_dvb.la
#libsource_dvb_la_SOURCES = source/source_dvb.cpp
#libsource_dvb_la_CXXFLAGS =
#libsource_dvb_la_LDFLAGS =  
#endif

#if SOURCE_PLUGIN_BLIPTV
#lib_LTLIBRARIES += libsource_bliptv.la
#libsource_bliptv_la_SOURCES = source/source_bliptv.cpp
#libsource_bliptv_la_CXXFLAGS = $(CURL_CFLAGS) $(LIBXML_CFLAGS)
#libsource_bliptv_la_LDFLAGS = $(CURL_LIBS) $(LIBXML_LIBS)  
#endif

#if SOURCE_PLUGIN_YOUTUBEPLAYLIST
#lib_LTLIBRARIES += libsource_youtubeplaylist.la
#libsource_youtubeplaylist_la_SOURCES = source/source_youtubeplaylist.cpp
#libsource_youtubeplaylist_la_CXXFLAGS = $(CURL_CFLAGS) $(LIBXML_CFLAGS)	
#libsource_youtubeplaylist_la_LDFLAGS = $(CURL_LIBS) $(LIBXML_LIBS)
#endif


endif # tests only
