#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([dlvhex-actionplugin],[0.1.0],[selenbasol@su.sabanciuniv.edu])
AC_CONFIG_SRCDIR([src/ActionPlugin.cpp])
AC_CONFIG_HEADER([include/config.h])

MAJOR=`echo $PACKAGE_VERSION | sed "s,^\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\),\1,"`
MINOR=`echo $PACKAGE_VERSION | sed "s,^\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\),\2,"`
MICRO=`echo $PACKAGE_VERSION | sed "s,^\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\),\3,"`

AC_DEFINE_UNQUOTED([ACTIONPLUGIN_MAJOR], [$MAJOR], [Major version number of the action-plugin])
AC_DEFINE_UNQUOTED([ACTIONPLUGIN_MINOR], [$MINOR], [Minor version number of the action-plugin])
AC_DEFINE_UNQUOTED([ACTIONPLUGIN_MICRO], [$MICRO], [Micro version number of the action-plugin])

AM_INIT_AUTOMAKE([dist-bzip2])

#Initialize libtool
AC_LIBTOOL_DLOPEN
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# doxygen stuff
#
# these are the defaults, can be overridden by --enable-doxygen-...
DX_HTML_FEATURE([ON])
DX_CHM_FEATURE([OFF])
DX_CHI_FEATURE([OFF])
DX_MAN_FEATURE([OFF])
DX_RTF_FEATURE([OFF])
DX_XML_FEATURE([OFF])
DX_PDF_FEATURE([OFF])
DX_PS_FEATURE([OFF])
DX_INIT_DOXYGEN([dlvhex-actionplugin], [doxygen.cfg], [doc])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_ARG_ENABLE(debug-symbols,[  --enable-debug-symbols 	Enable debug symbols for the plugin.],[ DEBUG_FLAGS="-g -Wall" ; AC_SUBST([DEBUG_FLAGS])],
[DEBUG_FLAGS="" ; AC_SUBST([DEBUG_FLAGS])])
# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h] [sys/param.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T 

# Checks for raptor and dlvhex via pkg-config
PKG_CHECK_MODULES([DLVHEX], [dlvhex >= 1.7])
#PKG_CHECK_MODULES([RAPTOR], [raptor >= 1.4.8])

# checking for boost libs
#BOOST_REQUIRE([1.35.0])
#BOOST_SMART_PTR
#BOOST_STRING_ALGO
BOOST_REGEX

AC_ARG_ENABLE(userinstall, 
		[  --enable-userinstall    Install plugin into home directory],
		[plugindir=$HOME/`pkg-config dlvhex --variable=userplugindir`],
		[plugindir=`pkg-config dlvhex --variable=sysplugindir`]
	     )
AC_SUBST([plugindir])

#
# the default system-wide plugin dir $(pkglibdir)/plugins can be
# overridden by setting PLUGIN_DIR=... at configure-time
#
if test "$SYS_PLUGIN_DIR" = ""; then
    sysplugindir="\${libdir}/dlvhex/plugins"
else
    sysplugindir="$SYS_PLUGIN_DIR"
fi

AC_SUBST(sysplugindir)

CPPFLAGS="$CPPFLAGS -DSYS_PLUGIN_DIR='\"$sysplugindir\"'"

#
# the user plugin dir consists of the user home concatenated with
# and USER_PLUGIN_DIR, which is .dlvhex/plugins by default and can be
# overridden by setting USER_PLUGIN_DIR=... at configure-time
#
if test "$USER_PLUGIN_DIR" = ""; then
    userplugindir=".dlvhex/plugins"
else
    userplugindir="$USER_PLUGIN_DIR"
fi

AC_SUBST(userplugindir)

CPPFLAGS="$CPPFLAGS -DUSER_PLUGIN_DIR='\"$userplugindir\"'"

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
		 examples/Makefile
		 include/Makefile
                 src/Makefile
                 testsuite/Makefile])
AC_OUTPUT


