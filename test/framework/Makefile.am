noinst_LTLIBRARIES = libtestframework.la

AM_CPPFLAGS = \
    -I$(top_srcdir)/include \
	-I$(top_srcdir)/src/lib \
	-I./framework \
	${FUPPES_CFLAGS} \
	${LIBXML_CFLAGS} \
	${PCRE_CFLAGS} \
	${CURL_CFLAGS}

libtestframework_la_LIBADD = $(LIBADD_DL)
libtestframework_la_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${CURL_LIBS} \
	${LIBICONV}
	
libtestframework_la_SOURCES = \
    framework/BaseTest.h \
    framework/BaseTest.cpp


    
noinst_PROGRAMS = setup-test-env

setup_test_env_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

setup_test_env_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

setup_test_env_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

setup_test_env_SOURCES = \
	framework/ToneGenerator.h \
	framework/ToneGenerator.cpp \
	framework/setup-test-env.cpp
