noinst_PROGRAMS += thread-test

thread_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

thread_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

thread_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

thread_test_SOURCES = \
  thread/thread-test.cpp





noinst_PROGRAMS += dummy-process
dummy_process_LDADD = 
dummy_process_DEPENDENCIES = 
dummy_process_LDFLAGS = 
dummy_process_SOURCES = thread/dummy-process.cpp

noinst_PROGRAMS += process-test

process_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

process_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

process_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

process_test_SOURCES = \
  thread/process-test.cpp

