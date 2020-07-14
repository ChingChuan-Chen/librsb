#!/bin/bash
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

# This script is an example of configuring the library for debug purposes.

./configure '--enable-allocator-wrapper'  '--enable-debug' \
       	'FC=gfortran' \
       	'CC=gcc' \
       	'CFLAGS=-O0 -ggdb -pipe -Wall -Wredundant-decls -Wno-switch -Wdisabled-optimization -Wdeclaration-after-statement   -Wpointer-arith -Wstrict-prototypes ' \
	'FCFLAGS=-O0 -ggdb  '	\
	'--enable-librsb-stats' \
	'--enable-rsb-num-threads' \
	'--enable-zero-division-checks-on-solve' \
	"$@"

