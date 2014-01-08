noinst_PROGRAMS += file-test

file_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

file_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

file_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

file_test_SOURCES = \
	file/file-test.cpp
	