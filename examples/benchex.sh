#!/bin/sh

# systematic comparative benchmark, mostly for dense matrices
# (with Intel MKL, if linked) benchmark comparing   
# produces a number of plots systematically
bench/dense.sh

# the benchmark command; assumes A.mtx is a file in Matrix Market format
./rsbench -oa -Ob -f A.mtx -qH -R -n1 -t100 --verbose -TD --compare-competitors 

# rsbench is very flexible tool; see the help for it:
./rsbench -oa -Ob --help
