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

TESTS = tests/run-dlvhex-tests.sh
TESTS_ENVIRONMENT = DLVHEX=dlvhex TESTDIR=$(top_srcdir)/examples/tests PARAMETERS="-s --plugindir=$(top_builddir)/src"
