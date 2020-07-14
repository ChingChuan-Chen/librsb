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

# This script is intended for the librsb developer usage.

if ! grep CFLAGS.*coverage Makefile 2>&1 > /dev/null  ; then
	echo "[!] Cannot perform coverage test (did not compile with --coverage !?)" 
	exit
else
	true;
fi
cd examples 
cd -
make -j 2 all sbtc || exit 1

#rm -f *.gcda        *.gcov
lcov           --directory `pwd` --zerocounters

make qqtests        || exit 1
scripts/devtests.sh
./rsbench --generate-matrix -r 100 -c 100 -n 1024 >  /dev/shm/rsb_matrix.mtx && ./rsbench -oa -Ob -R  -f  /dev/shm/rsb_matrix.mtx # for coverage of rsb_util_sort_row_major_parallel
./rsbench oa -Ob -R  --dense 2 --zig-zag # coverage of rsb_do_reverse_odd_rows
RSB_SHORT_TEST_SH=1 sh scripts/test.sh || exit 1
for f in *.o ; do gcov -f ${f/.o/}  ; done
cd examples || exit 1
#rm -f *.gcda        *.gcov
make tests  || exit 1
for f in *.o ; do gcov -f ${f/.o/}  ; done
cd -

rm -f *.info
lcov --capture --directory `pwd`         --output-file coverage.info
lcov --capture --directory `pwd`/examples/ --output-file coverage-examples.info 
lcov  -a coverage.info -a coverage-examples.info  -o coverage-total.info
genhtml coverage-total.info --highlight --legend --no-branch-coverage --function-coverage --branch-coverage  --output-directory coverage-info-dir
echo "[*] Coverage test performed." 
echo "[*] At next 'make clean', remember to rm -f *.gcov *.gcno" 
