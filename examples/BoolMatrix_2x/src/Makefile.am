plugin_LTLIBRARIES = libdlvhexplugin-boolMatrix_action.la

#
# extend compiler flags by CFLAGS of other needed libraries
#
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS) \
	$(DLVHEX_CFLAGS)

libdlvhexplugin_boolMatrix_action_la_SOURCES = BoolMatrixActionPlugin.cpp TGAImageMaker.cpp
libdlvhexplugin_boolMatrix_action_la_LDFLAGS = -avoid-version -module -L$(top_builddir)/src/ #-rpath /
libdlvhexplugin_boolMatrix_action_la_LIBADD = -ldlvhex-action $(DLVHEX_LIBS)

libdlvhexplugin-boolMatrix_action-static.la: $(libdlvhexplugin_boolMatrix_action_la_OBJECTS)
	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexplugin_boolMatrix_action_la_OBJECTS) $(libdlvhexplugin_boolMatrix_action_la_LIBADD)

install-static: libdlvhexplugin-boolMatrix_action-static.la
	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexplugin-boolMatrix_action-static.la $(DESTDIR)$(plugindir)/libdlvhexplugin-boolMatrix_action-static.la

uninstall-static:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexplugin-boolMatrix_action-static.la
