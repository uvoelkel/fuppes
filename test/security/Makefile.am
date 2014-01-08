noinst_PROGRAMS += security-test

security_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

security_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

security_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

security_test_SOURCES = \
  security/security-test.cpp
