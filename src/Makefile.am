#
# replace 'plugin' on the right side by your plugin's name
#
plugin_LTLIBRARIES = libdlvhexplugin-action.la

#
# replace 'plugin' on the left side as above and
# add all sources of your plugin
#
libdlvhexplugin_action_la_SOURCES = ActionPlugin.cpp

#
# extend compiler flags by CFLAGS of other needed libraries
#
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(BOOST_CPPFLAGS) \
	$(DLVHEX_CFLAGS)

libdlvhexplugin_action_la_LDFLAGS = -avoid-version -module


libdlvhexplugin-action-static.la: $(libdlvhexplugin_action_la_OBJECTS)
	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexplugin_action_la_OBJECTS)

install-static: libdlvhexplugin-action-static.la
	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexplugin-action-static.la $(DESTDIR)$(plugindir)/libdlvhexplugin-action-static.la

uninstall-static:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexplugin-action-static.la
