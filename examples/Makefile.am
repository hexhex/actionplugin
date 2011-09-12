EXTRA_DIST = \
  concat.hex \
  strstr.hex \
  split.hex \
  sha1sum.hex \
  tests/run-dlvhex-tests.sh \
  tests/stringplugintests.test \
  tests/concat.out \
  tests/strstr.out \
  tests/split.out \
  tests/sha1sum.out

TESTS = ../testsuite/run-dlvhex-tests.sh
TESTS_ENVIRONMENT = \
  TOP_SRCDIR=$(top_srcdir) \
  DLVHEX="$(DLVHEX_BINDIR)/dlvhex -s --plugindir=$(top_builddir)/src --verbose=255" \
  EXAMPLESDIR=$(top_srcdir)/examples \
  TESTDIR=$(top_srcdir)/examples/tests

#TESTS_ENVIRONMENT = DLVHEX=dlvhex TESTDIR=$(top_srcdir)/examples/tests PARAMETERS="-s --plugindir=$(top_builddir)/src"

#TESTS_ENVIRONMENT = DLVHEX=dlvhex TOPSRCDIR=$(top_srcdir) TOPBUILDDIR=$(top_builddir) PARAMETERS="-s --plugindir=$(top_builddir)/src"
