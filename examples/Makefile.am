EXTRA_DIST = \
  concat.hex \
  strstr.hex \
  split.hex \
  sha1sum.hex \
  tests/stringplugintests.test \
  tests/concat.out \
  tests/strstr.out \
  tests/split.out \
  tests/sha1sum.out

DIST_SOURCES = ../testsuite/run-dlvhex-tests.sh

TESTS = $(top_srcdir)/testsuite/run-dlvhex-tests.sh

TESTS_ENVIRONMENT = \
  TOP_SRCDIR=$(top_srcdir) \
  DLVHEX="$(DLVHEX_BINDIR)/dlvhex -s --plugindir=$(top_builddir)/src --verbose=255" \
  EXAMPLESDIR=$(top_srcdir)/examples \
  TESTDIR=$(top_srcdir)/examples/tests