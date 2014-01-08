AM_CPPFLAGS = \
    -I$(top_srcdir)/include

noinst_LTLIBRARIES = libfuppes_fam.la

libfuppes_fam_la_SOURCES = \
	EventHandler.h \
	Monitor.h \
	Monitor.cpp \
	MonitorNull.h

if HAVE_INOTIFY
libfuppes_fam_la_SOURCES += \
	MonitorInotify.h \
	MonitorInotify.cpp \
	inotify-cxx-0.7.4/inotify-cxx.h \
  	inotify-cxx-0.7.4/inotify-cxx.cpp
endif
