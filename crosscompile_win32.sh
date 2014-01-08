#!/bin/bash

#
# cross compile FUPPES for Windows using mingw
# Copyright (C) 2009-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
#
# tested on debian using gcc-mingw32-4.4 and  and mingw32-runtime-3.18-1
# furthermore you need wine and wine-bin in order to
# execute test programms build by configure scripts
# autotools, wget and svn
#


# in order to build ffmpeg you need at least mingw-runtime-3.15 which is not available on debian
# so you need to build it on your own
# here is a good starting point:
# http://www.plutinosoft.com/ffmpeg-cross-compiling-for-windows-on-linux

# or you can download a prebuild package from
# http://fuppes.ulrich-voelkel.de/mingw32-runtime_3.18-1_all.deb


# debian settings

# the build host
#HOST="i586-mingw32msvc"
HOST="i686-w64-mingw32"
#HOST="amd64-mingw32msvc"

# target directory
PREFIX=`pwd`"/win32"
#PREFIX=`pwd`"/win64"

# make command (e.g. "make -j 2")
MAKE="make -j 2"
MAKE_INSTALL="make install"



# you should not need to change anything below

# exports
export CFLAGS="-I$PREFIX/include $CFLAGS"
export CPPFLAGS="-I$PREFIX/include $CPPFLAGS"
export LDFLAGS="-L$PREFIX/lib"
export PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig"
export WINEDLLPATH="$PREFIX/bin"

FUPPES_DIR=`pwd`


# create target directory
if ! test -d $PREFIX; then
  mkdir $PREFIX
fi

# create source directory
if ! test -d $PREFIX"/src"; then
  mkdir $PREFIX"/src"
fi
cd $PREFIX"/src"/


# $1 = paket name, $2 = file ext, $3 = url
function loadpkt {

  if ! test -d ../downloads; then
    mkdir ../downloads
  fi

  SRC_PKT=$1$2
  if ! test -e ../downloads/$SRC_PKT; then
    wget --directory-prefix=../downloads/ $3$SRC_PKT
  fi

  case $2 in
    ".tar.gz")
      tar -xvzf ../downloads/$1$2
    ;;
    ".tar.bz2")
      tar -xvjf ../downloads/$1$2
    ;;
    ".zip")
      unzip -o ../downloads/$1$2
    ;;
  esac

  cd $1
}


BUILD_FUPPES_ONLY="no"  # if set to "yes" compilation of all dependecies will be skipped

# select optional dependencies to build
BUILD_TAGLIB="yes"
BUILD_LAME="yes"
BUILD_OGGVORBIS="yes"
BUILD_FLAC="yes"
BUILD_MUSEPACK="yes"
BUILD_MAD="no"

BUILD_EXIV2="yes"
BUILD_JPEG="yes"
BUILD_PNG="yes"
BUILD_SIMAGE="no"
BUILD_IMAGEMAGICK="yes"

BUILD_FFMPEG="yes"
BUILD_FFMPEGTHUMBNAILER="yes"

BUILD_LIBCURL="yes"

BUILD_MYSQLCLIENT="no"

# strip executables
DO_STRIP="yes"



if test "$BUILD_FUPPES_ONLY" == "no"; then

    # libev 4.15 
    echo "building libev"
    loadpkt "libev-4.15" ".tar.gz" \
            "http://dist.schmorp.de/libev/Attic/"

    LDFLAGS="$LDFLAGS -lws2_32" \
    ./configure --host=$HOST --prefix=$PREFIX
    $MAKE libev_la_LDFLAGS="-version-info 4:0:0 -no-undefined"
    $MAKE_INSTALL
    cd ..


    #echo "building libevent"
    #loadpkt "libevent-2.0.20-stable" ".tar.gz" \
    #        "https://github.com/downloads/libevent/libevent/"
    #./configure --host=$HOST --prefix=$PREFIX
    #$MAKE
    #$MAKE_INSTALL
    #cd ..



    # zlib 1.2.7 (for taglib, exiv2, imagemagick & co.) 
    echo "building zlib"

    loadpkt "zlib-1.2.7" ".tar.gz" \
            "http://prdownloads.sourceforge.net/libpng/"

    echo "AC_INIT([zlib], [1.2.7], [fuppes@ulrich-voelkel.de])
    AM_INIT_AUTOMAKE([foreign])

    AC_PROG_CC
    AC_LIBTOOL_WIN32_DLL
    AC_PROG_LIBTOOL
    AC_PROG_INSTALL
    AM_PROG_INSTALL_STRIP

    AC_OUTPUT([Makefile])" \
    > configure.ac

    echo "include_HEADERS = zlib.h zconf.h  

    lib_LTLIBRARIES = libz.la

    libz_la_SOURCES = \
      adler32.h adler32.c \
      compress.h compress.c \
      crc32.h crc32.c \
      gzguts.h gzclose.c gzlib.c gzread.c gzwrite.c \
      uncompr.h uncompr.c \
      deflate.h deflate.c \
      trees.h trees.c \
      zutil.h zutil.c \
      inflate.h inflate.c \
      infback.h infback.c \
      inftrees.h inftrees.c \
      inffast.h inffast.c

    libz_la_LDFLAGS = -no-undefined" \
    > Makefile.am

    autoreconf -vfi
    ./configure --host=$HOST --prefix=$PREFIX
    $MAKE
    $MAKE_INSTALL
    cd ..




    # libiconv 1.14 (recommended)
    echo "building libiconv"

    loadpkt "libiconv-1.14" ".tar.gz" \
            "http://ftp.gnu.org/pub/gnu/libiconv/"
    ./configure --host=$HOST --prefix=$PREFIX
    $MAKE
    $MAKE_INSTALL
    cd ..



    # PCRE 8.32 (required)
    echo "building pcre"
    loadpkt "pcre-8.32" ".tar.gz" \
            "ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/"
    ./configure --host=$HOST --prefix=$PREFIX \
    --enable-utf8 --enable-newline-is-anycrlf --disable-cpp
    $MAKE
    $MAKE_INSTALL
    cd ..


    # SQLite 3.7.15.2 (required)
    echo "building sqlite"
    #loadpkt "sqlite-amalgamation-3.7.3" ".tar.gz" \
    #        "http://www.sqlite.org/"
    #cd sqlite-3.7.3
    loadpkt "sqlite-autoconf-3071502" ".tar.gz" \
            "http://www.sqlite.org/"
    ./configure --host=$HOST --prefix=$PREFIX \
    --enable-threadsafe
    $MAKE
    $MAKE_INSTALL
    cd ..


    # libxml2 2.7.8 (required)
    echo "building libxml2"
    #loadpkt "libxml2-2.6.32" ".tar.gz" \
    #        "ftp://xmlsoft.org/libxml2/old/"

    #./configure --host=$HOST --prefix=$PREFIX \
    #--enable-ipv6=no --with-debug=no --with-http=no \
    #--with-ftp=no --with-docbook=no --with-schemas=yes --with-schematron=no \
    #--with-catalog=no --with-html=no --with-legacy=no --with-pattern=no \
    #--with-push=yes --with-python=no --with-readline=no --with-regexps=no \
    #--with-sax1=no --with-xinclude=no --with-xpath=no --with-xptr=no --with-modules=no \
    #--with-valid=no --with-reader=yes --with-writer=yes --with-tree=yes

    loadpkt "libxml2-2.7.8" ".tar.gz" \
            "ftp://xmlsoft.org/libxml2/"
            
    ./configure --host=$HOST --prefix=$PREFIX \
    --with-catalog=no --with-debug=no --with-docbook=no \
    --with-ftp=no --with-html=no --with-http=no --with-legacy=no \
    --with-output=no --with-pattern=no --with-push=no --with-python=no \
    --with-regexps=no --with-sax1=no --with-schemas=no --with-schematron=no \
    --with-valid=no --with-xinclude=no --with-xpath=no --with-xptr=no \
    --with-modules=no 

    $MAKE
    $MAKE_INSTALL
    cd ..



    #
    # OPTIONAL
    #

    # taglib
    if test "$BUILD_TAGLIB" == "yes"; then

      echo "building taglib"

      loadpkt "taglib-1.6.3" ".tar.gz" \
              "http://developer.kde.org/~wheeler/files/src/"

      #loadpkt "taglib-1.8" ".tar.gz" \
      #        "https://github.com/downloads/taglib/taglib/"

      CXXFLAGS="$CXXFLAGS -DMAKE_TAGLIB_LIB" \
      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..
      
    else
      echo "skipped taglib"
    fi


    # LAME
    if test "$BUILD_LAME" == "yes"; then

      echo "building lame"

      loadpkt "lame-3.99.5" ".tar.gz" \
              "http://downloads.sourceforge.net/lame/"

      # we need to reconfigure the sources because of this bug (with at least 3.99.5 no longer necessary)
      # http://git.savannah.gnu.org/gitweb/?p=autoconf.git;a=commitdiff;h=78dc34
      # sed -i -e 's/AM_PATH_GTK(1.2.0, HAVE_GTK="yes", HAVE_GTK="no")/HAVE_GTK="no"\nGTK_CFLAGS=""\nAC_SUBST(GTK_CFLAGS)/' configure.in
      # autoreconf -vfi
              
      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped LAME"
    fi


    # ogg/vorbis
    if test "$BUILD_OGGVORBIS" == "yes"; then

    # ogg
    echo "building ogg"
    loadpkt "libogg-1.3.0" ".tar.gz" \
            "http://downloads.xiph.org/releases/ogg/"
    # autoreconf -vfi # seems to be obsolete as of version 1.2.1
    ./configure --host=$HOST --prefix=$PREFIX
    $MAKE
    $MAKE_INSTALL
    cd ..

    # vorbis
    echo "building vorbis"
    loadpkt "libvorbis-1.3.3" ".tar.gz" \
            "http://downloads.xiph.org/releases/vorbis/"
    ./configure --host=$HOST --prefix=$PREFIX
    $MAKE
    $MAKE_INSTALL
    cd ..

    else
      echo "skipped ogg/vorbis"
    fi


    # FLAC
    if test "$BUILD_FLAC" == "yes"; then

    #http://sourceforge.net/tracker/index.php?func=detail&aid=2000973&group_id=13478&atid=313478

    echo "building flac"

    loadpkt "flac-1.2.1" ".tar.gz" \
            "http://downloads.sourceforge.net/flac/"



    sed -i -e 's/!defined __MINGW32__ &&/ /' include/share/alloc.h

    sed -i -e 's/AM_MAINTAINER_MODE/AM_MAINTAINER_MODE\nAC_LIBTOOL_WIN32_DLL/' configure.in
    sed -i -e 's/AM_PATH_XMMS/echo "xmms"\n# AM_PATH_XMMS/' configure.in

    sed -i -e 's/@OGG_LIBS@/@OGG_LIBS@ -lws2_32/' src/libFLAC/Makefile.am 
    sed -i -e 's/libFLAC_la_LDFLAGS =/libFLAC_la_LDFLAGS = -no-undefined/' src/libFLAC/Makefile.am 


    sed -i -e 's/AM_INIT_AUTOMAKE(flac, 1.2.1)/AM_INIT_AUTOMAKE(flac, 1.2.1)\n\nAC_CONFIG_MACRO_DIR([m4])/' configure.in
    sed -i -e 's/DISTCLEANFILES = libtool-disable-static/DISTCLEANFILES = libtool-disable-static\n\nACLOCAL_AMFLAGS = -I m4/' Makefile.am


    autoreconf -vfi
    ./configure --host=$HOST --prefix=$PREFIX \
    --disable-xmms-plugin --disable-cpplibs
    $MAKE
    $MAKE_INSTALL
    cd ..

    else
      echo "skipped FLAC"
    fi


    # musepack
    if test "$BUILD_MUSEPACK" == "yes"; then

    echo "building musepack"

    # SV7
    #loadpkt "libmpcdec-1.2.6" ".tar.bz2" \
    #        "http://files.musepack.net/source/"

    # SV8
    loadpkt "musepack_src_r435" ".tar.gz" \
            "http://files.musepack.net/source/"

    # fixes for SV7
    #sed -i -e 's/AC_MSG_ERROR(\[working memcmp is not available.\])/echo "no memcmp"/' configure.ac
    #sed -i -e 's/AM_PROG_LIBTOOL/AM_PROG_LIBTOOL\nAC_PROG_CXX/' configure.ac

    # fixes for SV8 (some tools don't compile but as we don't need them anyway we just skip them)
    sed -i -e 's/\\/\n/' Makefile.am
    sed -i -e 's/\tmpcchap/#/' Makefile.am


    autoreconf -vfi
    #LDFLAGS="-no-undefined $LDFLAGS" \
    ./configure --host=$HOST --prefix=$PREFIX
    $MAKE
    $MAKE_INSTALL

    # SV7 only
    #cp include/mpcdec/config_win32.h $PREFIX/include/mpcdec/

    cd ..

    else
      echo "skipped MusePack"
    fi


    # MAD
    if test "$BUILD_MAD" == "yes"; then

    #http://downloads.sourceforge.net/mad/libid3tag-0.15.1b.tar.gz

      echo "building mad"

      loadpkt "libmad-0.15.1b" ".tar.gz" \
              "http://downloads.sourceforge.net/mad/"
      sed -i -e 's/libmad_la_LDFLAGS =/libmad_la_LDFLAGS = -no-undefined/' Makefile.am
      touch NEWS AUTHORS ChangeLog
      autoreconf -vfi
      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    echo "# Package Information for pkg-config

    prefix=$PREFIX
    exec_prefix=\${prefix}
    libdir=\${exec_prefix}/lib
    includedir=\${prefix}/include

    Name: mad
    Description: MPEG Audio Decoder
    Requires:
    Version: 0.15.1b
    Libs: -L\${libdir} -lmad
    Cflags: -I\${includedir}" \
    > $PREFIX/lib/pkgconfig/mad.pc

    else
      echo "skipped libmad"
    fi


    # exiv2
    if test "$BUILD_EXIV2" == "yes"; then

      echo "building exiv2"
      loadpkt "exiv2-0.23" ".tar.gz" \
              "http://www.exiv2.org/"

      PARAMS="--disable-xmp" # --disable-visibility
      if test "$HAVE_ZLIB" != "yes"; then
        PARAMS="$PARAMS --without-zlib"
      #else
      #  PARAMS="$PARAMS --with-zlib=$PREFIX"
      fi
      
      ./configure --host=$HOST --prefix=$PREFIX $PARAMS

      # fixes for 0.18.2
      #sed -i -e 's/LDFLAGS = /LDFLAGS = -no-undefined /' config/config.mk
      #sed -i -e 's/-lm//' config/config.mk
      $MAKE
      mv src/.libs/exiv2 src/.libs/exiv2.exe
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped exiv2"
    fi




    #libjpeg (for ImageMagick, simage and ffmpegthumbnailer)
    if test "$BUILD_JPEG" == "yes"; then
      echo "building jpeg"  
      loadpkt "jpegsrc.v8d" ".tar.gz" \
              "http://www.ijg.org/files/"
      cd jpeg-8d
      #sed -i -e 's/typedef int boolean/typedef char boolean/' jmorecfg.h
      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped libjpeg"
    fi

    #libpng (for ImageMagick, simage and ffmpegthumbnailer)
    if test "$BUILD_PNG" == "yes"; then
      echo "building png"
      loadpkt "libpng-1.5.14" ".tar.bz2" \
              "http://prdownloads.sourceforge.net/libpng/"
      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped libpng"
    fi


    # todo libtiff & libungif

    #simage
    if test "$BUILD_SIMAGE" == "yes"; then

      echo "building simage"
      loadpkt "simage-1.7.0" ".tar.gz" \
              "http://ftp.coin3d.org/coin/src/all/"

      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped simage"
    fi


    # ImageMagick
    if test "$BUILD_IMAGEMAGICK" == "yes"; then

      echo "building ImageMagick"
      loadpkt "ImageMagick-6.7.3-10" ".tar.gz" \
              "ftp://ftp.imagemagick.org/pub/ImageMagick/legacy/"

      sed -i -e 's/"\*.la"/"\*.dll"/' magick/module.c
      sed -i -e 's/"%s.la"/"%s.dll"/' magick/module.c
      sed -i -e 's/GetEnvironmentValue("MAGICK_CODER_MODULE_PATH")/ConstantString(".\\\\magick-modules")/' magick/module.c
      sed -i -e 's/GetEnvironmentValue("MAGICK_CODER_FILTER_PATH")/ConstantString(".\\\\magick-filters")/' magick/module.c


      ./configure --host=$HOST --prefix=$PREFIX \
      --disable-deprecated --without-perl --with-modules \
      --without-x --without-gslib --with-magick-plus-plus=no \
      --with-quantum-depth=8 --enable-embeddable
      #--disable-installed 
      
      $MAKE CFLAGS="-DHAVE_BOOLEAN $CFLAGS"
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped ImageMagick"
    fi


    if test "$BUILD_FFMPEG" == "yes"; then

    echo "building ffmpeg"

    if ! test -d ffmpeg-svn; then
      svn co svn://svn.mplayerhq.hu/ffmpeg/trunk ffmpeg-svn
      cd ffmpeg-svn
    else
      cd ffmpeg-svn
      make clean
      svn update
    fi


    PARAMS=""
    # https://roundup.ffmpeg.org/issue2067
    # we ned to set U__STRICT_ANSI__ for at least gcc 4.4
    PARAMS="$PARAMS --extra-cflags=-U__STRICT_ANSI__"
    # and D_MT for gcc 4.2
    # PARAMS="$PARAMS --extra-cflags=-D_MT"
     
    ./configure --prefix=$PREFIX \
    --enable-cross-compile --target-os=mingw32 --arch=x86 \
    --enable-memalign-hack --cross-prefix=$HOST- --enable-shared --enable-w32threads \
    --enable-gpl --disable-dxva2 \
    $PARAMS

    # --disable-ffmpeg --disable-ffplay --disable-ffserver
    # --disable-demuxer=dv1394 --disable-indevs 

    $MAKE
    $MAKE_INSTALL
    cd ..

    else
      echo "skipped ffmpeg"
    fi


    if test "$BUILD_FFMPEGTHUMBNAILER" == "yes"; then

      echo "building ffmpegthumbnailer"
      loadpkt "ffmpegthumbnailer-2.0.8" ".tar.gz" \
              "http://ffmpegthumbnailer.googlecode.com/files/"

      sed -i -e 's/-version-info 4:8:0/-no-undefined -version-info 4:8:0/' Makefile.am
      autoreconf -vfi
      
      #LDFLAGS="-Wl,--no-undefined $LDFLAGS" \
      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped ffmpegthumbnailer"
    fi




    if test "$BUILD_MYSQLCLIENT" == "yes"; then

      echo "building mysqlclient"
      # go to http://dev.mysql.com/downloads/connector/c/
      # download the latest mysql c connector source
      # and put it in the win32/downloads/ directory
      if ! test -e ../downloads/mysql-connector-c-6.0.2.tar.gz; then
        echo "please download mysql-connector-c-6.0.2.tar.gz"
        exit;
      fi

      tar -xvzf ../downloads/mysql-connector-c-6.0.2.tar.gz
      cd mysql-connector-c-6.0.2/

      #CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined"
      #-DCMAKE_FIND_ROOT_PATH="/usr/"$HOST \

      cmake -G "Unix Makefiles" \
      -DCMAKE_SYSTEM_NAME="Windows" \
      -DCMAKE_C_COMPILER=$HOST-gcc \
      -DCMAKE_CXX_COMPILER=$HOST-g++ \
      -DCMAKE_PREFIX_PATH=/usr/$HOST \
      -DCMAKE_INCLUDE_PATH=/usr/$HOST/include \
      -DCMAKE_LIBRARY_PATH=/usr/$HOST/lib \
      -DCMAKE_INSTALL_PREFIX=$PREFIX
      $MAKE VERBOSE=1
      $MAKE_INSTALL
      cd ..




    else
      echo "skipped mysqlclient"
    fi


    if test "$BUILD_LIBCURL" == "yes"; then

      echo "building curl"
      loadpkt "curl-7.28.1" ".tar.bz2" \
              "http://curl.haxx.se/download/"

      ./configure --host=$HOST --prefix=$PREFIX
      $MAKE
      $MAKE_INSTALL
      cd ..

    else
      echo "skipped libcurl"
    fi



fi


# FUPPES

#  -static-libstdc++

cd $FUPPES_DIR
autoreconf -vfi
LDFLAGS="$LDFLAGS -static-libgcc" \
./configure --host=$HOST --prefix=$PREFIX --localstatedir=$PREFIX/var/lib \
--enable-lame --enable-windows-service \
--disable-musepack --disable-silent-rules # --enable-transcoder-ffmpeg
$MAKE
$MAKE_INSTALL


# strip libraries and executables
if test "$DO_STRIP" == "yes"; then
  $HOST-strip -s $PREFIX/bin/*
  $HOST-strip -s $PREFIX/lib/bin/*
  touch $PREFIX/bin/*
  touch $PREFIX/lib/bin/*
  touch $PREFIX/share/fuppes/*
fi




if ! test -d $PREFIX"/build"; then
  mkdir $PREFIX"/build"
  mkdir $PREFIX"/build/data"
  mkdir $PREFIX"/build/devices"
  mkdir $PREFIX"/build/vfolders"
  if test "$HAVE_IMAGEMAGICK" == "yes"; then
    mkdir $PREFIX"/build/magick-modules"
    mkdir $PREFIX"/build/magick-filters"
  fi
fi


files=(
  bin/fuppes.exe
  bin/fuppes-svc.exe
  bin/fuppes-win.exe
  bin/libfuppes-0.dll

  bin/libz-0.dll
  bin/libxml2-2.dll
  bin/libpcre-0.dll
  bin/libiconv-2.dll
  
  bin/libsqlite3-0.dll
  lib/bin/libdatabase_sqlite3-0.dll
  
  bin/libtag-1.dll
  lib/bin/libmetadata_taglib-0.dll
  
  bin/avcore-0.dll
  bin/avcodec-52.dll
  bin/avformat-52.dll
  bin/avutil-50.dll
  lib/bin/libmetadata_libavformat-0.dll

  bin/swscale-0.dll


  bin/libjpeg-8.dll
  bin/libpng15-15.dll
  bin/libffmpegthumbnailer-4.dll
  lib/bin/libmetadata_ffmpegthumbnailer-0.dll

  bin/libexiv2-10.dll
  lib/bin/libmetadata_exiv2-0.dll

  bin/libMagickCore-5.dll
  bin/libMagickWand-5.dll
  lib/bin/libmetadata_magickwand-0.dll
  lib/bin/libtranscoder_magickwand-0.dll
  
  bin/libcurl-4.dll
);

#  lib/bin/libtranscoder_ffmpeg-0.dll

for i in ${files[@]}
do
  echo "copy file "$i
  cp $PREFIX/$i $PREFIX/build/
done

cp $PREFIX/share/fuppes/* $PREFIX/build/data/

# copy config files
cp config/fuppes.cfg.default $PREFIX/build/
cp config/devices/* $PREFIX/build/devices/
cp config/vfolders/* $PREFIX/build/vfolders/

echo "finished build in $PREFIX/build"

