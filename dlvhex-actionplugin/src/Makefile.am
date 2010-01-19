AUTOMAKE_OPTIONS = gnu

plugin_LTLIBRARIES = libdlvhexaction.la

# install those libraries into $pkglibdir/lib
#pkglib_LTLIBRARIES = libaddonatom.la 

AM_CPPFLAGS= -I$(top_srcdir)/include \
             $(DEBUG_FLAGS) \
             $(DLVHEX_CFLAGS) \
	     $(BOOST_CPPFLAGS)


libdlvhexaction_la_SOURCES = ActionPlugin.cpp ActionConverter.cpp ActionOutputBuilder.cpp AddonContainer.cpp AddonAtom.cpp

libdlvhexaction_la_LDFLAGS =-version-info="" -avoid-version -lboost_regex -module $(BOOST_LIBS) 

#libaddonatom_la_SOURCES = AddonAtom.cpp
