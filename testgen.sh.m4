#!/bin/sh


# FIXME : THIS SCRIPT IS NO LONGER NEEDED, AS rsbench PERFORMS WELL.
return 0;

MOPS=$(echo 'WANT_MATRIX_OPS'  | sed 's/[,()]/ /g')

rm -f Makefile.mops

echo CFLAGS=CFLAGS_	>>  Makefile.mops
echo CC=CC_		>>  Makefile.mops

for mop in $MOPS
do
	echo -e \
	"extern int main_block_partitioned_$mop(int argc,char *argv[]);\n"\
	"int main(int argc,char *argv[])\n"\
	"{\n"\
	"	return main_block_partitioned_$mop(argc,argv);\n"\
	"}\n" > main_block_partitioned_$mop.c
	echo main_block_partitioned_$mop: main_block_partitioned_$mop.o LIBOBJS >>  Makefile.mops
	echo '	$(CC) $(CFLAGS)' -o main_block_partitioned_$mop main_block_partitioned_$mop.o LIBOBJS >>  Makefile.mops
	echo  >>  Makefile.mops
done
for mop in $MOPS
do
	make -f Makefile.mops main_block_partitioned_$mop
done

