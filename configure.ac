AC_PREREQ([2.61])
AC_INIT([dlvhex-stringplugin],[0.3.0],[roman@kr.tuwien.ac.at])
AC_CONFIG_SRCDIR([src/stringplugin.cpp])

MAJOR=`echo $PACKAGE_VERSION | sed "s,^\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\),\1,"`
MINOR=`echo $PACKAGE_VERSION | sed "s,^\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\),\2,"`
MICRO=`echo $PACKAGE_VERSION | sed "s,^\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\),\3,"`

AC_DEFINE_UNQUOTED([STRINGPLUGIN_MAJOR], [$MAJOR], [Major version number of the string-plugin])
AC_DEFINE_UNQUOTED([STRINGPLUGIN_MINOR], [$MINOR], [Minor version number of the string-plugin])
AC_DEFINE_UNQUOTED([STRINGPLUGIN_MICRO], [$MICRO], [Micro version number of the string-plugin])

AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

# by default we look into /opt/local and /sw for include files (macports vs. fink)
case $host_os in
     darwin*|macosx*)
	CPPFLAGS="$CPPFLAGS -I/opt/local/include -I/sw/include"
     ;;
esac 


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS([sys/param.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_LIBTOOL_DLOPEN # we build a module library
AC_DISABLE_STATIC # we don't need a static library
AC_PROG_LIBTOOL

#
# check for other libraries here
#

#
# of course we need dlvhex itself!
#
PKG_CHECK_MODULES([DLVHEX], [dlvhex >= 1.7])


# checking for boost libs
BOOST_REQUIRE([1.33.0])
BOOST_SMART_PTR
BOOST_STRING_ALGO
BOOST_TOKENIZER


#
# where the plugin will be installed by default.
# running configure will take sysplugindir from dlvhex.pc,
# running configure --enable-userinstall will take homeplugindir.
#

AC_ARG_ENABLE(userinstall, 
             [  --enable-userinstall    Install plugin into home directory],
             [plugindir=$HOME/`pkg-config dlvhex --variable=userplugindir`],
             [plugindir=`pkg-config dlvhex --variable=sysplugindir`]
             )
AC_SUBST([plugindir])

#
# all needed makefiles
#
AC_CONFIG_FILES([
           Makefile 
           src/Makefile
           examples/Makefile
])

AC_OUTPUT

