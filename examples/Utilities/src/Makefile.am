plugin_LTLIBRARIES = libdlvhexplugin-utilities.la

#
# extend compiler flags by CFLAGS of other needed libraries
#
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS) \
	$(DLVHEX_CFLAGS)

libdlvhexplugin_utilities_la_SOURCES = UtilitiesActionPlugin.cpp
libdlvhexplugin_utilities_la_LDFLAGS = -avoid-version -module -L$(top_builddir)/src/ #-rpath /
libdlvhexplugin_utilities_la_LIBADD = -ldlvhex-action $(DLVHEX_LIBS)

libdlvhexplugin-utilities-static.la: $(libdlvhexplugin_utilities_la_OBJECTS)
	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexplugin_utilities_la_OBJECTS) $(libdlvhexplugin_utilities_la_LIBADD)

install-static: libdlvhexplugin-utilities-static.la
	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexplugin-utilities-static.la $(DESTDIR)$(plugindir)/libdlvhexplugin-utilities-static.la

uninstall-static:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexplugin-utilities-static.la
