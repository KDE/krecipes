#! /usr/bin/env bash
$EXTRACTRC `find -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
$XGETTEXT `find . -name "*.cpp" -o -name "*.h" | grep -v '/tests/'` -o $podir/krecipes.pot
rm -f rc.cpp
