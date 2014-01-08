noinst_PROGRAMS += large-file-test

large_file_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

large_file_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

large_file_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

large_file_test_SOURCES = \
	http-server/HttpSession.h \
	http-server/HttpFileSession.h \
	http-server/HttpCallbackSession.h \
	http-server/large-file-test.cpp



noinst_PROGRAMS += http-request-test

http_request_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

http_request_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

http_request_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

http_request_test_SOURCES = \
	http-server/HttpSession.h \
	http-server/HttpFileSession.h \
	http-server/HttpCallbackSession.h \
	http-server/http-request-test.cpp
  
  