#!/bin/bash
# Script to build the librsb example programs.

LIBRSB_CONFIG=${LIBRSB_CONFIG:-librsb-config}

for s in *.c
do
	p=${s/.c/}
	rm -f $p 
	CFLAGS=`${LIBRSB_CONFIG} --I_opts`
       	LDFLAGS=`${LIBRSB_CONFIG} --ldflags --extra_libs`
	CC=`${LIBRSB_CONFIG} --cc`
	cmd="$CC $CFLAGS $s $LDFLAGS -o $p"
	echo $cmd
	$cmd
done

if test x"yes" = x"yes" ; then
# activated if you have built the Fortran modules and installed them in the right path.
for s in *.F90
do
	p=${s/.F90/}
	rm -f $p 
	CFLAGS=`${LIBRSB_CONFIG} --I_opts`
       	LDFLAGS=`${LIBRSB_CONFIG} --ldflags --extra_libs`
	FC=`${LIBRSB_CONFIG} --fc`
	cmd="$FC $CFLAGS $s $LDFLAGS -o $p"
	echo $cmd
	$cmd
done
fi


