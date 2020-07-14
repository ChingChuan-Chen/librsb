#!/bin/bash
#
# Copyright (C) 2008-2016 Michele Martone
# 
# This file is part of librsb.
# 
# librsb is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# librsb is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with librsb; see the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.

# This script is intended for the librsb developer usage.

# in the following line, libefence may fail when librsb allocates e.g. 0 bytes (it happens)
LD_PRELOAD=libefence.so.0.0 ./rsbench -B # || exit 255 # efence does not stand zero sized reallocs
if grep RSB_REINIT_SINGLE_VALUE *.c --exclude rsb_rsb.c ; then exit 255 ; fi
#if grep '//' *.c  ; then exit 255 ; fi # TODO: activate this.
if grep -n 'RSB_DO_ERR_RETURN\>' rsb_rsb.c  ; then exit 255; else true ; fi
if cpp rsb.h | grep '()$'   ; then echo '[!] failed'; exit 255 ; else true ; fi
#for f in *.h ; do if cpp $f | grep '()$'   ; then echo '[!] failed'; exit 255 ; else true ; fi ; done
if grep -n --exclude=rsb_rsb.c 'RSB_DO_ERR_RETURN_INTERFACE\>' *.c ; then exit 255 ; else true ; fi
if test -f librsb.a ; then
if nm librsb.a | grep  '\s[DG]\s' | grep -v '\s[DG]\s''rsb_' ; then exit 255 ; else true ; fi
if nm librsb.a  | grep '\<T\>' | sed 's/^.*\s//g' | grep -v '^\(rsb\|BLAS\|blas\|__\)' ; then exit 255 ; else true ; fi
if ar t librsb.a | grep -v  '_a-rsb' | grep -v ^rsb_ ; then echo '[!] failed source filenames check'; exit 255; else true ; fi
else
echo "no librsb.a -- skipping part of the test."
fi
flawfinder rsb_rsb.c | tee flawfinder.log
echo "output of running flawfinder in rats.log"
rats rsb_rsb.c | tee rats.log
if cat *.F90 examples/*.F90| sed 's/!.*$//g'| grep '^.\{73,\}' ; then echo 'Some source code exceeds 72 chars!'; fi
if grep -n '^.\{81,\}' README ; then exit 255 ; else true ; fi
if grep -n '^.\{81,\}' NEWS       ; then exit 255 ; else true ; fi
if grep '	' *.F90 */*.F90 ; then exit 255 ; else true ; fi
./rsbench -E 0.1s || exit 255
./rsbench  --limits-testing || exit 255
echo "output of running rats in rats.log"
