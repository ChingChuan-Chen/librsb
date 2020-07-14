#!/bin/sh

which aclocal    || { echo "no aclocal executable !" ; exit 1 ; }
aclocal    || { echo "aclocal fails!" ; exit 1 ; }
which autoheader || { echo "no autoheader executable !" ; exit 1 ; }
autoheader || { echo "autoheader fails!" ; exit 1 ; }
which autoconf   || { echo "no autoconf executable !" ; exit 1 ; }
autoconf   || { echo "autoconf fails!" ; exit 1 ; }
if test -f ltmain.sh ; then true ; else libtoolize -c || { echo "no libtoolize ?" ; exit 1 ; } ; fi
automake -c -Woverride --add-missing || { echo "no automake ?" ; exit 1 ; } 

