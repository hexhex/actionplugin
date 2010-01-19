AUTOMAKE_OPTIONS = gnu

plugin_LTLIBRARIES = libactionkbmod.la

AM_CPPFLAGS= -I$(top_srcdir)/include \
             $(DEBUG_FLAGS) \
             $(DLVHEX_CFLAGS)

libactionkbmod_la_SOURCES = KBModAddon.cpp AssertAtom.cpp RetractAtom.cpp ExecuteAtom.cpp

libactionkbmod_la_LDFLAGS =-version-info="" -avoid-version
