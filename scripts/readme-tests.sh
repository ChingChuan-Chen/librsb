if test x"${srcdir}" = x ; then srcdir=. ; fi
 ./rsbench -oa -Ob -f ${srcdir}/A.mtx -qH -R -n1 -t100 --verbose  || exit 255
 ./rsbench --help || exit 255
 ./rsbench -oa -Ob --help || exit 255
 ./rsbench --help || exit 255
 ./rsbench --version || exit 255
 ./rsbench -I || exit 255
 ./rsbench -C || exit 255
    test -f sbtc && ./sbtc||true  || exit 255
    test -f sbtf && ./sbtf||true  || exit 255
    ./rsbench -Q 10.0  || exit 255
    ./rsbench  -oa -Ob -qH -R --dense 1                    --verbose || exit 255
    ./rsbench  -oa -Ob -qH -R --dense 1024                 --verbose || exit 255
    ./rsbench  -oa -Ob -qH -R --lower 1024 --as-symmetric  --verbose || exit 255
    ./rsbench  -oa -Ob -qH -R --dense 1000 --gen-lband 10 --gen-uband 3 || exit 255
    ./rsbench  -oa -Ob -qH -R --generate-diagonal 1000 || exit 255
