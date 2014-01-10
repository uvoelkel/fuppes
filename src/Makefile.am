## Process this file with automake to produce Makefile.in
## $Id: $

if !BUILD_TESTS_ONLY

include $(top_srcdir)/PreRules.am

BUILT_SOURCES =
CLEANFILES=

AM_CPPFLAGS = \
    -I$(top_srcdir)/include \
    ${FUPPES_CFLAGS} \
    ${UUID_CFLAGS} \
    ${LIBXML_CFLAGS} \
    ${PCRE_CFLAGS} \
	-DFUPPES_DATADIR=\"$(datadir)/fuppes\" \
	-DFUPPES_PLUGINDIR=\"$(libdir)/fuppes\" \
	-DFUPPES_LOCALSTATEDIR=\"$(localstatedir)/lib/fuppes\"

lib_LTLIBRARIES = libfuppes.la

bin_PROGRAMS = fuppes

fuppes_SOURCES = console_main.cpp
fuppes_LDADD = ./libfuppes.la $(LIBADD_DL)
fuppes_DEPENDENCIES = ./libfuppes.la
fuppes_LDFLAGS = \
	$(FUPPES_LIBS) \
	${UUID_LIBS} \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
    ${MAD_LIBS} \
	${LIBICONV}


if WIN32

fuppes_LDADD += \
	fuppes.res

fuppes.res: windows/fuppes.rc
	$(WINDRES) -i windows/fuppes.rc --input-format=rc -o fuppes.res -O coff 

BUILT_SOURCES += \
	fuppes.res

CLEANFILES += \
	fuppes.res

endif


if BUILD_DAEMON
bin_PROGRAMS += fuppesd
fuppesd_SOURCES = daemon_main.cpp
fuppesd_LDADD = ./libfuppes.la $(LIBADD_DL)
fuppesd_DEPENDENCIES = ./libfuppes.la
fuppesd_LDFLAGS = \
	$(FUPPES_LIBS)\
	${UUID_LIBS}\
	${LIBXML_LIBS}\
	${PCRE_LIBS}\
	${MAD_LIBS}\
	${LIBICONV}

endif

libfuppes_la_SOURCES = lib/libmain.cpp

## service
#libfuppes_la_SOURCES += \
#    lib/Service/ServiceAware.h \
#    lib/Service/ServiceAware.cpp \
#    lib/Service/ServiceContainer.h

## fuppes http
libfuppes_la_SOURCES += \
    lib/Fuppes/FuppesHttpSession.h \
    lib/Fuppes/FuppesHttpSession.cpp \
    lib/Fuppes/HttpRequest.h \
    lib/Fuppes/HttpResponse.h \
    lib/Fuppes/RequestItem.h \
    lib/Fuppes/RequestItem.cpp \
    lib/Fuppes/Core.h \
    lib/Fuppes/Core.cpp

## fuppes common
libfuppes_la_SOURCES += \
    lib/Common/DateTime.h \
    lib/Common/DateTime.cpp    

## presentation
#libfuppes_la_SOURCES += \
#    lib/Presentation/PresentationHandler.h \
#    lib/Presentation/PresentationHandler.cpp \
#    lib/Presentation/PresentationPage.h \
#    lib/Presentation/PresentationPage.cpp

## upnp
libfuppes_la_SOURCES += \
    lib/UPnP/UPnPObjectTypes.h \
    lib/UPnP/UPnPErrorCodes.h \
    lib/UPnP/UPnPObject.h \
    lib/UPnP/UPnPObject.cpp

## device identification
libfuppes_la_SOURCES += \
    lib/DeviceIdentification/DeviceSettings.h \
    lib/DeviceIdentification/DeviceSettings.cpp \
    lib/DeviceIdentification/DeviceConfigFile.h \
    lib/DeviceIdentification/DeviceConfigFile.cpp \
    lib/DeviceIdentification/IdentificationMgr.h \
    lib/DeviceIdentification/IdentificationMgr.cpp \
    lib/DeviceIdentification/IdentificationMgrConfig.h

## configuration
#libfuppes_la_SOURCES += \
#    lib/Configuration/ConfigFile.h \
#    lib/Configuration/ConfigFile.cpp \
#    lib/Configuration/BaseSettings.h \
#    lib/Configuration/BaseSettings.cpp \
#    lib/Configuration/SharedObjects.h \
#    lib/Configuration/SharedObjects.cpp \
#    lib/Configuration/GlobalSettings.h \
#    lib/Configuration/GlobalSettings.cpp \
#    lib/Configuration/DatabaseSettings.h \
#    lib/Configuration/DatabaseSettings.cpp \
#    lib/Configuration/NetworkSettings.h \
#    lib/Configuration/NetworkSettings.cpp \
#    lib/Configuration/VirtualContainerSettings.h \
#    lib/Configuration/VirtualContainerSettings.cpp \
#    lib/Configuration/DeviceIdentificationSettings.h \
#    lib/Configuration/DeviceIdentificationSettings.cpp \
#    lib/Configuration/FuppesConfig.h \
#    lib/Configuration/FuppesConfig.cpp

## plugin
#libfuppes_la_SOURCES += \
#    lib/Plugins/MetadataPlugin.h \
#    lib/Plugins/MetadataPlugin.cpp \
#    lib/Plugins/DecoderPlugin.h \
#    lib/Plugins/DecoderPlugin.cpp \
#    lib/Plugins/EncoderPlugin.h \
#    lib/Plugins/EncoderPlugin.cpp \
#    lib/Plugins/TranscoderPlugin.h \
#    lib/Plugins/TranscoderPlugin.cpp \
#    lib/Plugins/ItemSource.h \
#    lib/Plugins/ItemSource.cpp

## dlna
#libfuppes_la_SOURCES += \
#	lib/DLNA/DLNA.h \
#    lib/DLNA/DLNA.cpp \
#    lib/DLNA/DlnaItem.h \
#    lib/DLNA/dlna_audio_profiles.h \
#    lib/DLNA/dlna_image_profiles.h \
#    lib/DLNA/dlna_video_profiles.h

# lib/ContentDirectory/VirtualContainerMgr.h
# lib/ContentDirectory/VirtualContainerMgr.cpp
# lib/ContentDirectory/PlaylistFactory.h
# lib/ContentDirectory/PlaylistFactory.cpp
# lib/ContentDirectory/PlaylistParser.h
# lib/ContentDirectory/PlaylistParser.cpp

libfuppes_la_SOURCES += \
    lib/SSDP/UDPSocket.h\
    lib/SSDP/SSDPCtrl.h\
    lib/SSDP/SSDPMessage.h\
    lib/SSDP/MSearchSession.h\
    lib/SSDP/NotifyMsgFactory.h\
    lib/OldHTTP/HTTPParser.h \
    lib/OldHTTP/HTTPParser.cpp \
    lib/OldHTTP/HTTPMessage.h \
    lib/OldHTTP/HTTPMessage.cpp \
    lib/OldHTTP/HTTPClient.h \
    lib/OldHTTP/HTTPClient.cpp \
    lib/UPnP/UPnPBase.h\
    lib/UPnP/UPnPBase.cpp\
    lib/UPnP/UPnPDevice.h\
    lib/UPnP/UPnPDevice.cpp\
    lib/UPnP/UPnPService.h\
    lib/UPnPActions/UPnPAction.h\
    lib/UPnPActions/UPnPAction.cpp\
    lib/UPnPActions/UPnPActionFactory.h\
    lib/UPnPActions/UPnPBrowse.h\
    lib/UPnPActions/UPnPSearch.h\
    lib/UPnPActions/UPnPActionFactory.cpp\
    lib/UPnPActions/UPnPBrowse.cpp\
    lib/UPnPActions/UPnPSearch.cpp\
    lib/MediaServer/MediaServer.h\
    lib/ContentDirectory/ContentDirectory.h\
    lib/ContentDirectory/ContentDirectory.cpp\
    lib/ContentDirectory/ContentDirectoryDescription.cpp\
    lib/ConnectionManager/ConnectionManager.h\
    lib/GENA/SubscriptionMgr.h\
    lib/GENA/EventNotification.h\
    lib/SharedLog.h\
    lib/Common/RegEx.h\
    lib/Common/Common.cpp\
    lib/Common/Common.h\
    lib/Common/Exception.cpp\
    lib/Common/Exception.h\
    lib/Common/UUID.cpp\
    lib/Common/UUID.h\
    lib/Common/Timer.cpp\
    lib/Common/Timer.h\
    lib/Common/XMLParser.cpp\
    lib/Common/XMLParser.h\
    lib/Common/File.cpp\
    lib/Common/File.h\
    lib/Common/Directory.cpp\
    lib/Common/Directory.h\
    lib/Common/Socket.cpp\
    lib/Common/Socket.h\
    lib/SSDP/UDPSocket.cpp\
    lib/SSDP/SSDPCtrl.cpp\
    lib/SSDP/SSDPMessage.cpp\
    lib/SSDP/MSearchSession.cpp\
    lib/SSDP/NotifyMsgFactory.cpp\
    lib/ControlInterface/ControlInterface.h \
    lib/ControlInterface/ControlInterface.cpp \
    lib/ControlInterface/SoapControl.h \
    lib/ControlInterface/SoapControl.cpp \
    lib/ControlInterface/SoapControlDescription.cpp \
    lib/ControlInterface/CliControl.h \
    lib/ControlInterface/CliControl.cpp \
    lib/MediaServer/MediaServer.cpp \
    lib/ContentDirectory/HotPlug.cpp\
    lib/ContentDirectory/HotPlug.h\
    lib/ConnectionManager/ConnectionManager.cpp\
    lib/ConnectionManager/ConnectionManagerDescription.cpp\
    lib/GENA/SubscriptionMgr.cpp\
    lib/GENA/EventNotification.cpp\
    lib/XMSMediaReceiverRegistrar/XMSMediaReceiverRegistrar.h\
    lib/XMSMediaReceiverRegistrar/XMSMediaReceiverRegistrar.cpp\
    lib/XMSMediaReceiverRegistrar/XMSMediaReceiverRegistrarDescription.cpp\
    lib/SharedLog.cpp

libfuppes_la_LIBADD = \
	$(LIBADD_DL) \
	lib/Configuration/libfuppes_configuration.la \
	lib/ContentDatabase/libfuppes_contentdatabase.la \
	lib/Database/libfuppes_database.la \
	lib/Dlna/libfuppes_dlna.la \
	lib/Fam/libfuppes_fam.la \
	lib/Http/libfuppes_http.la \
	lib/Log/libfuppes_log.la \
	lib/Plugin/libfuppes_plugin.la \
	lib/Presentation/libfuppes_presentation.la \
	lib/Security/libfuppes_security.la \
	lib/Thread/libfuppes_thread.la \
	lib/Transcoding/libfuppes_transcoding.la \
	lib/VirtualContainer/libfuppes_virtualcontainer.la

libfuppes_la_LDFLAGS = \
	-version-info 0:0:0 \
	$(FUPPES_LIBS)\
	${UUID_LIBS}\
	${LIBXML_LIBS}\
	${PCRE_LIBS}\
	${MAD_LIBS}\
	${LIBICONV}

# pixmapsdir = $(datadir)$(datadir)/fuppes
# dist_pixmaps_DATA = \
#  ../resources/style.css \
#  ../resources/fuppes-small.jpg \
#  ../resources/fuppes-small.png \
#  ../resources/header-gradient.png \
#  ../resources/header-gradient-small.png

endif # tests only
