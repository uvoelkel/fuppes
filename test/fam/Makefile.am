noinst_PROGRAMS += fam-test

fam_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

fam_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

fam_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

fam_test_SOURCES = \
  fam/fam-test.cpp
