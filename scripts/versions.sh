#!/bin/sh
#
# Copyright (C) 2008-2015 Michele Martone
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

# should dump out versions of programs used to build our library with success.
# this info once collected wull be useful in case of debugging and compatibility issues.
# FIXME : unfinished :)

grep=grep
which=which
test=test
mf=Makefile
null=/dev/null
sed=sed

$which $grep 2>&1 > $null || exit
$which $sed  2>&1 > $null || exit

se='s/^.*=\s//g'
s="$sed $se" 
tr='tr "\n"   "_" '
#tr=cat
e=echo

v=--version

`$grep '^M4 ='     $mf | $s` $v | $tr
$e
`$grep '^CC ='     $mf | $s` $v | $tr
$e
`$grep '^OCTAVE =' $mf | $s` $v | $tr
$e

