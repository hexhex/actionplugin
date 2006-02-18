AUTOMAKE_OPTIONS = foreign

#
# replace 'plugin' on the right side by your plugin's name
#
plugin_LTLIBRARIES = libdlvhexstring.la

#
# replace 'plugin' on the left side as above and
# add all sources of your plugin
#
libdlvhexstring_la_SOURCES = stringplugin.cpp

#
# extend compiler flags by CFLAGS of other needed libraries
#
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(DLVHEX_CFLAGS)

#
# replace 'plugin' on the left side as above and
# add other needed libs to linker flags
#
libdlvhexstring_la_LDFLAGS = $(DLVHEX_LIBS)



