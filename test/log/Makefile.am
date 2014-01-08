noinst_PROGRAMS += log-test

log_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

log_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

log_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

log_test_SOURCES = \
  log/log-test.cpp
