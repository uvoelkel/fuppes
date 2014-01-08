## Process this file with automake to produce Makefile.in
## $Id: $

if !BUILD_TESTS_ONLY

include $(top_srcdir)/PreRules.am

if WIN32

bin_PROGRAMS = fuppes-win

fuppes_win_SOURCES = \
	Common.cpp \
	Common.h \
	WinMainForm.cpp \
	WinMainForm.h \
	WinMain.cpp \
	fuppes.rc

fuppes_win_LDFLAGS = \
	-mwindows

fuppes_win_LDADD = \
	./../libfuppes.la \
	fuppes.res



if WINDOWS_SERVICE

bin_PROGRAMS += fuppes-svc

fuppes_svc_SOURCES = \
	WinService.cpp \
	fuppes.rc

fuppes_svc_LDFLAGS = \
	-mwindows
	-advapi32

fuppes_svc_LDADD = \
	./../libfuppes.la \
	fuppes.res

endif # WINDOWS_SERVICE


fuppes.res: fuppes.rc
	$(WINDRES) -i fuppes.rc --input-format=rc -o fuppes.res -O coff

BUILT_SOURCES = \
	fuppes.res

CLEANFILES = \
	fuppes.res

endif # WIN32

endif # tests only
