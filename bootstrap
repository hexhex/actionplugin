#!/bin/sh

echo "libtoolize..."
libtoolize \
    || { echo "libtoolize failed"; exit 1; }

echo "aclocal..."
# use this if you have additional files in the m4 directory:
#aclocal -I m4
aclocal \
    || { echo "aclocal failed"; exit 1; }
#
# use this if you want to user autoheader
#
#echo "autoheader..."
#autoheader --warnings=all \
#    || { echo "autoheader failed"; exit 1; }

echo "automake..."
automake --warnings=all --warnings=no-syntax --foreign --add-missing \
    || { echo "automake failed"; exit 1; }

echo "autoconf..."
autoconf --warnings=all \
    || { echo "autoconf failed"; exit 1; }
