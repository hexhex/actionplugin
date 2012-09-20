
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
		PluginActionBase.cpp \
		ActionPluginModelCallback.cpp \
		ActionPluginFinalCallback.cpp \
		ActionPluginInterface.cpp \
		BestModelSelector.cpp \
		ExecutionScheduleBuilder.cpp \
		ActionPluginCtxData.cpp \
		ActionParserModule.cpp \
		BuiltInDeclarationsParserModule.cpp

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
