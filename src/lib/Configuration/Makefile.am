AM_CPPFLAGS = \
    -I$(top_srcdir)/include \
    ${LIBXML_CFLAGS} \
	-DFUPPES_DATADIR=\"$(datadir)/fuppes\" \
	-DFUPPES_PLUGINDIR=\"$(libdir)/fuppes\" \
	-DFUPPES_LOCALSTATEDIR=\"$(localstatedir)/fuppes\"

noinst_LTLIBRARIES = libfuppes_configuration.la

libfuppes_configuration_la_SOURCES = \
    ConfigFile.h \
    ConfigFile.cpp \
    BaseSettings.h \
    BaseSettings.cpp \
    SharedObjects.h \
    SharedObjects.cpp \
    GlobalSettings.h \
    GlobalSettings.cpp \
    DatabaseSettings.h \
    DatabaseSettings.cpp \
    NetworkSettings.h \
    NetworkSettings.cpp \
    ContentDatabase.h \
    ContentDatabase.cpp \
    VirtualContainerSettings.h \
    VirtualContainerSettings.cpp \
    DeviceIdentificationSettings.h \
    DeviceIdentificationSettings.cpp \
    FuppesConfig.h \
    FuppesConfig.cpp \
    PathFinder.h \
    PathFinder.cpp \
    Config.h \
    Config.cpp

