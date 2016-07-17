#!/bin/sh
$EXTRACTRC `find . -name '*.ui' -o -name '*.rc' -o -name '*.ui'` >> rc.cpp
$XGETTEXT `find . -name '*.cpp'` -o $podir/krename.pot
rm -f rc.cpp
