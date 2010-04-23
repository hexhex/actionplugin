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
	$(BOOST_CPPFLAGS) \
	$(DLVHEX_CFLAGS)

libdlvhexstring_la_LDFLAGS = -avoid-version -module


libdlvhexstring-static.la: $(libdlvhexstring_la_OBJECTS)
	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexstring_la_OBJECTS)

install-static: libdlvhexstring-static.la
	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexstring-static.la $(DESTDIR)$(plugindir)/libdlvhexstring-static.la

uninstall-static:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexstring-static.la
