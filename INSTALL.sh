#!/bin/bash
libtoolize --install --copy --force --automake
aclocal -I m4
autoconf
autoheader
automake --add-missing --copy --foreign --force-missing
./configure
make clean
make
./configure --enable-debug
make clean
make
