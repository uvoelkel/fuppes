dnl Process this file with autoconf to produce a configure script.
dnl $Id: $

m4_include([m4/version.m4])
AC_INIT([fuppes], VERSION_NUMBER, [fuppes@ulrich-voelkel.de])
AM_CONFIG_HEADER(src/config.h)
AM_INIT_AUTOMAKE([1.9 subdir-objects])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AH_TEMPLATE([FUPPES_VERSION], [The Current Version Number of FUPPES])
AC_DEFINE([FUPPES_VERSION], "[VERSION_NUMBER]")

AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_PROG_CXX
AC_DISABLE_STATIC
#AC_DISABLE_SHARED
AC_LIBTOOL_WIN32_DLL
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PROG_INSTALL_STRIP

AC_FUNC_FSEEKO
AC_TYPE_OFF_T
AC_SYS_LARGEFILE
AC_C_BIGENDIAN

PKG_PROG_PKG_CONFIG

FUPPES_CFLAGS=""
FUPPES_LIBS=""

# CHECK HOST
is_windows=false
build_daemon=true
AC_MSG_CHECKING([whether we are building with mingw])
case $host in 
  *-*-mingw32*)
    AC_MSG_RESULT([yes])

    FUPPES_LIBS="$FUPPES_LIBS -lws2_32 -lole32 -liphlpapi"
		LDFLAGS="$LDFLAGS -no-undefined"

    AC_CHECK_TOOL(WINDRES, windres)
		build_daemon=false
		is_windows=true
	;;
	*)
    AC_MSG_RESULT([no])
	;;
esac
AM_CONDITIONAL([BUILD_DAEMON], [test x$build_daemon = xtrue])
AM_CONDITIONAL([WINDOWS_BUILD], [test x$is_windows = xtrue])
AM_CONDITIONAL([WIN32], [test x$is_windows = xtrue])

AC_ARG_ENABLE([windows-service], [AC_HELP_STRING([--enable-windows-service],
    [build windows service [default=no]])],
    [enable_windows_service=$enableval], [enable_windows_service='no'])
AM_CONDITIONAL([WINDOWS_SERVICE], [test "x$enable_windows_service" = "xyes" -a "x$is_windows" = "xtrue"])


dnl Checks for header files.

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h string.h sys/ioctl.h sys/param.h sys/socket.h unistd.h sys/utsname.h syslog.h])


AH_TEMPLATE([HAVE_SELECT], [])
if test x$is_windows = xfalse; then
  AC_CHECK_FUNCS([select], [], AC_ERROR([select not found]))
else
  AC_DEFINE([HAVE_SELECT], [1], [])
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_SIZEOF(off_t)
AC_CHECK_SIZEOF(long long int)
AC_CHECK_SIZEOF(unsigned long long int)
AC_CHECK_SIZEOF(long int)
AC_CHECK_SIZEOF(unsigned long int)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(unsigned short)


dnl expand paths and add them to config.h

#SYSCONFDIR=`eval echo $sysconfdir/fuppes`
#AH_TEMPLATE([FUPPES_SYSCONFDIR], [])
#AC_DEFINE_UNQUOTED([FUPPES_SYSCONFDIR], "[$SYSCONFDIR]", [])

#DATADIR=`eval echo $datadir/fuppes`
#AH_TEMPLATE([FUPPES_DATADIR], [])
#AC_DEFINE_UNQUOTED([FUPPES_DATADIR], "[$DATADIR]", [])

#PLUGINDIR=`eval echo $libdir/fuppes`
#AH_TEMPLATE([FUPPES_PLUGINDIR], [])
#AC_DEFINE_UNQUOTED([FUPPES_PLUGINDIR], "[$PLUGINDIR]", [])



	



dnl Checks for libraries.

echo ""
echo "check minimum dependencies"
echo ""

PKG_CHECK_MODULES(PCRE, libpcre >= 5.0)
PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= 2.7.0)
PKG_CHECK_MODULES(SQLITE3, sqlite3 >= 3.2)
AC_CHECK_LIB(pthread, pthread_create, FUPPES_LIBS="$FUPPES_LIBS -lpthread")

have_clock_gettime=no
AH_TEMPLATE([HAVE_CLOCK_GETTIME], [])
AC_CHECK_LIB(rt, clock_gettime, have_clock_gettime=yes)
if test "x$have_clock_gettime" = "xyes"; then
	FUPPES_LIBS="$FUPPES_LIBS -lrt"
	AC_DEFINE([HAVE_CLOCK_GETTIME], [1], [])
fi



have_ev=no
AH_TEMPLATE([HAVE_LIBEV], [])
AC_CHECK_HEADERS([ev.h], [have_ev=yes])
if test "x$have_ev" = "xyes"; then      
  FUPPES_LIBS="$FUPPES_LIBS -lev"
  AC_DEFINE([HAVE_LIBEV], [1], [])
fi

#libevent_pthreads
have_event=no
AH_TEMPLATE([HAVE_LIBEVENT2], [])
#if test "x$have_ev" = "xno"; then
#    if test "x$is_windows" = "xfalse"; then
#        PKG_CHECK_MODULES(LIBEVENT2, libevent_pthreads >= 2.0, have_event=yes, have_event=no)
#    else
#        PKG_CHECK_MODULES(LIBEVENT2, libevent >= 2.0, have_event=yes, have_event=no)
#    fi
#fi

if test "x$have_event" = "xyes"; then
AC_DEFINE([HAVE_LIBEVENT2], [1], [])
FUPPES_CFLAGS="$FUPPES_CFLAGS $LIBEVENT2_CFLAGS"
FUPPES_LIBS="$FUPPES_LIBS $LIBEVENT2_LIBS"
fi

if test "x$have_event" = "xno"; then
    if test "x$have_ev" = "xno"; then
        AC_MSG_ERROR("libev not found")
        #you need either libev or libevent
    fi
fi

echo ""
echo "check recommended dependencies"
echo ""

# uuid
have_uuid=no
AH_TEMPLATE([HAVE_UUID], [])
PKG_CHECK_MODULES(UUID, uuid >= 0, have_uuid=yes, have_uuid=no)
if test "x$have_uuid" = "xyes"; then
  AC_DEFINE([HAVE_UUID], [1], [])
fi
AC_SUBST(UUID_CFLAGS)
AC_SUBST(UUID_LIBS)


AM_ICONV
if test x$is_windows = xtrue; then
# the above iconv macro executes some tests that require a shared iconv lib.
# if we are crosscompiling we can use  wine to execute the tests (wine-bin on debian)
# or we just set the LDFLAGS if at least the  iconv function declaration is found
	if test "x$am_cv_func_iconv" = "xyes"; then
		am_func_iconv=yes
		FUPPES_LIBS="$FUPPES_LIBS -liconv"
		#AC_SUBST(LIBICONV)
		AC_DEFINE(HAVE_ICONV, 1)
	fi
fi



echo ""
echo "check optional stuff"
echo ""

#
# TAGLIB
#
have_taglib=no
AC_ARG_ENABLE([taglib], [AC_HELP_STRING([--enable-taglib],
    [enable taglib metadata plugin [default=yes]])],
    [enable_taglib=$enableval], [enable_taglib='yes'])
if test "x$enable_taglib" = "xyes" ; then
  PKG_CHECK_MODULES(TAGLIB, taglib >= 1.6, have_taglib=yes, have_taglib=no)
fi
AM_CONDITIONAL(TAGLIB_METADATA_PLUGIN, test "x$have_taglib" = "xyes")
AC_SUBST(TAGLIB_CFLAGS)
AC_SUBST(TAGLIB_LIBS)
# END TAGLIB


#
# libffmpegthumbnailer
#
have_ffmpegthumbnailer=no
AC_ARG_ENABLE([ffmpegthumbnailer], [AC_HELP_STRING([--enable-ffmpegthumbnailer],
    [enable ffmpegthumbnailer metadata plugin [default=yes]])],
    [enable_ffmpegthumbnailer=$enableval], [enable_ffmpegthumbnailer='yes'])
if test "x$enable_ffmpegthumbnailer" = "xyes" ; then
  PKG_CHECK_MODULES(FFMPEGTHUMBNAILER, libffmpegthumbnailer >= 2.0.0, have_ffmpegthumbnailer=yes, have_ffmpegthumbnailer=no)
fi
AM_CONDITIONAL(FFMPEGTHUMBNAILER_METADATA_PLUGIN, test "x$have_ffmpegthumbnailer" = "xyes")
AC_SUBST(FFMPEGTHUMBNAILER_CFLAGS)
AC_SUBST(FFMPEGTHUMBNAILER_LIBS)



#
# ImageMagick (MagickWand)
#
have_magickwand=no
AC_ARG_ENABLE([magickwand], [AC_HELP_STRING([--enable-magickwand],
    [enable ImageMagick metadata/transcoder plugin [default=yes]])],
    [enable_magickwand=$enableval], [enable_magickwand='yes'])
if test "x$enable_magickwand" = "xyes" ; then
  PKG_CHECK_MODULES(IMAGEMAGICK_WAND, Wand >= 6.2.4, have_magickwand=yes, have_magickwand=no)
fi
AM_CONDITIONAL(MAGICK_WAND_METADATA_PLUGIN, test "x$have_magickwand" = "xyes")
AM_CONDITIONAL(MAGICK_WAND_TRANSCODER_PLUGIN, test "x$have_magickwand" = "xyes")



#
# Exiv2
#
have_exiv2=no
AC_ARG_ENABLE([exiv2], [AC_HELP_STRING([--enable-exiv2],
    [enable exiv2 metadata plugin [default=yes]])],
    [enable_exiv2=$enableval], [enable_exiv2='yes'])
if test "x$enable_exiv2" = "xyes" ; then
  PKG_CHECK_MODULES(EXIV2, exiv2 >= 0.16, have_exiv2=yes, have_exiv2=no)
fi
AM_CONDITIONAL(EXIV2_METADATA_PLUGIN, test "x$have_exiv2" = "xyes")
AC_SUBST(EXIV2_CFLAGS)
AC_SUBST(EXIV2_LIBS)




#
# libcurl (for tests. may also be used by plugins)
#
have_curl=no
PKG_CHECK_MODULES(CURL, libcurl >= 2.0.0, have_curl=yes, have_curl=no)
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)



#
# blip.tv plugin
#
#have_bliptv=no
#AC_ARG_ENABLE([blip.tv], [AC_HELP_STRING([--enable-bliptv],
#    [enable blib.tv plugin [default=no]])],
#    [enable_bliptv=$enableval], [enable_bliptv='no'])
#if test "x$enable_bliptv" = "xyes" ; then
#  have_bliptv=$have_curl
#fi
#AM_CONDITIONAL(SOURCE_PLUGIN_BLIPTV, test "x$have_bliptv" = "xyes")

#
# youtube playlist plugin
#
#have_youtubeplaylist=no
#AC_ARG_ENABLE([youtubeplaylist], [AC_HELP_STRING([--enable-youtubeplaylist],
#    [enable youtube playlist plugin [default=no]])],
#    [enable_youtubeplaylist=$enableval], [enable_youtubeplaylist='no'])
#if test "x$enable_youtubeplaylist" = "xyes" ; then
#  have_youtubeplaylist=$have_curl
#fi
#AM_CONDITIONAL(SOURCE_PLUGIN_YOUTUBEPLAYLIST, test "x$have_youtubeplaylist" = "xyes")

#
# dvb plugin
#
#have_source_dvb=no
#AC_ARG_ENABLE([dvb], [AC_HELP_STRING([--enable-dvb],
#    [enable dvb plugin [default=no]])],
#    [enable_source_dvb=$enableval], [enable_source_dvb='no'])
#if test "x$enable_source_dvb" = "xyes" ; then
#  have_source_dvb = $enable_source_dvb
#fi
#AM_CONDITIONAL(SOURCE_PLUGIN_DVB, test "x$have_source_dvb" = "xyes")




#
# libavformat
#
AH_TEMPLATE([HAVE_LIBAVFORMAT], [])
use_libavformat=yes
AC_ARG_ENABLE(libavformat, 
            [  --disable-libavformat  Do not use libavformat (from the FFmpeg package)],
            [ use_libavformat=no; ] )

have_libavformat=no

if test "x$use_libavformat" = "xyes" ; then
  PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 52, have_libavformat=yes, have_libavformat=no)
  PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 52, have_libavcodec=yes, have_libavcodec=no)
  PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 50, have_libavutil=yes, have_libavutil=no)
fi

if test "x$have_libavformat" = "xyes"; then     
    AC_DEFINE([HAVE_LIBAVFORMAT], [1], [])  
fi 
AM_CONDITIONAL(LIBAVFORMAT_METADATA_PLUGIN, test "x$have_libavformat" = "xyes")


#have_transcoder_ffmpeg=no
#AC_ARG_ENABLE([transcoder-ffmpeg], [AC_HELP_STRING([--enable-transcoder-ffmpeg],
#    [enable ffmpeg video transcoder plugin [default=no]])],
#    [have_transcoder_ffmpeg=$enableval], [have_transcoder_ffmpeg='no'])
#if test "x$have_libavformat" = "xno" ; then
#	have_transcoder_ffmpeg=no
#fi
#AM_CONDITIONAL(FFMPEG_TRANSCODER_PLUGIN, test "x$have_transcoder_ffmpeg" = "xyes")
# end libavformat


# check for mp4v2
have_mp4v2=no
AC_ARG_ENABLE([mp4v2], [AC_HELP_STRING([--enable-mp4v2],
    [enable mp4v2 metadata plugin [default=yes]])],
    [enable_mp4v2=$enableval], [enable_mp4v2='yes'])


if test "x$enable_mp4v2" = "xyes" ; then
	AH_TEMPLATE([HAVE_LAME], [])
	AC_CHECK_HEADERS([mp4v2/mp4v2.h], have_mp4v2=yes, have_mp4v2=no)
	if test "x$have_mp4v2" = "xyes"; then    
		MP4V2_CFLAGS=""
		MP4V2_LIBS="-lmp4v2"
	fi
fi
AM_CONDITIONAL(MP4V2_METADATA_PLUGIN, test "x$have_mp4v2" = "xyes")


              

# check for LAME
have_lame=no
LAME_LIBS=""
AC_ARG_ENABLE([lame], [AC_HELP_STRING([--enable-lame],
    [enable lame encoder plugin [default=no]])],
    [enable_lame=$enableval], [enable_lame='no'])

AC_ARG_WITH(lame-dir, AC_HELP_STRING([--with-lame-dir=path],
            [include path of lame/lame.h]),
            [
	      	if test "x$withval" != "x"; then
	          LAME_H_DIR="$withval/"
		  CXXFLAGS="$CXXFLAGS -I$withval"
		fi
	    ],
	    [LAME_H_DIR=""])
AC_SUBST(LAME_H_DIR)

if test "x$enable_lame" = "xyes" ; then

	AH_TEMPLATE([HAVE_LAME], [])
	AC_CHECK_HEADERS([${LAME_H_DIR}lame/lame.h], have_lame=yes, have_lame=no)
	if test "x$have_lame" = "xyes"; then    
		LAME_LIBS="-lmp3lame"
		AC_DEFINE([HAVE_LAME], [1], [])  
	fi
fi

# check for TwoLAME
have_twolame=no
TWOLAME_LIBS=""
AC_ARG_ENABLE([twolame], [AC_HELP_STRING([--enable-twolame],
    [enable twolame encoder plugin [default=no]])],
    [enable_twolame=$enableval], [enable_twolame='no'])
    
if test "x$enable_twolame" = "xyes" ; then
	AH_TEMPLATE([HAVE_TWOLAME], [])
	PKG_CHECK_MODULES(TWOLAME, twolame >= 0.3.8, have_twolame=yes, have_twolame=no)
	if test "x$have_twolame" = "xyes"; then
		AC_DEFINE([HAVE_TWOLAME], [1], [])
	fi
fi		




# vorbis decoder plugin
have_vorbis=no
have_tremor=no
AC_ARG_ENABLE([vorbis], [AC_HELP_STRING([--enable-vorbis],
    [enable vorbis decoder plugin [default=yes]])],
    [enable_vorbis=$enableval], [enable_vorbis='yes'])
AC_ARG_ENABLE([tremor], [AC_HELP_STRING([--enable-tremor],
    [use tremor instead of libvorbisfile [default=no]])],
    [enable_tremor=$enableval], [enable_tremor='no'])
    
if test "x$enable_vorbis" = "xyes" ; then

  TREMOR_H_DIR=""
  if test "x$enable_tremor" = "xyes"; then
    AC_CHECK_HEADERS(${TREMOR_H_DIR}tremor/ivorbisfile.h, have_tremor=yes, have_tremor=no)
    if test "x$have_tremor" = "xyes"; then
      VORBISFILE_CFLAGS="-DTREMOR"
		else
			echo "Tremor not found. Checking for libvorbisfile instead."
		fi
  fi

  if test "x$have_tremor" = "xno"; then
    PKG_CHECK_MODULES(VORBISFILE, vorbisfile >= 1.0, have_vorbis=yes, have_vorbis=no)    
  fi

fi


# musepack decoder plugin
MUSEPACK_CFLAGS=""
MUSEPACK_LIBS=""
have_mpc=no
AC_ARG_ENABLE([musepack], [AC_HELP_STRING([--enable-musepack],
    [enable musepack decoder plugin [default=yes]])],
    [enable_musepack=$enableval], [enable_musepack='yes'])

AC_ARG_WITH(musepack-dir, AC_HELP_STRING([--with-musepack-dir=path],
        [include path of mpcdec/mpcdec.h]),
        [
            if test "x$withval" != "x"; then
              MPC_H_DIR="$withval/"
              CXXFLAGS="$CXXFLAGS -I$withval"
            fi
        ],
        [MPC_H_DIR=""])
      AC_SUBST(MPC_H_DIR)

if test "x$enable_musepack" = "xyes" ; then
    AC_CHECK_HEADERS(${MPC_H_DIR}mpcdec/config_types.h, have_mpc=yes, have_mpc=no)
    if test "x$have_mpc" = "xno"; then
      AC_CHECK_HEADERS(${MPC_H_DIR}mpc/mpcdec.h, have_mpc=yes, have_mpc=no)
      MUSEPACK_CFLAGS="-DMUSEPACK2"
    fi
    if test "x$have_mpc" = "xyes"; then
      MUSEPACK_LIBS="-lmpcdec"      
		fi
fi



# flac decoder plugin
have_flac=no
FLAC_LIBS=""
AC_ARG_ENABLE([flac], [AC_HELP_STRING([--enable-flac],
    [enable flac decoder plugin [default=yes]])],
    [enable_flac=$enableval], [enable_flac='yes'])

AC_ARG_WITH(flac-dir, AC_HELP_STRING([--with-flac-dir=path],
        [include path of FLAC/file_decoder.h (flac <= 1.1.2) or FLAC/stream_decoder.h (flac >= 1.1.3)]),
        [
            if test "x$withval" != "x"; then
              FLAC_H_DIR="$withval/"
              CXXFLAGS="$CXXFLAGS -I$withval"
            fi
        ],
        [FLAC_H_DIR=""])
  AC_SUBST(FLAC_H_DIR)

if test "x$enable_flac" = "xyes" ; then

	# flac <= 1.1.2
  AC_CHECK_HEADERS(${FLAC_H_DIR}FLAC/file_decoder.h, have_flac=yes, have_flac=no)
  if test "x$have_flac" = "xyes"; then      
    FLAC_LIBS="-lFLAC"
		FLAC_CFLAGS="-DHAVE_FLAC_FILEDECODER"
  fi

	# flac >= 1.1.3
  if test "x$have_flac" = "xno"; then      
  AC_CHECK_HEADERS(${FLAC_H_DIR}FLAC/stream_decoder.h, have_flac=yes, have_flac=no)
    if test "x$have_flac" = "xyes"; then      
      FLAC_LIBS="-lFLAC"
    fi
  fi
fi



# libmad
have_mad=no
AC_ARG_ENABLE([mad], [AC_HELP_STRING([--enable-mad],
    [enable mad support [default=no]])],
    [enable_mad=$enableval], [enable_mad='no'])
if test "x$enable_mad" = "xyes" ; then
	AH_TEMPLATE([HAVE_MAD], [])
	PKG_CHECK_MODULES(MAD, mad >= 0.15, have_mad=yes, have_mad=no)    
	if test "x$have_mad" = "xyes"; then      
		have_decoder=yes
		AC_DEFINE([HAVE_MAD], [1], [])
	fi
fi



# faad
have_faad=no
have_mp4ff=no
AC_ARG_ENABLE([faad], [AC_HELP_STRING([--enable-faad],
    [enable faad support [default=no]])],
    [enable_faad=$enableval], [enable_faad='no'])

AC_ARG_WITH(faad-dir, AC_HELP_STRING([--with-faad-dir=path],
	      [include path of faad.h]),
	      [if test "x$withval" != "x"; then
	         FAAD_H_DIR="$withval/"
	         CXXFLAGS="$CXXFLAGS -I$withval"
	       fi], [FAAD_H_DIR=""])

if test "x$enable_faad" = "xyes" ; then

	AH_TEMPLATE([HAVE_FAAD], [])
	AH_TEMPLATE([HAVE_MP4FF_H], [])

	AC_CHECK_HEADERS(${FAAD_H_DIR}faad.h, have_faad=yes, have_faad=no)
	if test "x$have_faad" = "xyes"; then
		AC_RUN_IFELSE(
		  [AC_LANG_PROGRAM(
		    [[#include <faad.h>]],
		    [[(void) faacDecOpen]])],
		  [have_decoder=yes
		   AC_DEFINE([HAVE_FAAD], [1], [])],
		  [have_faad=no])

		AC_SUBST(FAAD_H_DIR)

		AC_CHECK_HEADERS(${FAAD_H_DIR}mp4ff.h, have_mp4ff=yes, have_mp4ff=no)
		if test "x$have_mp4ff" = "xyes"; then
		  AC_DEFINE([HAVE_MP4FF_H], [1], [])
		else
		  echo "mp4ff.h not found. AAC decoder is build without MP4 support"
		fi
	fi

fi




# check for opus
have_opus=no
AC_ARG_ENABLE([opus], [AC_HELP_STRING([--enable-opus],
    [enable opus encoder/decoder plugin [default=yes]])],
    [enable_opus=$enableval], [enable_opus='yes'])
    
if test "x$enable_opus" = "xyes" ; then
	AH_TEMPLATE([HAVE_OPUS], [])
	PKG_CHECK_MODULES(OPUS, opus >= 0.9.14, have_opus=yes, have_opus=no)
	if test "x$have_opus" = "xyes"; then
		AC_DEFINE([HAVE_OPUS], [1], [])
	fi
fi	




# audio encoder

AM_CONDITIONAL(LAME_ENCODER_PLUGIN, test "x$have_lame" = "xyes")
#AC_SUBST(LAME_CFLAGS)
AC_SUBST(LAME_LIBS)

AM_CONDITIONAL(TWOLAME_ENCODER_PLUGIN, test "x$have_twolame" = "xyes")
AC_SUBST(TWOLAME_CFLAGS)
AC_SUBST(TWOLAME_LIBS)


# audio decoder

AM_CONDITIONAL(VORBIS_DECODER_PLUGIN, test "x$have_vorbis" = "xyes")
AC_SUBST(VORBISFILE_CFLAGS)
AC_SUBST(VORBISFILE_LIBS)

AM_CONDITIONAL(MUSEPACK_DECODER_PLUGIN, test "x$have_mpc" = "xyes")
AC_SUBST(MUSEPACK_CFLAGS)
AC_SUBST(MUSEPACK_LIBS)

AM_CONDITIONAL(FLAC_DECODER_PLUGIN, test "x$have_flac" = "xyes")
AC_SUBST(FLAC_CFLAGS)
AC_SUBST(FLAC_LIBS)

AM_CONDITIONAL(FAAD_DECODER_PLUGIN, test "x$have_faad" = "xyes")
AC_SUBST(FAAD_CFLAGS)
AC_SUBST(FAAD_LIBS)

AM_CONDITIONAL(MAD_DECODER_PLUGIN, test "x$have_mad" = "xyes")
AC_SUBST(MAD_CFLAGS)
AC_SUBST(MAD_LIBS)

AM_CONDITIONAL(OPUS_DECODER_PLUGIN, test "x$have_opus" = "xyes")
AC_SUBST(OPUS_CFLAGS)
AC_SUBST(OPUS_LIBS)


#if test "x$transcoding" = "xno"; then
#  ENABLE_TRANSCODING="false"
#  CXXFLAGS="$CXXFLAGS -DDISABLE_TRANSCODING"
#  AC_MSG_NOTICE("Transcoding disabled");     
#fi
#AM_CONDITIONAL(DISABLE_TRANSCODING, test x$ENABLE_TRANSCODING = xfalse)




SIMAGE_CFLAGS=""
SIMAGE_LIBS=""
have_simage=no
if test -z "$SIMAGE_CONFIG"; then
  AC_PATH_PROG(SIMAGE_CONFIG, simage-config, no, $prefix/bin/)
fi

if test "$SIMAGE_CONFIG" != "no" ; then   
  AC_MSG_CHECKING(SIMAGE_CFLAGS)
  SIMAGE_CFLAGS=`$SIMAGE_CONFIG --cflags`
  AC_MSG_RESULT($SIMAGE_CFLAGS)
  
	AC_MSG_CHECKING(SIMAGE_LIBS)
	SIMAGE_LIBS=`$SIMAGE_CONFIG --libs`
  AC_MSG_RESULT($SIMAGE_LIBS)

  have_simage=yes
fi
AM_CONDITIONAL(SIMAGE_METADATA_PLUGIN, test "x$have_simage" = "xyes")
AC_SUBST(SIMAGE_CFLAGS)
AC_SUBST(SIMAGE_LIBS)


# mysql
MYSQL_CFLAGS=""
MYSQL_LIBS=""
have_mysql=no

#AC_ARG_ENABLE([mysql], [AC_HELP_STRING([--enable-mysql],
#    [enable mysql database plugin [default=no]])],
#    [enable_mysql=$enableval], [enable_mysql='no'])
    
#if test "x$enable_mysql" = "xyes" ; then

#    if test -z "$MYSQL_CONFIG"; then
#      AC_PATH_PROG(MYSQL_CONFIG, mysql_config, no, $prefix/bin/)
#    fi

#    if test "$MYSQL_CONFIG" != "no" ; then   
#      AC_MSG_CHECKING(MYSQL_CFLAGS)
#      MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
#      AC_MSG_RESULT($MYSQL_CFLAGS)
      
#      AC_MSG_CHECKING(MYSQL_LIBS)
#      MYSQL_LIBS=`$MYSQL_CONFIG --libs`
#      AC_MSG_RESULT($MYSQL_LIBS)
      
#      have_mysql=yes
#    fi
#fi
#AM_CONDITIONAL(MYSQL_DATABASE_PLUGIN, test "x$have_mysql" = "xyes")
#AC_SUBST(MYSQL_CFLAGS)
#AC_SUBST(MYSQL_LIBS)






dnl Checks for Additional stuffs.

# HTTP Port
AC_ARG_WITH([http-port],
    [AC_HELP_STRING([--with-http-port=PORT],
                    [set the default HTTP port])],
    [with_http_port=$withval], [with_http_port=''])
if test "x$with_http_port" != 'x'; then
	CXXFLAGS="$CXXFLAGS -DDEFAULT_HTTP_PORT=$withval"
fi


# inotify (linux only)
have_inotify=no
AH_TEMPLATE([HAVE_INOTIFY], [inotify support])
AC_ARG_ENABLE([inotify], [AC_HELP_STRING([--enable-inotify],
    [enable inotify support (Linux only) [default=yes]])],
    [enable_inotify=$enableval], [enable_inotify='yes'])
if test "x$enable_inotify" = "xyes" ; then	
	AC_CHECK_HEADERS([sys/inotify.h], 
			have_inotify=yes
			AC_DEFINE([HAVE_INOTIFY], [1], []), 
			have_inotify=no)
fi
AM_CONDITIONAL([HAVE_INOTIFY], [test "x$have_inotify" = "xyes"])



# dbus (to detect device addition/removal)
have_dbus=no
AH_TEMPLATE([HAVE_DBUS], [dbus support (to detect device addition/removal) (not yet working)])
AC_ARG_ENABLE([dbus], [AC_HELP_STRING([--enable-dbus],
    [enable dbus [default=no]])],
    [enable_dbus=$enableval], [enable_dbus='no'])
if test "x$enable_dbus" = "xyes" ; then
  PKG_CHECK_MODULES(DBUS, dbus-1 >= 1.2, 
    have_dbus=yes
    AC_DEFINE([HAVE_DBUS], [1], []), 
    have_dbus=no)
fi
AM_CONDITIONAL(HAVE_DBUS, test "x$have_dbus" = "xyes")
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)
FUPPES_CFLAGS="$FUPPES_CFLAGS $DBUS_CFLAGS"
FUPPES_LIBS="$FUPPES_LIBS $DBUS_LIBS"



# build tests
AC_ARG_ENABLE([tests], [AC_HELP_STRING([--enable-tests],
    [enable tests [default=no]])],
    [enable_tests=$enableval], [enable_tests='no'])
AM_CONDITIONAL([BUILD_TESTS], [test "x$enable_tests" = "xyes"])

if test "x$enable_tests" = "xyes"; then
if test "x$have_curl" = "xno"; then
    AC_MSG_ERROR("you need curl to build the tests")
fi
fi



AC_ARG_ENABLE([tests-only], [AC_HELP_STRING([--enable-tests-only],
    [enable tests_only [default=no]])],
    [enable_tests_only=$enableval], [enable_tests_only='no'])
if test "x$enable_tests" = "xno" ; then
    AM_CONDITIONAL([BUILD_TESTS], [test "x$enable_tests_only" = "xyes"])
fi
AM_CONDITIONAL([BUILD_TESTS_ONLY], [test "x$enable_tests_only" = "xyes"])


AC_SUBST(FUPPES_LIBS)
AC_SUBST(FUPPES_CFLAGS)


AC_CONFIG_FILES([postinstall.sh:postinstall.sh.in])

AC_OUTPUT([
Makefile
m4/Makefile

src/lib/Configuration/Makefile
src/lib/ContentDatabase/Makefile
src/lib/Database/Makefile
src/lib/Dlna/Makefile
src/lib/Fam/Makefile
src/lib/Http/Makefile
src/lib/Log/Makefile
src/lib/Plugin/Makefile
src/lib/Presentation/Makefile
src/lib/Security/Makefile
src/lib/Thread/Makefile
src/lib/Transcoding/Makefile
src/lib/VirtualContainer/Makefile

src/Makefile
src/plugins/Makefile
src/windows/Makefile

include/Makefile
resources/Makefile
config/Makefile
test/Makefile
])

dnl summary

echo ""
echo "CONFIGURATION SUMMARY"
echo ""


echo "  audio transcoding plugins"  
echo "    encoder:"
if test "x$have_lame" = "xyes"; then
  echo "      lame       : yes"
else
  echo "      lame       : no"
fi

if test "x$have_twolame" = "xyes"; then
  echo "      twolame    : yes"
else  
  echo "      twolame    : no"
fi

echo "      pcm/wav    : yes"
  
echo "    decoder:"
if test "x$have_vorbis" = "xyes"; then
  echo "      vorbis     : yes (libvorbisfile)"
else
  if test "x$have_tremor" = "xyes"; then
    echo "      vorbis     : yes (tremor)"
  else
    echo "      vorbis     : no"      
  fi
fi

if test "x$have_mpc" = "xyes"; then
  echo "      mpc        : yes"
else
  echo "      mpc        : no"      
fi     

if test "x$have_flac" = "xyes"; then
  echo "      flac       : yes"
else
  echo "      flac       : no"      
fi 

if test "x$have_faad" = "xyes"; then
  if test "x$have_mp4ff" = "xyes"; then
    echo "      faad       : yes (aac/mp4/m4a)"
  else
    echo "      faad       : yes (aac/NO mp4/m4a)"
  fi
else
  echo "      faad       : no (aac/mp4/m4a)"
fi 

if test "x$have_mad" = "xyes"; then
  echo "      mad        : yes (mpeg Layer I, II & III)"
else
  echo "      mad        : no (mpeg Layer I, II & III)"      
fi 

echo ""
#end audio transcoding

#echo "  video transcoding plugins"
#if test "x$have_transcoder_ffmpeg" = "xyes"; then
#  echo "    ffmpeg             : enabled"
#else
#  echo "    ffmpeg             : disabled"
#fi
#echo ""


echo "  image conversion/rescaling plugins"
if test "x$have_magickwand" = "xyes"; then
  echo "    ImageMagick        : enabled  (Wand C-API)"
else
  echo "    ImageMagick        : disabled (Wand C-API)"
fi
echo ""
#end image conversion/rescaling


echo "  audio metadata extraction plugins"
# taglib
if test "x$have_taglib" = "xyes"; then
  echo "    taglib             : enabled  (mp3, ogg, flac & mpc)"
else
  echo "    taglib             : disabled (mp3, ogg, flac & mpc)"
fi

# mp4v2
if test "x$have_mp4v2" = "xyes"; then
  echo "    mpeg4ip/mp4v2      : enabled  (mp4/m4a)"
else
  echo "    mpeg4ip/mp4v2      : disabled (mp4/m4a)"
fi
echo ""


echo "  image metadata extraction plugins"
# Exiv2
if test "x$have_exiv2" = "xyes"; then
  echo "    Exiv2              : enabled"
else
  echo "    Exiv2              : disabled"
fi

# ImageMagick
if test "x$have_magickwand" = "xyes"; then
  echo "    ImageMagick        : enabled  (Wand C-API)"
else
  echo "    ImageMagick        : disabled (Wand C-API)"
fi

# simage
if test "x$have_simage" = "xyes"; then
  echo "    simage             : enabled  (jpeg, png, gif, tiff, rgb, pic, tga, eps)"
else
  echo "    simage             : disabled (jpeg, png, gif, tiff, rgb, pic, tga, eps)"
fi
echo ""


echo "  video metadata extraction plugins"
# libavformat
if test "x$have_libavformat" = "xyes"; then
  echo "    libavformat        : enabled"
else
  echo "    libavformat        : disabled"
fi

# ffmpegthumbnailer
if test "x$have_ffmpegthumbnailer" = "xyes"; then
  echo "    ffmpegthumbnailer  : enabled"
else
  echo "    ffmpegthumbnailer  : disabled"
fi
echo ""

#end metadata


echo "  database plugins"
echo "    sqlite             : enabled"
# mysql
#if test "x$have_mysql" = "xyes"; then
#  echo "    mysql              : enabled"
#else
#  echo "    mysql              : disabled"
#fi


echo "  miscellaneous"

# iconv
if test "x$am_func_iconv" = "xyes"; then
  echo "    iconv              : enabled (charset conversion)"
else
  echo "    iconv              : disabled (charset conversion)"
fi

# uuid
if test x$is_windows = xfalse; then
if test "x$have_uuid" = "xyes"; then
  echo "    uuid               : enabled"
else
  echo "    uuid               : disabled"
fi
fi

# file alteration monitor
if test "x$have_inotify" = "xyes"; then
  echo "    fam                : inotify"
else  
  if test x$is_windows = xtrue; then
    echo "    fam                : windows"
  else 
    echo "    fam                : n/a"
  fi  
fi

# eventlib
if test "x$have_ev" = "xyes"; then
  echo "    eventlib           : libev"  
else
if test "x$have_event" = "xyes"; then
  echo "    eventlib           : libevent"  
fi
fi

echo ""
echo "  paths"

#echo "    sysconfdir:         $sysconfdir/fuppes"
echo "    localstatedir:      $localstatedir/fuppes"


echo ""
echo "Thanks for using fuppes"
echo ""

