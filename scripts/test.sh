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

if test x"${srcdir}" = x ; then srcdir=. ; fi
ULIMIT_S=10000
echo "Invoking ulimit -s ${ULIMIT_S}"
ulimit -s ${ULIMIT_S} 

# This should be the main script for library consistence checking.
# TODO : for every:
#	 * type
#	 * matrix storage
#	 * op <-
#	 * missing boundary and 'limit' testing! e.g.: ./rsbench -g -r 1000000 -c 1000000 -b 20
#	 * rsbenchxx (albeit minimal) testing
#	 * -i (in place)
#
#	TODO: need minimal testing for -h --help
# FIXME : complete the 'strict' option

gen_liminal()
{
	ms=`$B --configuration | grep RSB_MAX_MATRIX_DIM |sed "s/^RSB_MAX_MATRIX_DIM://g"` ; 
	[ -z "$ms" ] && exit 1
	r=$ms
	c=$ms
	n=4
cat << EOF > $M
%%MatrixMarket matrix coordinate real general
% this matrix is sized as the limits of your architecture allow
$r $c $n
1 1
1 $c
$r 1
$r $c
EOF
}
nulldev=/dev/null
alias 2>&1 > $nulldev || exit 1

alias fail='exit 1'
#alias fail='( echo "[!]" ; exit 1 ; )' # seems to not fail..
e='echo'
x='echo "--"; echo'
strict=1
d=`pwd`
B=$d/rsbench
M=$d/test.mtx
n=100
pt=0
ft=0
st=0
fc=""
$x || exit -1

make rsbench || fail	# we want our test programs

ru=`$B -C | grep row.unrolls |sed "s/^row unrolls://g;s/ /,/g"`;
cu=`$B -C | grep column.unrolls |sed "s/^column unrolls://g;s/ /,/g"`;

if test -z "$ru" ; then fail ; fi
if test -z "$cu" ; then fail ; fi

# FIXME : compact all of these flags in some way..
if true ; then 

$x "$B -h"
    $B -h || fail # 

$x "$B -oa -Ob -h"
    $B -oa -Ob -h || fail # 

#$x "$B -M"
#    $B -M || fail # 

$x "$B -H"
    $B -H || fail # 

$x "$B -C"
    $B -C || fail # 

pdm=pd.mtx

$x "$B -G $pdm"
    $B -G $pdm || fail # 

# FIXME: the following functionalities should be improved and made public
$x "$B -oa -Ob  -f $pdm --matrix-dump-graph $pdm.dot --matrix-dump-internals"
$B -oa -Ob  -f $pdm --matrix-dump-graph $pdm.dot --matrix-dump-internals || fail # 

$x "$B -ot -Ob --lower 3"
$B -ot -Ob --lower 3 || fail # 

$x "$B -P $pdm"
    $B -P $pdm || fail # 

$x "$B -I"
   # $B -I || make feedback
    $B -I || fail # system information dumpout

bmfn=test.mtx.rsb
for deff in "-R -Fbo -qH" "-R -Fbo" ; do
for detr in "--lower" "--dense" ; do
if $B --configuration | grep 'XDR.*off' ; then
	st=$((st+1))
else
	$x "$B -oa -Ob $detr=10 -w $bmfn $deff"
	    $B -oa -Ob $detr=10 -w $bmfn  || fail # binary I/O test
	
	$x "$B -oa -Ob -b $bmfn $deff"
	    $B -oa -Ob -b $bmfn  || fail # binary I/O test
fi
done
done

$x "$B -OR"
    $B -OR || fail # 

#$x "$B -Ot -b"
#    $B -Ot -b || fail #  FIXME : broken

#$x "$B -e"
#    $B -e -f test.mtx || fail # FIXME : broken

#$x "$B -Or"
#    $B -Or || fail # FIXME : full testing . slow !

$x "$B --matrix-ls ${srcdir}/A.mtx"
    $B --matrix-ls ${srcdir}/A.mtx || fail # 

#$x "$B -Oc -f $M"
#    $B -Oc -f $M || fail # FIXME : fuller testing . slow !

#$x "$B -os -Ob"
#    $B -os -Ob || fail # FIXME


#$x "$B -o$o -Ob"
#    $B -o$o -Ob || fail # FIXME : o in v a m s c i n S

	$B --plot-matrix -aRzd -f $pdm || fail
fi

# The following are here for (rather flimsy) testing/coverage purposes:
$x "$B -oa -Ob -f ${srcdir}/A.mtx -Fo  --z-sorted-coo"
$B -oa -Ob -f ${srcdir}/A.mtx -Fo  --z-sorted-coo || fail
$x "$B -oa -Ob --lower 4 --want-no-recursive --ilu0"
$B -oa -Ob --lower 4 --want-no-recursive --ilu0 || fail
$x "$B -oa -Ob --lower 4 -K"
$B -oa -Ob --lower 4 -K || fail
$x "$B -oa -Ob --dense 10 --nrhs 1,2 --incy 1,2 --nrhs 1,2 -K"
$B -oa -Ob --dense 10 --nrhs 1,2 --incy 1,2 --nrhs 1,2 -K

if test "x`which gfortran`" != x -a gfortran -v ; then
	make blas_sparse.F90 || fail
	make sbtf.F90 || fail
	CFLAGS='-O0 -ggdb -DHAVE_RSB_KERNELS=1 -fopenmp'
	gfortran $CFLAGS -c blas_sparse.F90 || fail
	gfortran $CFLAGS -o sbtf blas_sparse.F90 sbtf.F90 -lrsb -L. || fail
	./sbtf || fail
fi

# FIXME : -b X implies a bandwidth of X!
for o in "-b 9" "-n 10%" "-n 2%"; 
do
#for n in 10 100 1000 2000 4000 8000  ;
for n in 1 2 3 4  10 100 200;
do
case $n in 
	-1)
	# FIXME : the code is still not mature for handling well this...
	gen_liminal ;; # in one case we test for a limit sized matrix
	1|2|3|4)
	# matrix generation
	$x "$B -g -r $n -c $n -b $n  > $M"
	   $B -g -r $n -c $n -b $((n-1))  > $M || fail
	;;
	*)
	# matrix generation
	$x "$B -g -r $n -c $n $o  > $M"
	   $B -g -r $n -c $n $o  > $M || fail
esac

# 20110607 FIXME this is only a partial fix, which shall provide a small matrix when matrix creation was disabled at configure time
$B --configuration | grep RSB_IOLEVEL:0 && cp $pdm $M 

sep="              *"

for f in `$B --configuration | grep format.switches |sed "s/^format switches://g"` ; # for every supported format
do

# various formats testing
#for a in "" "-A" "-R";	# with automatic and non automatic blocking size choice (still (still brokenbroken)
for a in "" "-R";	# with automatic and non automatic blocking size choice (still (still brokenbroken)
do
	# matrix dumpout
	$x "$B -Od -f $M -F $f"
	   $B -Od -f $M -F $f > $nulldev || fail # needs -f matrixname

	# basic matrix handling tests (FIXME : doesn't test/support -A flag!)
	# 20100829 removed -s from the following
#	$x "$B -Ot -f $M -r $ru -c $cu -F $f $a"
#	   $B -Ot -f $M -r $ru -c $cu -F $f $a || fail 

#	$x "$B -oa -Ob -f $M -d -F $f -t 1 $a"
#	   $B -oa -Ob -f $M -d -F $f -t 1 $a || fail
	c="$?"
#	$x "$B -oa -Ob -f $M -d -F $f -t 1 $a"
#	es="$B -oa -Ob -f $M -d -F $f -t 1 $a"

	$x "$B -oa -Ob -f $M  -F $f -t 1 $a"
	   $B -oa -Ob -f $M  -F $f -t 1 $a || fail
	c="$?"
	$x "$B -oa -Ob -f $M  -F $f -t 1 $a"
	es="$B -oa -Ob -f $M  -F $f -t 1 $a"

	if test x"$c" = x"0" 
	then
		pt=$((pt+1))
	else
		ft=$((ft+1))
		fc="$fc\n$es"
		if test x"$strict" = x"1" ; then
			# FIXME
			exit -1
		fi
	fi
#	./rsbench -Ot -f $B # test_matops.c should care
#	if test x"$?" = x"0" ; then pt=$((pt+1)) ; else ft=$((ft+1)) ; fi
done
done
done
done

$x "$B -oa -Ob -R --dense 100 --write-performance-record test.rpr"
    $B -oa -Ob -R --dense 100 --write-performance-record test.rpr || $x
$x "$B                      --read-performance-record test.rpr"
    $B                      --read-performance-record test.rpr || $x

$x "$B -oa -Ob -R --write-performance-record test.rpr pd.mtx non-existing.mtx"
    $B -oa -Ob -R --write-performance-record test.rpr pd.mtx non-existing.mtx || $x

rm test.rpr || $x

if ! test x"$RSB_SHORT_TEST_SH" = x1; then
$x "$B -B"
    $B -B || fail # 
fi

$e "passed  tests : $pt"
$e "failed  tests : $ft"
$e "skipped tests : $st"

if test "$ft" != "0" ; then fail ; fi

echo $fc
