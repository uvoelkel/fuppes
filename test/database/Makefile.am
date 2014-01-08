noinst_PROGRAMS += search-criteria-test

search_criteria_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

search_criteria_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

search_criteria_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

search_criteria_test_SOURCES = \
  database/search-criteria-test.cpp


noinst_PROGRAMS += sort-criteria-test

sort_criteria_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

sort_criteria_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

sort_criteria_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

sort_criteria_test_SOURCES = \
  database/sort-criteria-test.cpp
