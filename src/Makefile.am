# #
# # replace 'plugin' on the right side by your plugin's name
# #
# plugin_LTLIBRARIES = libdlvhexplugin-action.la
# 
# #
# # replace 'plugin' on the left side as above and
# # add all sources of your plugin
# #
# libdlvhexplugin_action_la_SOURCES = \
# 					ActionPlugin.cpp \
# 					Action.cpp \
# 					ActionPluginModelCallback.cpp \
# 					ActionPluginFinalCallback.cpp \
# 					ActionScheduler.cpp \
# 					RobotActionPlugin.cpp
# 
# #
# # extend compiler flags by CFLAGS of other needed libraries
# #
# AM_CPPFLAGS = \
# 	-I$(top_srcdir)/include \
# 	$(BOOST_CPPFLAGS) \
# 	$(DLVHEX_CFLAGS)
# 
# libdlvhexplugin_action_la_LDFLAGS = -avoid-version -module
# 
# 
# libdlvhexplugin-action-static.la: $(libdlvhexplugin_action_la_OBJECTS)
# 	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexplugin_action_la_OBJECTS)
# 
# install-static: libdlvhexplugin-action-static.la
# 	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexplugin-action-static.la $(DESTDIR)$(plugindir)/libdlvhexplugin-action-static.la
# 
# uninstall-static:
# 	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexplugin-action-static.la

lib_LTLIBRARIES = libdlvhex-action.la
plugin_LTLIBRARIES = libdlvhexplugin-action.la

# most parts of the implementation are in here
# this is not a plugin library but a regular shared library with a version
#
# the reason for that is, that additional context semantics can be implemented
# in additional plugins which then depend on libdlvhex-action
# see the testcases for examples of such additional plugins
libdlvhex_action_la_SOURCES = \
		Action.cpp \
		ActionPlugin.cpp \
		PluginActionBase.cpp \
		ActionPluginModelCallback.cpp \
		ActionPluginFinalCallback.cpp \
		ActionScheduler.cpp \
		ActionPluginInterface.cpp \
		BestModelSelector.cpp \
		ExecutionModeRewriter.cpp \
		ActionPluginCtxData.cpp \
		ActionPluginParserModule.cpp \
		ActionPluginParserModuleForBuiltInConstants.cpp

# for more information on `-version-info' see
# <http://www.gnu.org/software/libtool/manual.html#Versioning>, and
# the USENIX paper `Library Interface Versioning in Solaris and Linux'
# <http://www.usenix.org/publications/library/proceedings/als00/2000papers/papers/full_papers/browndavid/browndavid.pdf>
libdlvhex_action_la_LDFLAGS = -version-info 2:0:0

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
  $(BOOST_CPPFLAGS) \
	$(DLVHEX_CFLAGS)

# the plugin library module, which links against libdlvhex-action
libdlvhexplugin_action_la_SOURCES = ActionPlugin.cpp
libdlvhexplugin_action_la_LDFLAGS = -avoid-version -module
libdlvhexplugin_action_la_LIBADD = libdlvhex-action.la


#robot_LTLIBRARIES = libdlvhexplugin-robot_action.la
#libdlvhexplugin_robot_action_la_SOURCES = RobotActionPlugin.cpp
#libdlvhexplugin_robot_action_la_LDFLAGS = -avoid-version -module
#libdlvhexplugin-robot_action-static.la: $(libdlvhexplugin_robot_action_la_OBJECTS)
#	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexplugin_robot_action_la_OBJECTS)
#install-static: libdlvhexplugin-robot_action-static.la
#	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexplugin-robot_action-static.la $(DESTDIR)$(plugindir)/libdlvhexplugin-robot_action-static.la
#uninstall-static:
#	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexplugin-robot_action-static.la
