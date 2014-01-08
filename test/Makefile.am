
if BUILD_TESTS

include framework/Makefile.am

include database/Makefile.am
include fam/Makefile.am
include file/Makefile.am
include http-server/Makefile.am
include log/Makefile.am
include security/Makefile.am
include thread/Makefile.am
include transcoding/Makefile.am


EXTRA_DIST = \
	run-test.sh.in \
	run-test-gdb.sh.in \
	run-test-valgrind.sh.in

CLEANFILES = \
	run-test.sh \
	run-test-gdb.sh \
	run-test-valgrind.sh

nodist_noinst_DATA = \
	run-test.sh \
	run-test-gdb.sh \
	run-test-valgrind.sh

run-test.sh: run-test.sh.in
	$(SED) \
		-e "s|\@DATA_DIR\@|$(datadir)/fuppes|" \
		-e "s|\@PLUGIN_DIR\@|$(libdir)/fuppes|" \
	$@.in > $@
	chmod 775 $@

run-test-gdb.sh: run-test-gdb.sh.in
	$(SED) \
		-e "s|\@DATA_DIR\@|$(datadir)/fuppes|" \
		-e "s|\@PLUGIN_DIR\@|$(libdir)/fuppes|" \
	$@.in > $@
	chmod 775 $@

run-test-valgrind.sh: run-test-valgrind.sh.in
	$(SED) \
		-e "s|\@DATA_DIR\@|$(datadir)/fuppes|" \
		-e "s|\@PLUGIN_DIR\@|$(libdir)/fuppes|" \
	$@.in > $@
	chmod 775 $@


endif # BUILD_TESTS

