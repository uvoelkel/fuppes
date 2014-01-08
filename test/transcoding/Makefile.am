noinst_PROGRAMS += decoder-test

decoder_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

decoder_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

decoder_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

decoder_test_SOURCES = \
	transcoding/decoder-test.cpp
  
  
  
noinst_PROGRAMS += encoder-test

encoder_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

encoder_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

encoder_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

encoder_test_SOURCES = \
	transcoding/encoder-test.cpp
	
	

noinst_PROGRAMS += transcoderexternal-test

transcoderexternal_test_LDADD = \
	./libtestframework.la \
	../src/libfuppes.la

transcoderexternal_test_DEPENDENCIES = \
	./libtestframework.la \
	../src/libfuppes.la

transcoderexternal_test_LDFLAGS = \
	$(FUPPES_LIBS) \
	${LIBXML_LIBS} \
	${PCRE_LIBS} \
	${LIBICONV}

transcoderexternal_test_SOURCES = \
	transcoding/transcoderexternal-test.cpp
