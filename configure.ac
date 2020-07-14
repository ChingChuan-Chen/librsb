AC_PREREQ([2.54])
################################################################################
dnl	************************************************************************
dnl	******* the following line shall be commented in branches/... **********
dnl define([librsbsvnversion],[esyscmd([sh -c "basename `svn info | grep ^URL | sed 's/URL: '//g` | tr -d '\n'"])])dnl
dnl	************************************************************************
define([librsbtodaydate],[esyscmd([sh -c "date +'%B %d, %Y' | tr -d '\n'"])])dnl
define([librsbsvnrevision],[esyscmd([sh -c "svnversion -n"])])dnl
dnl	************************************************************************
dnl	**** the following lines shall be updated at each release **************
dnl	************************************************************************
define([librsb_ver_major],[1])dnl
define([librsb_ver_minor],[2])dnl
define([librsb_ver_patch],[0])dnl
define([librsb_ver_prers],[8])dnl
dnl	************************************************************************
define([librsb_ver_string],librsb_ver_major.librsb_ver_minor.librsb_ver_patch)dnl
define([librsb_librsbvernum],[librsb_ver_major"0"librsb_ver_minor"00"])dnl
define([librsb_abi_vernum],[0:0:0])dnl
define([librsb_lib_string],librsb_librsbvernum)dnl
define([librsb_release],librsb_ver_string[.]librsb_ver_prers)dnl
define([librsbversion],[ifelse(librsbsvnversion,[trunk],librsbsvnrevision,librsb_release)])dnl
dnl define([librsbversion],[ifelse(librsbsvnversion,[trunk],[trunk],librsb_ver_major.librsb_ver_minor.librsb_ver_patch[]librsb_ver_prers)])dnl
AC_INIT([librsb],[librsbversion],[michelemartone_AT_users_DOT_sourceforge_DOT_net])
dnl
# The following are valid for both trunk and release version.
# It indicates the version this trunk is loosely related to.
LIBRSB_VER_MAJOR=librsb_ver_major
LIBRSB_VER_MINOR=librsb_ver_minor
LIBRSB_VER_PATCH=librsb_ver_patch
LIBRSB_LIBRSB_VER=librsb_librsbvernum
if test x"librsbsvnversion" = x"trunk" ; then
LIBRSB_VER_PRERS="-trunk"
else
LIBRSB_VER_PRERS="librsb_ver_prers"
fi
LIBRSB_VER_DATE="librsbtodaydate"
LIBRSB_VERSION="librsbversion"
LIBRSB_MAIN_RELEASE="librsb_ver_string"
LIBRSB_ABI_VERSION="librsb_abi_vernum"
AC_SUBST(LIBRSB_VER_MAJOR)
AC_SUBST(LIBRSB_VER_MINOR)
AC_SUBST(LIBRSB_VER_PATCH)
AC_SUBST(LIBRSB_VER_DATE)
AC_SUBST(LIBRSB_VER_PRERS)
AC_SUBST(LIBRSB_LIBRSB_VER)
AC_SUBST(LIBRSB_VERSION)
AC_SUBST(LIBRSB_MAIN_RELEASE)
AC_SUBST(LIBRSB_ABI_VERSION)
################################################################################
SVN_REVISION="librsbsvnrevision"
AH_TEMPLATE([SVN_REVISION])
AC_DEFINE(SVN_REVISION,"librsbsvnrevision",[SVN REVISION])
AC_SUBST(SVN_REVISION)
AH_TEMPLATE([COPYRIGHT_STRING])
AC_DEFINE(COPYRIGHT_STRING,"Copyright (c) 2008-2019 Michele Martone",[])
dnl AC_SUBST([COPYRIGHT_STRING],["Copyright (c) 2008-2019 Michele Martone"])
AC_COPYRIGHT([Copyright (c) 2008-2019, Michele Martone])
################################################################################
AM_INIT_AUTOMAKE
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
dnl AC_PROG_FC
AC_PROG_FC(xlf2003 xlf2003_r ifort gfortran )
m4_if(m4_version_compare(m4_defn([AC_AUTOCONF_VERSION]),[2.60]),-1, [],[AC_OPENMP()])
AC_PROG_CXX(xlC xlC_r7 xlC_r4 xlC_r g++ pgCC )
# on epsilon cc is not reloaded with modules
AC_PROG_CC(xlc_r xlc icc pgcc gcc cc)
# ... Makefile.am:45: compiling `unroll.c' with per-target flags requires `AM_PROG_CC_C_O' in `configure.ac'
AM_PROG_CC_C_O
# libtoolize if autoconf complains for the following
AC_PROG_LIBTOOL
AM_PROG_AS
#AC_GNU_SOURCE
#AC_PROG_C
#AC_PROG_INSTALL
dnl AC_PROG_RANLIB
AC_PROG_AWK
AC_PROG_GREP
AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([char])
dnl AC_CHECK_SIZEOF([unsigned char])
AC_CHECK_SIZEOF([int])
dnl AC_CHECK_SIZEOF([unsigned int])
AC_CHECK_SIZEOF([short int])
dnl AC_CHECK_SIZEOF([short unsigned int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long int])
dnl AC_CHECK_SIZEOF([long unsigned int])
AC_CHECK_SIZEOF([long long int])
dnl AC_CHECK_SIZEOF([long long unsigned int])
AC_CHECK_SIZEOF([size_t])
AC_CHECK_SIZEOF([float])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([long double])
# to use complex we need a specialized header
AC_CHECK_SIZEOF([complex])
AC_CHECK_SIZEOF([float complex])
AC_CHECK_SIZEOF([double complex])
#
m4_if(m4_version_compare(m4_defn([AC_AUTOCONF_VERSION]),[2.60]),-1, [],[AC_OPENMP()])
#
AC_CHECK_PROG([have_grep],[grep],[yes],[no])
AC_CHECK_PROG([have_sed],[sed],[yes],[no])
AC_CHECK_PROGS([OCTAVE],[$OCTAVE octave],[false],[])
AC_CHECK_PROGS([DOXYGEN],[$DOXYGEN doxygen],[false],[])
AC_CHECK_PROGS([HELP2MAN],[$HELP2MAN help2man],[false],[])
AC_CHECK_PROGS([M4],[$M4 m4 gm4 /opt/freeware/bin/m4],[false],[])
dnl Precious variables:
AC_ARG_VAR([M4],[M4 macro preprocessor])
AC_ARG_VAR([OCTAVE],[GNU Octave executable])
AC_ARG_VAR([AR],[Library archiver program])
AC_ARG_VAR([ARFLAGS],[Library archiver program flags])
AC_ARG_VAR([LD],[Linker program])
AC_ARG_VAR([DOXYGEN],[Doxygen program for generating documentation from librsb source code])
AC_ARG_VAR([HELP2MAN],[Help2man is a program for generating man pages from program help output])
AC_ARG_VAR([RSB_USER_SET_MEM_HIERARCHY_INFO],[Memory hierarchy info string for librsb; e.g.: L2:4/64/512K,L1:8/64/24K])
AC_ARG_VAR([OPENMP_CFLAGS],[C compilation flags for OpenMP])
AC_ARG_VAR([OPENMP_FCFLAGS],[Fortran compilation flags for OpenMP])
dnl AC_ARG_VAR([SED],["sed program."])
dnl AC_ARG_VAR([GREP],["grep program."])
#
AC_CONFIG_HEADERS([rsb-config.h],[sed 's/^#define /#define RSB_/g;s/ RSB_RSB_/ RSB_/g' rsb-config.h > rsb-config.h.tmp ; echo '#endif /* RSB_CONFIG_H_INCLUDED */' >> rsb-config.h.tmp ; cat $srcdir/rsb_license_header.inc $srcdir/rsb-config.h.hin rsb-config.h.tmp > rsb-config.h ; rm rsb-config.h.tmp])
AC_C_BIGENDIAN()
AC_FUNC_VPRINTF
AC_HEADER_STDC
AC_C_CONST()
AC_C_INLINE()
AC_TYPE_SIZE_T()
AC_HEADER_TIME()
dnl AC_STRUCT_TM()
dnl AC_HEADER_STDBOOL
dnl AC_C_VOLATILE
dnl AC_CHECK_TYPES([ptrdiff_t])
#
AC_CHECK_FUNC([mlockall], AC_DEFINE([HAVE_MLOCKALL], [1], [If present, the mlockall function makes all allocations memory resident.]))
AC_CHECK_FUNC([sysconf], AC_DEFINE([HAVE_SYSCONF], [1], [If present, the sysconf function gives lots of system info.]))
dnl AC_CHECK_FUNC([hwloc_topology_init], AC_DEFINE([HAVE_HWLOC], [1], [If present, the hwloc_topology_init function gives lots of system info.]))
AC_CHECK_FUNC([gethostname], AC_DEFINE([HAVE_GETHOSTNAME], [1], [If present, will give us host name.]))
AC_CHECK_FUNC([posix_memalign], AC_DEFINE([HAVE_POSIX_MEMALIGN], [1], [The POSIX aligned memory allocator.(The function posix_memalign() is available since glibc 2.1.91)]))
AC_CHECK_FUNC([memalign], AC_DEFINE([HAVE_MEMALIGN], [1], [This function is obsolete.]))
AC_CHECK_FUNC([getenv], AC_DEFINE([HAVE_GETENV], [1], [Get an environment variable.]))
AC_CHECK_FUNC([fileno], AC_DEFINE([HAVE_FILENO], [1], [fileno(): C FILE to posix file descriptor.]))
dnl AC_CHECK_FUNC([gzdirect], AC_DEFINE([HAVE_GZDIRECT], [1], []))

AC_CHECK_FUNCS([rand isatty])
AC_CHECK_FUNCS([sched_getaffinity])
AC_CHECK_FUNCS([memset memcmp strncmp strcpy])
AC_CHECK_FUNCS([dup])
dnl AC_CHECK_FUNCS([read write])
AC_CHECK_FUNCS([fread fwrite])
#dnl	***********************************************************************
#dnl					THESE ARE ESSENTIAL
#dnl	***********************************************************************
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_HEADERS([sched.h])
AC_CHECK_HEADERS([dmalloc.h])
dnl	AC_CHECK_HEADERS([duma.h])
AC_CHECK_FUNC([getopt_long], AC_DEFINE([HAVE_GETOPT_LONG], [1], [getopt_long is GNU candy]))
AC_CHECK_FUNC([times], AC_DEFINE([HAVE_TIMES], [1], [times]))
AC_CHECK_FUNC([gettimeofday], AC_DEFINE([HAVE_GETTIMEOFDAY], [1], [gettimeofday]))
AC_CHECK_FUNC([setenv], AC_DEFINE([HAVE_SETENV], [1], [setenv]))
dnl It would be nice to use alloca (on-stack allocator), but our code is not amenable for this.
dnl AC_CHECK_FUNC([alloca], AC_DEFINE([HAVE_ALLOCA], [1], [alloca]))
dnl AC_CHECK_HEADERS([alloca.h])
AC_CHECK_HEADERS([omp.h])
AC_CHECK_HEADERS([getopt.h])
AC_CHECK_HEADERS([malloc.h memory.h])
AC_CHECK_HEADERS([pthread.h]) dnl unused, for now
AC_CHECK_HEADERS([papi.h])
AC_CHECK_HEADERS([gsl/gsl_sort.h])
AC_CHECK_HEADERS([times.h]) dnl for times (optional)
AC_CHECK_HEADERS([sys/utsname.h])
AC_CHECK_HEADERS([sys/resource.h]) dnl for getrusage (optional)
AC_CHECK_HEADERS([complex.h])
AC_CHECK_HEADERS([assert.h])
AC_CHECK_HEADERS([rpc/xdr.h])
AC_CHECK_HEADERS([sys/mman.h]) dnl mlockall
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([unistd.h]) dnl read write
AC_CHECK_HEADERS([stdio.h]) dnl printf
AC_CHECK_HEADERS([stdarg.h]) dnl vprintf
AC_CHECK_HEADERS([time.h]) dnl 
AC_CHECK_HEADERS([regex.h]) dnl 
AC_CHECK_HEADERS([string.h] [strings.h] [ctype.h]) dnl 
dnl
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [
#ifdef __MINGW32__
#error "You are likely using MINGW (Minimalist GNU for Windows)."
#else
    /* "You are likely not using MINGW (Minimalist GNU for Windows)." */
#endif
])], [ac_cv_we_use_mingw=no], [ac_cv_we_use_mingw=yes])
if test "x$ac_cv_we_use_mingw" = xyes; then
	ac_cv_mingw_add="-D__USE_MINGW_ANSI_STDIO=1"
      	AC_MSG_NOTICE([You are likely using MINGW (Minimalist GNU for Windows). Adding ${ac_cv_mingw_add} to compilation flags to avoid broken C99 support.])
	CFLAGS="${CFLAGS} ${ac_cv_mingw_add}"
fi
dnl
dnl	In the following, will use  AC_ARG_ENABLE  for proper options/features and  AC_ARG_WITH  for packages or environment or compilation switches.
# rsbench-only LIBS and CFLAGS:
RSB_RSBENCH_LIBS=
RSB_RSBENCH_CFLAGS=
dnl
AC_ARG_WITH(math, AC_HELP_STRING([--with-math], [Specify the math library]), [if test "x$withval" = xno; then want_math_libs= ; else want_math_libs="$withval" ; fi], [want_math_libs="-lm"])
AC_CHECK_HEADERS([math.h], [LIBS="${LIBS} $want_math_libs"], [break])

AC_ARG_WITH(xdr, AC_HELP_STRING([--with-xdr], [Specify XDR library.  e.g.: --with-xdr="..." ]), [if test "x$withval" = xno; then want_xdr_libs= ; else want_xdr_libs="$withval" ; fi], [want_xdr_libs=" "])
AC_CHECK_HEADERS([rpc/xdr.h])

AH_TEMPLATE([RSB_WITH_HWLOC])
AC_ARG_WITH(hwloc, AC_HELP_STRING([--with-hwloc], [Specify the hwloc library (EXPERIMENTAL)]), [if test "x$withval" = xno; then want_hwloc_libs= ; else want_hwloc_libs="$withval"; if test "x$want_hwloc_libs" = x"yes" ; then want_hwloc_libs="-lhwloc" ; fi; enable_hwloc=yes ; fi], [want_hwloc_libs=" "])
AC_CHECK_HEADERS([hwloc.h], [if test "x$enable_hwloc" != x -a "x$want_hwloc_libs" != x ; then LIBS="${LIBS} $want_hwloc_libs"; fi;], [break])
 
if test "x${CC}" = x"xlc" -o "x${CC}" = x"xlc_r"  ; then
      	AC_MSG_NOTICE([Disabling XDR support: our API was only tested on Linux.])
	want_xdr_libs=
	ac_cv_header_rpc_xdr_h=no
fi

if test x"$want_xdr_libs" != x"" && test "x$ac_cv_header_rpc_xdr_h" = xyes ; then
	dnl use --without-xdr to disable it
      	AC_MSG_NOTICE([Enabling xdr support.])
	AC_DEFINE([RSB_WANT_XDR_SUPPORT],[1],[experimental.])
	LIBS="${LIBS} $want_xdr_libs"
	else
	AC_MSG_WARN([No xdr headers found.])
	AC_DEFINE([RSB_WANT_XDR_SUPPORT],[0],[experimental.])
fi
dnl
AH_TEMPLATE([RSB_WANT_DMALLOC])
AC_ARG_WITH(dmalloc, AC_HELP_STRING([--with-dmalloc], [With dmalloc (experimental).]),
[if test "x$withval" = xyes; then
	enable_dmalloc=1;
	if test x"$ac_cv_header_dmalloc_h" = xyes ; then
		LIBS="${LIBS} -ldmalloc"
		DMALLOC_CFLAGS="-DDMALLOC"
	fi
 else 
	enable_dmalloc=0
 fi],[enable_dmalloc=0])
AC_DEFINE_UNQUOTED([RSB_WANT_DMALLOC],[$enable_dmalloc],[])
dnl
dnl
dnl
AH_TEMPLATE([RSB_WANT_MKL])
AC_ARG_WITH(mkl-include, AC_HELP_STRING([--with-mkl-include], [Specify the MKL (Intel Math Kernel Library) library headers path. e.g.: --with-mkl-include="/opt/intel/mkl/include". ]), [if test "x$withval" = xno; then MKL_INCLUDE= ; else if test "x$withval" = xyes; then MKL_INCLUDE="" ; else MKL_INCLUDE="$withval" ; fi  ; fi], [true] )

AC_ARG_WITH(mkl, AC_HELP_STRING([--with-mkl], [Specify the MKL (Intel Math Kernel Library) library to be used with the benchmarking program. E.g.: --with-mkl="...". Include options should be specified in the MKL_INCLUDE environment variable. ]), [if test "x$withval" = xno; then want_mkl_libs= ; else if test "x$withval" = xyes; then want_mkl_libs="-static -L/opt/intel/mkl/lib/ia32/ -lmkl_solver -Wl,--start-group -lmkl_intel -lmkl_gnu_thread -lmkl_core -Wl,--end-group -fopenmp -lpthread" ; else want_mkl_libs="$withval" ; fi  ; fi], [true] )

dnl
dnl 
dnl
AC_ARG_WITH(zlib, AC_HELP_STRING([--with-zlib], [Specify Z library.  e.g.: --with-zlib="..." for reading gzip-compressed matrix files.]), [if test "x$withval" = xno; then want_zlib_libs="" ; else want_zlib_libs="$withval" ; fi], [want_zlib_libs=""])
AC_CHECK_HEADERS([zlib.h])
want_zlib_support="no"
if test x"$want_zlib_libs" != x"" && test "x$ac_cv_header_zlib_h" = xyes ; then
	dnl use --without-zlib to disable it
      	AC_MSG_NOTICE([Enabling zlib support.])
	AC_DEFINE([RSB_WANT_ZLIB_SUPPORT],[1],[Support for reading gzipped matrices.])
	if test x"$want_zlib_libs" = x"yes" ; then want_zlib_libs=-lz; fi
	LIBS="${LIBS} $want_zlib_libs"
	want_zlib_support="yes"
	else
dnl	AC_MSG_WARN([No zlib headers found.])
	AC_DEFINE([RSB_WANT_ZLIB_SUPPORT],[0],[Support for reading gzipped matrices.])
	want_zlib_support="no"
fi
dnl
dnl 
dnl
AC_ARG_WITH(ompio, AC_HELP_STRING([--with-ompio], [Use OpenMP and fgets_unlocked() for parallel I/O]), [if test "x$withval" = xno; then want_ompio="no" ; else want_ompio="yes" ; fi], [want_ompio="no"])

if test x"$want_ompio" = x"yes" ; then
      	AC_MSG_NOTICE([Enabling OpenMP + fgets_unlocked() IO support.])
	AC_DEFINE([RSB_WANT_OMPIO_SUPPORT],[1],[Support for reading matrices in parallel (Experimental, untested).])
	else
	AC_DEFINE([RSB_WANT_OMPIO_SUPPORT],[0],[Support for reading matrices in parallel (Experimental, untested).])
fi
dnl
dnl 
dnl
AC_CHECK_HEADERS([limits.h], [break], [break])
AC_CHECK_HEADERS([signal.h], [break], [break])
dnl AC_CHECK_HEADERS([bits/sigaction.h], [], [],
dnl [#ifdef RSB_HAVE_SIGNAL_H
dnl # include <signal.h>
dnl #endif
dnl ])
# an AIX specific check
AC_CHECK_HEADERS([sys/systemcfg.h], [break], [break])
AC_DEFINE([RSB_WANT_VERBOSE_MESSAGES],[0],[If set, the library will be much more verbose. Should be enabled for debugging purposes only.])
AC_DEFINE([RSB_WANT_KERNELS_DEBUG],[1],[If set, RSB_WANT_KERNELS_DEBUG will enable comparative consistency checking of the multiplying kernels against a naive, trusted implementation.])
AC_DEFINE([RSB_SORT_IN_PLACE],[0],[If set, sort operations will happen in place.])
dnl AC_DEFINE([RSB_WANT_BLOCK_TRAILING_STRUCT_QUICK],[0],[This flag is still unsupported])
AC_DEFINE([RSB_BLOCK_SMALL_INDICES],[1],[If set, the library will use smaller indices in blocks.])
dnl
dnl
detected_memhinfo=`$srcdir/scripts/linux-sys-cache.sh`;
dnl
AC_ARG_WITH(nounroll-cflag, AC_HELP_STRING([--with-nounroll-cflag], [Specify the no unroll compiler flag (if unset, will be guessed).]),
        [if test "x$withval" = xno; then userset_nounroll_cflag="" ; else userset_nounroll_cflag="$withval" ; fi] , [userset_nounroll_cflag="";])
dnl
default_want_int_verrbosity="0";
AC_ARG_ENABLE(internals-error-verbosity, AC_HELP_STRING([--enable-internals-error-verbosity], [Set error verbosity level of library internal functions (RSB_INT_ERR_VERBOSITY): can be 0 (no printout at all, never), 1 (on error). Use this to debug the library itself. Experimental.]),
        [if test "x$enableval" = xno; then want_int_verrbosity="${default_want_int_verrbosity}" ; else want_int_verrbosity="$enableval" ; fi] , [want_int_verrbosity="${default_want_int_verrbosity}";])
dnl
default_want_ext_verrbosity="0";
AC_ARG_ENABLE(interface-error-verbosity, AC_HELP_STRING([--enable-interface-error-verbosity], [Set error verbosity level of library interface functions (RSB_OUT_ERR_VERBOSITY): can be 0 (no printout), 1 (printout on error, if requested), 2 (printout on error), 99 (exit on error). Use this to debug your program or to better understand the library usage. ]),
        [if test "xenableval" = xno; then want_ext_verrbosity="${default_want_ext_verrbosity}" ; else want_ext_verrbosity="$enableval" ; fi] , [want_ext_verrbosity="${default_want_ext_verrbosity}";])
dnl
default_want_io_level=0;
AC_ARG_ENABLE(io-level, AC_HELP_STRING([--enable-io-level], [Set input/output functionality level (RSB_WANT_IO_LEVEL), a number between 0 and 7, as any sum combination of 1 (standard input/output), 2 (standard error), 4 (arbitrary descriptors). (experimental).]),
        [if test "x$enableval" = xno; then want_io_level="${default_want_io_level}" ; else want_io_level="$enableval" ; fi] , [want_io_level="7";])
dnl
AC_ARG_WITH(max-threads, AC_HELP_STRING([--with-max-threads], [Maximal number of supported threads (default 64).]),
        [if test "x$withval" = xno; then want_max_threads="64" ; want_max_threads=64 ; else want_max_threads="$withval" ; fi] , [want_max_threads="64";])
dnl
AC_ARG_WITH(memhinfo, AC_HELP_STRING([--with-memhinfo], [Compile with user specified memory hierarchy information, which can be overridden by runtime detection and runtime read of RSB_USER_SET_MEM_HIERARCHY_INFO environment variable.]),
        [if test "x$withval" = xno; then memhinfo="" ; openmp_flags= ; else memhinfo="$withval" ; fi] , [memhinfo="";])
if test "x${detected_memhinfo}" = x -a "x${memhinfo}" = x; then
  FALLBACK_MEM_HIERARCHY_INFO='L2:4/64/512K,L1:8/64/24K'
  AC_MSG_WARN([Failed detecting memory hierarchy info: setting to fallback value of ${FALLBACK_MEM_HIERARCHY_INFO}. If you know it, consider setting it via e.g. --with-memhinfo=${FALLBACK_MEM_HIERARCHY_INFO} ."])
  memhinfo="${FALLBACK_MEM_HIERARCHY_INFO}"
fi
AC_SUBST([RSB_USER_SET_MEM_HIERARCHY_INFO],"${memhinfo}")
AC_SUBST([RSB_DETECTED_MEM_HIERARCHY_INFO],"${detected_memhinfo}")
dnl AC_DEFINE([RSB_USER_SET_MEM_HIERARCHY_INFO],[$memhinfo],[If not null, the library will rely on this for memory hierarchy info.])
dnl
dnl
AC_ARG_WITH(ar, AC_HELP_STRING([--with-ar], [Specify the library archiver program explicitly.]), [if test "x$withval" = xno; then true ; else AR="$withval" ; fi] , [ AR="$AR" ] )
AC_ARG_WITH(arflags, AC_HELP_STRING([--with-arflags], [Specify the library archiver program flags explicitly.]), [if test "x$withval" = xno; then true ; else ARFLAGS="$withval" ; fi] , [ ARFLAGS="$ARFLAGS" ] )
AC_ARG_WITH(m4, AC_HELP_STRING([--with-m4], [Specify the M4 preprocessor program explicitly.]), [if test "x$withval" = xno; then true ; else M4="$withval" ; fi] , [true;] )
dnl
m4_if(m4_version_compare(m4_defn([AC_AUTOCONF_VERSION]),[2.60]),-1, [
AC_ARG_WITH(openmp, AC_HELP_STRING([--with-openmp], [Use the OpenMP multithreading mechanisms.]),
        [if test "x$withval" = xno; then enable_openmp=no ; openmp_flags= ; else openmp_flags="$withval" ; enable_openmp=yes ; fi] , [enable_openmp=yes])
],[])
dnl
if test "x$enable_openmp" = x ; then
	enable_openmp=yes
fi
dnl
if test "x$enable_openmp" != x"yes" && test "x$want_ompio" = x"yes"; then
	AC_MSG_ERROR([You must enable OpenMP if you want OpenMP-backed I/O!])
fi
dnl
#dnl	***********************************************************************
dnl define(DEFAULT_MATRIX_OPS,[spmv_uaua,spmv_sxsa,spmv_uxua,infty_norm,rowssums,spmv_sasa,spsv_sxsx,spsv_uxua])
define(DEFAULT_MATRIX_OPS,[spmv_uaua,spmv_uauz,spmv_uxua,spmv_unua,spmv_sasa,spsv_uxua,spmv_sxsa,spsv_sxsx,infty_norm,rowssums,scale])
dnl define(DEFAULT_MATRIX_OPS,[spmv_uaua,spmv_uauz,spmv_uxua,spmv_unua,spmv_sasa,spmv_sxsa,infty_norm,rowssums,scale])
dnl define(DEFAULT_UNROLLS,[1,2,4])
define(DEFAULT_UNROLLS,[1])
define(RSB_M4_SIMPLE_LOOP_UNROLL_DEFAULT_FACTOR,[16])
dnl define(DEFAULT_TYPES,[int,double])
define(DEFAULT_TYPES,["double,float,float complex,double complex"])
#dnl	***********************************************************************
# the default block unrolls
default_unrolls=DEFAULT_UNROLLS
default_util_unrolls=RSB_M4_SIMPLE_LOOP_UNROLL_DEFAULT_FACTOR
# the default types for macro-generated code
blas_matrix_types="double,float,float complex,double complex"
psblas_matrix_types="${blas_matrix_types}"
non_blas_matrix_types="int"
all_matrix_types="$non_blas_matrix_types,$blas_matrix_types"
#default_types=int,double,float,float complex, double complex
# float complex and double complex are c99 types
default_types=DEFAULT_TYPES
# the default matrix operations
blas_matrix_ops=spmv_uaua,spmv_uauz,spmv_uxua,spmv_unua,spmv_sasa,spsv_uxua,spmv_sxsa,spsv_sxsx
dnl
psblas_matrix_ops="${blas_matrix_ops}",infty_norm,rowssums,scale
dnl
dnl extra_blas_matrix_ops=,spmv_sxsx,spmv_uxux
extra_blas_matrix_ops= # 20140719 these kernels are not active at the moment
#
non_blas_matrix_ops=infty_norm,rowssums,scale
dnl non_blas_matrix_ops=...spmm_az # 20140719 this kernel is not active at the moment
all_matrix_ops="$blas_matrix_ops,$non_blas_matrix_ops$extra_blas_matrix_ops"
#
default_matrix_ops=DEFAULT_MATRIX_OPS
#dnl	***********************************************************************
dnl AC_ARG_WITH(block-unrolls, AC_HELP_STRING([--with-block-unrolls], [Generate unrolled kernels for blocks with specified columns/rows (default:DEFAULT_UNROLLS)(experimental)]), [default_unrolls="$withval"],[default_unrolls="$default_unrolls"])
#dnl	***********************************************************************
dnl AC_ARG_WITH(row-unrolls, AC_HELP_STRING([--with-row-unrolls], [Generate unrolled kernels for blocks with specified rows (default:DEFAULT_UNROLLS)(experimental)]), [row_unrolls="$withval"],[row_unrolls="$default_unrolls"])
row_unrolls="$default_unrolls"
#dnl	***********************************************************************
dnl AC_ARG_WITH(column-unrolls, AC_HELP_STRING([--with-column-unrolls], [Generate unrolled kernels for blocks with specified columns (default:DEFAULT_UNROLLS) (experimental)]), [column_unrolls="$withval"],[column_unrolls="$default_unrolls"])
column_unrolls="$default_unrolls"
#dnl	***********************************************************************
AC_DEFINE([RSB_WANT_SPARSE_BLAS_LEVEL_1],[1],[If set, a reference, unoptimized Sparse BLAS Level 1 interface will be functional.])
#dnl	***********************************************************************
AC_ARG_ENABLE(matrix-types, AC_HELP_STRING([--enable-matrix-types], [Generate kernels for specified types (default:DEFAULT_TYPES) (you can specify 'all' to get all of them, or 'blas' for Sparse BLAS ones)]), [want_matrix_types="$enableval"],[want_matrix_types="$default_types"])
#dnl	***********************************************************************
AC_ARG_ENABLE(matrix-ops, AC_HELP_STRING([--enable-matrix-ops], [Generate kernels for specified matrix operations (default:DEFAULT_MATRIX_OPS) (you can specify 'all' to get all of them, or 'blas' for only ones for Sparse BLAS, or 'psblas' for only ones for PSBLAS) (Experimental, the default "all" is recommended.)]), [want_matrix_ops="$enableval"],[want_matrix_ops="$default_matrix_ops"])
#dnl	***********************************************************************
AC_ARG_ENABLE(vector-utils-loop-unrolls, AC_HELP_STRING([--enable-vector-utils-loop-unrolls], [Loop unrolling of generated vector utility functions (default:RSB_M4_SIMPLE_LOOP_UNROLL_DEFAULT_FACTOR)]), [util_unrolls="$enableval"],[util_unrolls="$default_util_unrolls"])
#dnl	***********************************************************************
AC_ARG_ENABLE(octave-testing, AC_HELP_STRING([--enable-octave-testing], [Enabling GNU Octave based testing.]),
[if test "x$enableval" = xno; then
	enable_octave_testing=no
 else 
	enable_octave_testing=yes
 fi],[enable_octave_testing=yes])
#dnl	***********************************************************************
if test "x$want_matrix_types" = xall; then
	AC_MSG_NOTICE([Enabling all matrix types.])
	want_matrix_types="${all_matrix_types}";
else 
	true;
	if test "x$want_matrix_types" = x"blas"; then
		AC_MSG_NOTICE([Enabling all matrix types for Sparse BLAS (S,C,D,Z).])
		want_matrix_types="${blas_matrix_types}";
		#if test "x$enable_octave_testing" = xyes; then want_matrix_types=${want_matrix_types},int ; fi
	else 
	    if test "x$want_matrix_types" = x"psblas"; then
		AC_MSG_NOTICE([Enabling matrix types for Parallel Sparse BLAS (PSBLAS).])
		want_matrix_types="${psblas_matrix_types}";
		#if test "x$enable_octave_testing" = xyes; then want_matrix_types=${want_matrix_types},int ; fi
	    else
		true;
	    fi 
	fi
fi
#dnl	***********************************************************************
if test "x$want_matrix_ops" = xall; then
	AC_MSG_NOTICE([Enabling all of the matrix ops.])
	want_matrix_ops="${all_matrix_ops}";
else 
	if test "x$want_matrix_ops" = xblas; then
		AC_MSG_NOTICE([Enabling matrix ops for Sparse BLAS.])
		want_matrix_ops="${blas_matrix_ops}";
	else
	    if test "x$want_matrix_ops" = xpsblas; then
		AC_MSG_NOTICE([Enabling matrix ops for Parallel Sparse BLAS (PSBLAS).])
		want_matrix_ops="${psblas_matrix_ops}";
	    else
		true;
	    fi 
	fi
fi
#dnl	***********************************************************************
if test x"$want_matrix_types" != x"$default_types" -o x"$want_matrix_ops" != x"$default_matrix_ops" -o x"$util_unrolls" != x"$default_util_unrolls"; then
if test x"$M4" = x"false"; then
	AC_MSG_ERROR([Did not specify an m4 processor, so code generation from m4 files is disabled (and so configure time specification of non default types, operations, unrolls) !])
fi
fi
#dnl	***********************************************************************
sparse_blas_interface_default=yes
AC_ARG_ENABLE(sparse-blas-interface, AC_HELP_STRING([--disable-sparse-blas-interface], [Build a Sparse BLAS interface to librsb.]),
[if test "x$enableval" = xno; then
	sparse_blas_interface=no
 else 
	sparse_blas_interface=yes
 fi],[sparse_blas_interface="${sparse_blas_interface_default}"])
#dnl	***********************************************************************
enable_looping_kernels=no
AH_TEMPLATE([RSB_WANT_LOOPING_KERNELS])
#dnl	***********************************************************************
AC_ARG_WITH(oski, AC_HELP_STRING([--with-oski], [OSKI comparative benchmarking (WARNING: be sure to set 
	OSKI_INCLUDE, OSKI_LUA_PATH, OSKI_PATH environment variables first). UNFINISHED.]),
[if test "x$withval" = xno; then
	enable_oski=no
 else 
	enable_oski=yes
 fi],[enable_oski=no])
#dnl	***********************************************************************
#AC_ARG_WITH(papi, AC_HELP_STRING([--with-papi], [PAPI (Performance Application Programming Interface). UNFINISHED.]),
#[if test "x$withval" = xno; then
#	enable_papi=no
# else 
#	enable_papi=yes
# fi],[enable_papi=no])
#dnl	***********************************************************************
AH_TEMPLATE([RSB_WITH_LIKWID])
AC_ARG_WITH(likwid, AC_HELP_STRING([--with-likwid], [LIKWID support (will add the LIKWID_LIBS variable to LIBS and LIKWID_CFLAGS to CFLAGS). UNFINISHED.]),
[if test "x$withval" = xno; then
	enable_likwid=no
 else 
	enable_likwid=yes
 fi],[enable_likwid=no])
#dnl	***********************************************************************
AC_ARG_ENABLE(allocator-wrapper, AC_HELP_STRING([--enable-allocator-wrapper], [If enabled, librsb will keep count of internal memory allocations via a allocator functions wrappers.]),
[if test "x$enableval" = xno; then
	disable_allocator_wrapper=yes
 else 
	disable_allocator_wrapper=no
 fi],[disable_allocator_wrapper=yes])
#dnl	***********************************************************************
AC_ARG_ENABLE(alignment, AC_HELP_STRING([--disable-alignment], [By default, we allocate aligned memory. This can be disabled.]),
[if test "x$enableval" = xno; then
	enable_alignment=no
 else 
	enable_alignment=yes
 fi],[enable_alignment=yes])
enable_b=yes
enable_c=yes
#dnl	***********************************************************************
AC_ARG_ENABLE(librsb-stats, AC_HELP_STRING([--enable-librsb-stats], [If enabled, will allow collection of time statistics in librsb operations.]),
[if test "x$enableval" = xno; then
	enable_librsb_stats=no
 else 
	enable_librsb_stats=yes
 fi],[enable_librsb_stats=no])
#dnl	***********************************************************************
AC_ARG_ENABLE(rsb-num-threads, AC_HELP_STRING([--enable-rsb-num-threads], [RSB_NUM_THREADS environment variable to control number of threads (Experimental, with effect on rsb_spmv/rsb_spmm).]),
[if test "x$enableval" = xno; then
	enable_rsb_num_threads=no
 else 
	enable_rsb_num_threads=yes
 fi],[enable_rsb_num_threads=no])
#dnl	***********************************************************************
dnl AC_ARG_ENABLE(fortran-interface, AC_HELP_STRING([--disable-fortran-interface], [Fortran interface.]),
dnl [if test "x$enableval" = xno; then
dnl	enable_fortran=no
dnl else 
dnl 	enable_fortran=yes
dnl  fi],[enable_fortran=yes])
#dnl	***********************************************************************
AC_ARG_ENABLE(fortran-module-install, AC_HELP_STRING([--enable-fortran-module-install], [Install (compiler specific) Fortran module (blas_sparse.mod) (experimental).]),
[if test "x$enableval" = xno; then
	want_blas_sparse_mod_install=no
 else 
	want_blas_sparse_mod_install=yes
 fi],[want_blas_sparse_mod_install=no])
#dnl	***********************************************************************
want_install_pkg_config_default=no
AC_ARG_ENABLE(pkg-config-install, AC_HELP_STRING([--enable-pkg-config-install], [Install pkg-config file (librsb.pc) installation.]),
[if test "x$enableval" = x"yes"; then
	want_install_pkg_config=yes
	AC_MSG_NOTICE([Will install pkg-config librsb.pc file.])
 else 
	want_install_pkg_config=yes
	AC_MSG_NOTICE([Will not install pkg-config librsb.pc file (--enable-pkg-config-install to change).])
 fi],[want_install_pkg_config="${want_install_pkg_config_default}"])
#dnl	***********************************************************************
dnl AC_ARG_ENABLE(fortran-blas-sparse-header-install, AC_HELP_STRING([--enable-fortran-blas-sparse-header-install], [Install (non standard) Sparse BLAS Fortran header (blas_sparse.fi) (experimental).]),
dnl [if test "x$enableval" = xno; then
dnl dnl 	want_blas_sparse_fi_install=no
dnl  else 
dnl 	want_blas_sparse_fi_install=yes
dnl fi],[want_blas_sparse_fi_install=no])
dnl want_blas_sparse_fi_install="${sparse_blas_interface}"
#dnl	***********************************************************************
AC_ARG_ENABLE(doc-build, AC_HELP_STRING([--enable-doc-build], [If doxygen is detected or supplied (DOXYGEN environment variable), documentation will be rebuilt. If 'help2man' (HELP2MAN) is also present, it will be used to build additional man pages.]),
[if test "x$enableval" = xno; then
	want_build_doc=no
 else 
	want_build_doc=yes
 fi],[want_build_doc=no])
#dnl	***********************************************************************
AC_ARG_ENABLE(shlib-linked-examples, AC_HELP_STRING([--enable-shlib-linked-examples], [Shared library based examples (experimental: developer only).]),
[if test "x$enableval" = xno; then
	want_rsb_dl=no
 else 
	want_rsb_dl=yes
 fi],[want_rsb_dl=no])
#dnl	***********************************************************************
AC_ARG_ENABLE(c-examples, AC_HELP_STRING([--disable-c-examples], [C example programs building.]),
[if test "x$enableval" = xno; then
	enable_c_examples=no
 else 
	enable_c_examples=yes
 fi],[enable_c_examples=yes])
#dnl	***********************************************************************
AC_ARG_ENABLE(fortran-examples, AC_HELP_STRING([--disable-fortran-examples], [Fortran test and example programs generation and building. (experimental)]),
[if test "x$enableval" = xno; then
	enable_fortran_examples=no
 else 
	enable_fortran_examples=yes
 fi],[enable_fortran_examples=yes])
#dnl	***********************************************************************
AC_C_RESTRICT()
AC_ARG_ENABLE(restrict, AC_HELP_STRING([--disable-restrict], [Use the restrict keyword.]),
[if test "x$enableval" = xno; then
	enable_restrict=no
 else 
	enable_restrict=yes
 fi],[enable_restrict=yes])
#dnl	***********************************************************************
AC_ARG_WITH(c99-flag, AC_HELP_STRING([--with-c99-flag], [Add the -std=c99 compilation flag to CFLAGS.]),
[if test "x$withval" = xno; then
	enable_c99=no
 else 
	enable_c99=yes
 fi],[enable_c99=yes])
#dnl	***********************************************************************
want_spsm_diagonal_check_default=yes
AC_ARG_ENABLE(zero-division-checks-on-solve, AC_HELP_STRING([--enable-zero-division-checks-on-solve], [Prevents zero-division when performing triangular solution.]),
[if test "x$enableval" = xyes; then
	want_spsm_diagonal_check=yes
 else 
	want_spsm_diagonal_check=no
 fi],[want_spsm_diagonal_check="${want_spsm_diagonal_check_default}"])
#dnl	***********************************************************************
want_sigaction_in_rsbench=no
AC_ARG_ENABLE(sigaction-interruptible-rsbench, AC_HELP_STRING([--enable-sigaction-interruptible-rsbench], [rsbench will be interruptible using sigaction (breaks the standard: may break the build.).]),
[if test "x$enableval" = xyes; then
	want_sigaction_in_rsbench=yes;
 else 
	want_sigaction_in_rsbench=no;
 fi],[want_sigaction_in_rsbench=no;])
if test x$want_sigaction_in_rsbench = xno ; then
	AC_DEFINE([RSB_WANT_ACTION_SIGNAL],[1],[experimental.])
fi
#dnl	***********************************************************************
enable_optimize=no
#dnl	***********************************************************************
AC_ARG_ENABLE(internal-headers-install, AC_HELP_STRING([--enable-internal-headers-install], [Install internal headers (only for debugging / inspection purposes, not for ordinary users).]),
[if test "x$enableval" = xyes; then
 	enable_ihi=yes;
 	AC_MSG_NOTICE([Will install also internal headers.])
else 
 	enable_ihi=no;
fi],[enable_ihi=no])
#dnl	***********************************************************************
RSB_USE_ASSERT="";
AC_ARG_ENABLE(debug-getenvs, AC_HELP_STRING([--enable-debug-getenvs], [Enable (undocumented) developer oriented getenv-based controls.]),
[if test "x$enableval" = xyes; then
	RSB_USE_ASSERT=1;
	AC_DEFINE([RSB_ALLOW_INTERNAL_GETENVS],[1],[Extra (undocumented) developer oriented control switches.])
else 
	AC_DEFINE([RSB_ALLOW_INTERNAL_GETENVS],[0],[Extra (undocumented) developer oriented control switches.])
fi],[true;])
#dnl	***********************************************************************
RSB_USE_ASSERT="";
AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug], [Compile with debug flags and enable assertions and other internals. This will slow down the code considerably.]),
[if test "x$enableval" = xyes; then
	enable_debug=yes;
	RSB_USE_ASSERT=1;
dnl
	want_int_verrbosity=1; # FIXME: this shall be removed from here, once the library gets stable for release!
	AC_MSG_NOTICE([With the debug switch enabled, also setting internal error verbosity level at value 1.])
dnl
else 
	enable_debug=no
fi],[enable_debug=no])
#dnl	***********************************************************************
AH_TEMPLATE([RSB_WITH_SPARSE_BLAS_INTERFACE])
if test "x$sparse_blas_interface" = xyes; then
	AC_MSG_NOTICE(Will build a Sparse BLAS interface to librsb.)
	AC_DEFINE([RSB_WITH_SPARSE_BLAS_INTERFACE],[1],[Sparse BLAS interface compilation.])
else 
      	AC_MSG_NOTICE([Will not build a Sparse BLAS interface to librsb.])
fi
#dnl	***********************************************************************
want_looping_kernels=0;
if test "x$enable_looping_kernels" = xyes; then
	AC_MSG_NOTICE(Enabling looping kernels.)
	want_looping_kernels=1;
	AC_DEFINE([RSB_WANT_LOOPING_KERNELS],[1],[Looping kernels.])


else 
dnl      	AC_MSG_NOTICE([Skipping the compilation of looping kernels.])
	true
fi
#dnl	***********************************************************************
AC_ARG_WITH(papi, AC_HELP_STRING([--with-papi], [Specify the PAPI library (UNFINISHED)]), [if test "x$withval" = xno; then want_papi_libs= ; else enable_papi=yes; want_papi_libs="$withval" ; fi], [want_papi_libs="-lpapi"])
#dnl	***********************************************************************
if test "x$enable_papi" = xyes; then
	if test "x$ac_cv_header_papi_h" != xyes; then
	AC_MSG_WARN([Header file <papi.h> not found, therefore we will not use it!])
	else
	if test "$want_papi_libs" = yes ; then want_papi_libs=-lpapi ; fi
	RSB_RSBENCH_LIBS="${RSB_RSBENCH_LIBS} $want_papi_libs"
	# TODO : set CFLAGS !
      	AC_MSG_NOTICE(Enabling PAPI (Performance Application Programming Interface).)
	# FIXME: should differentiate RSB_WANT_PERFORMANCE_COUNTERS from RSB_HAVE_PAPI
	AC_DEFINE([RSB_WANT_PERFORMANCE_COUNTERS],[1],[Performance Counters.])
	AC_DEFINE([RSB_HAVE_PAPI],[1],[Performance Application Programming Interface.])
	fi
else 
      	dnl AC_MSG_NOTICE(Skipping the use of PAPI (Performance Application Programming Interface).)
	true
fi
#dnl	***********************************************************************
if test "x$enable_likwid" = xyes; then
	if test "x$LIKWID_LIBS" = x; then
		LIKWID_LIBS="-llikwid"
	fi
  	AC_MSG_NOTICE(Enabling support for LIKWID (LIKWID_CFLAGS=${LIKWID_CFLAGS}) (LIKWID_LIBS=${LIKWID_LIBS}).)
	RSB_RSBENCH_LIBS="${RSB_RSBENCH_LIBS} ${LIKWID_LIBS}"
	RSB_RSBENCH_CFLAGS="${RSB_RSBENCH_CFLAGS} ${LIKWID_CFLAGS}"
	AC_DEFINE([RSB_WITH_LIKWID],[1],[LIKWID marker API support.])
else
	AC_DEFINE([RSB_WITH_LIKWID],[0],[LIKWID marker API support.])
fi
#dnl	***********************************************************************
if test "x$enable_hwloc" = xyes; then
	if test "x$want_hwloc_libs" != x; then
		HWLOC_LIBS="$want_hwloc_libs"
	fi
	AC_MSG_NOTICE(Enabling support for HWLOC (HWLOC_CFLAGS=${HWLOC_CFLAGS}) (HWLOC_LIBS=${HWLOC_LIBS}).)
	RSB_RSBENCH_LIBS="${RSB_RSBENCH_LIBS} ${HWLOC_LIBS}"
	RSB_RSBENCH_CFLAGS="${RSB_RSBENCH_CFLAGS} ${HWLOC_CFLAGS}"
	AC_DEFINE([RSB_WITH_HWLOC],[1],[HWLOC API support.])
else
	AC_DEFINE([RSB_WITH_HWLOC],[0],[HWLOC API support.])
fi
#dnl	***********************************************************************
if test "x$disable_allocator_wrapper" = xyes; then
AC_DEFINE([RSB_DISABLE_ALLOCATOR_WRAPPER],1,[If defined, will not account for internally used memory.])
      AC_DEFINE([RSB_WANT_ALLOCATOR_LIMITS],0,[If 1, will allow the user to set hard limits to the memory allocated by librsb. Trespass attempts will fail.])
      AC_MSG_NOTICE(Will disable memory allocators wrappers.)
else 
      AC_DEFINE([RSB_WANT_ALLOCATOR_LIMITS],1,[If 1, will allow the user to set hard limits to the memory allocated by librsb. Trespass attempts will fail.])
      AC_MSG_NOTICE(Enabling memory allocators wrappers.)
fi
#dnl	***********************************************************************
#dnl	***********************************************************************
if test "x$enable_alignment" = xno; then
      AC_MSG_NOTICE(Will not enforce aligned memory chunks allocation.)
else 
      AC_MSG_NOTICE(Will enforce aligned memory chunks allocation.)
AC_DEFINE([RSB_WANT_DOUBLE_ALIGNED],[1],[On some architectures (notably modern Intel), floating point computations on non double aligned data make loose some clock cycle.])
fi
#dnl	***********************************************************************
if test "x$enable_librsb_stats" = xyes; then
	AC_MSG_NOTICE(Enabling collection of time statistics in librsb operations (this introduces an overhead).)
	AC_DEFINE([RSB_WANT_LIBRSB_STATS],[1],[Enabling collection of time statistics in librsb operations (this introduces an overhead).])
fi
#dnl	***********************************************************************
if test "x$enable_rsb_num_threads" = xyes; then
	AC_MSG_NOTICE(Enabling experimental RSB_NUM_THREADS environment variable.)
	AC_DEFINE([RSB_WANT_RSB_NUM_THREADS],[1],[Enabling experimental RSB_NUM_THREADS environment variable.])
fi
#dnl	***********************************************************************
dnl if test x$enable_fortran = xno; then
dnl       AC_MSG_NOTICE(Will not build Fortran bindings.)
dnl else 
dnl       AC_MSG_NOTICE(Will build Fortran bindings.)
dnl       AC_DEFINE([RSB_WANT_EXPERIMENTAL_FORTRAN_INTERFACE],[1],[Fortran interface.])
dnl fi
#dnl	***********************************************************************
if test "x$enable_c_examples" = xno; then
      AC_MSG_NOTICE(Will not build C examples.)
else 
      AC_MSG_NOTICE(Will build C examples.)
fi
#dnl	***********************************************************************
if test "x$enable_fortran_examples" = xno; then
      AC_MSG_NOTICE(Will not build Fortran examples.)
else 
      AC_MSG_NOTICE(Will build (experimental) Fortran examples.)
fi
#dnl	***********************************************************************
if test "x$enable_restrict" = xyes; then
      AC_MSG_NOTICE([Will use the C99 restrict keyword.])
      AC_MSG_NOTICE([Will also add the -std=c99 flag.])
      enable_c99=yes
else 
      AC_MSG_NOTICE([Will not use the C99 restrict keyword ])
fi
#dnl	***********************************************************************
if test "x$want_build_doc" = xyes ; then
      if test x"$DOXYGEN" = x"false"; then
	      AC_MSG_ERROR([Doxygen not detected ! Please --disable-doc-build or supply a valid DOXYGEN variable.])
	      want_build_doc=no
      else
	      AC_MSG_NOTICE([Will rebuild the documentation using "$DOXYGEN" as Doxygen executable.])
	      if test x"$HELP2MAN" = x"false"; then
      	   	AC_MSG_NOTICE([Program man pages will not generated: HELPMAN not detected.])
	      fi
      fi
else 
	AC_MSG_NOTICE([Will not use Doxygen to build documentation (--enable-doc-build to change).])
fi
#dnl	***********************************************************************
no_unroll_flags=""
#dnl	***********************************************************************
if test xyes = xyes; then # we need a new flag here for this : FIXME
	# I am not sure whether these flags are optimal, but among these they are
	
	if test "x${CC}" = x"xlc" -o "x${CC}" = x"xlc_r"  && test "x$spigni_forte" = "x" ; then
		# use -qnostrict to turn  off aggressive optimization (debug cases)
		# use -q64 to enable 64 bit compilation and ar -X 64 cru ... for linking (FIXME)
          	# -qfdpr
          	# The compiler generates additional symbol information for use by the AIX "fdprpro" code optimizer.
		# /opt/freeware/bin path is harmful with autotools on the ENEA grid environment, as it is the default one!
		PATH="/bin/:$PATH"

		# the following should only be used along with -q64. not without! (FIXME)
		ARFLAGS="-X 64 cru"
		SPCFLAGS="-q 64"

		#spigni_forte="-O3 -lmass -lessl"
		spigni_forte="-O3 -lmass -lessl -q64 -bmaxdata:0x1000000000"
		#spigni_forte="-O3 -lmass -lessl -q64 -bmaxdata:0x70000000"
		# FIXME : configure is not smart enough to add -X64 to ARFLAGS
		# FIXME : CXXFLAGS too
#		spigni_forte="-O3 -lmass -lessl"
		restrict_flags="-qkeyword=restrict"
		c99_flags="-qlanglvl=extc99 $restrict_flags"
		debug_flags="-O0 -g"
		openmp_flags="-qsmp=omp"
		if test -f /bin/uname ; then
			# some AFS systems (e.g.: ENEA.it grid) need this fix
			uname_M="`/bin/uname -M`"
		else
			uname_M="`uname -M`"
		fi
		if test "x${uname_M}" = x"IBM,9118-575" ; then
			spigni_forte="$spigni_forte -qarch=pwr5 -qtune=pwr5"
		fi
		if test "x${uname_M}" = x"IBM,7040-681" ; then
			spigni_forte="$spigni_forte -qarch=pwr4 -qtune=pwr4"
		fi
		# verbose : 0-3
		# profiling info: -pga
		# -qbrowse 
		no_unroll_flags="-qunroll=no"
		# xlc has #pragma unroll !
		AC_MSG_NOTICE([Guessing the compiler is xlc.])
	fi

        have_icc=no; # a fix to set correctly openmp_flags
        if test "x${CC}" = x"icc" || ${CC} -V 2>&1 | grep Intel ; then have_icc=yes ; fi

        if test "x${have_icc}" = x"yes" && test "x$spigni_forte" = "x" ; then
#	if test "x${CC}" = x"icc" && test "x$spigni_forte" = "x" ; then
		spigni_forte="-O3 -xSSE3  -no-alias-const -no-multibyte-chars -pipe "
		# note: -tpp6 & -tpp7 and so on are old icc flags (version 11 does not support them)
		# ipo seems to break autotools
		# -xS ?
		# TODO : '-ax:SSE2'  .. generate multiple paths ..

		# -ax turns on the vectorizer (MMX, SSEx, ...)
		# -mtune=..
		restrict_flags="-restrict" # !
		c99_flags="$restrict_flags"
		debug_flags="-O0 -g"
		#no_unroll_flags="-fno-unroll"
		#20110608 icc v12 wants -unroll=0
		no_unroll_flags="-unroll=0"
		#openmp_flags="-openmp" # -parallel
		#openmp_flags="-qopenmp" # -parallel
		openmp_flags="-fopenmp" # -parallel
		AC_MSG_NOTICE([Guessing the compiler is icc.])
		walls="-Wall"
	fi

	if test "x${CC}" = x"pgcc" && test "x$spigni_forte" = "x" ; then
		spigni_forte="-O3 -Mvect=cachesize:automatic,fuse,prefetch,sse -Mquad -Mscalarsse -Mnoframe -Minfo=all" # O3 is same as 4 
		c99_flags="-c99 -Xa"
		restrict_flags="$c99_flags" # !
		debug_flags="-O0 -g"
		no_unroll_flags="-Mnounroll"
		openmp_flags="-mp"
		# -Mconcur is VERY interesting .. !!
		# -Mlist (creates a listing file)
		# -Mprof=hwcts     Use PAPI-based profiling with hardware counters (linux86-64 only).
		# -pg exists, -g too
		# -Mnovect disables the vectorizer, and is the default
		AC_MSG_NOTICE([Guessing the compiler is pgcc.])
	fi

dnl	AC_MSG_NOTICE([Guessing compiler best flags...])
	if test "x$ac_cv_c_compiler_gnu" = xyes && test "x$spigni_forte" = "x" ; then
		# note that CC=icc will not imply ac_cv_c_compiler_gnu=yes !
		# -malign-double does not make sense on 64 bit archs and triggers errors
		#spigni_forte="-O3 -fomit-frame-pointer -ffast-math"
		spigni_forte="-O3 -fomit-frame-pointer -mtune=native"
		c99_flags="-std=c99" # ?
		restrict_flags="$c99_flags" # !
		debug_flags="-O0 -ggdb"
		no_unroll_flags="-fno-unroll-loops"
		openmp_flags="-fopenmp"
		if test x != x"${OPENMP_CFLAGS}" ; then
			openmp_flags="${OPENMP_CFLAGS}"
		fi
		# NOTE: -ffast-math disables math functions specifications, and therefore is EVIL
		spigni_nativo="-pipe"
		cpuinfomn=`cat /proc/cpuinfo| grep model.name | sed s/^.*://g`
		# FIXME : the following will fail on tcsh
#		if test x"` $CC -v 2>&1| grep -i red.*hat`" != x ; then
		gcc_v=`$CC --version` # will be catched on tcsh
		if test x"` $CC -v 2>&1 | grep -i red.*hat`" != x -o x"`echo $gcc_v` | grep -i red.hat" != x; then
			# uhm..
#			if test x"` echo $cpuinfomn | grep Athlon `" != x ; then
#				# fails for a 
#				# model name      : AMD Athlon(tm) 7750 Dual-Core Processor
#				spigni_nativo="$spigni_nativo -march=athlon -mtune=athlon"
#			fi
			if test x"` echo $cpuinfomn | grep 'AMD Opteron.*2216\>'`" != x ; then
				# model name      : Dual-Core AMD Opteron(tm) Processor 2216
				spigni_nativo="$spigni_nativo -march=opteron -mtune=opteron"
			fi
			if test x"` echo $cpuinfomn | grep 'AMD Opteron.*2352\>'`" != x ; then
				# Opteron barcelona are 2344-2350, but the instruction set is ok
				# model name      : AMD Athlon(tm) 7750 Dual-Core Processor
				spigni_nativo="$spigni_nativo -march=barcelona -mtune=barcelona"
			fi
			if test x"` echo $cpuinfomn | grep 'AMD Athlon.*7750'`" != x ; then
				# this is Phenom, not Opteron arcelona, but same instruction set
				# model name      : AMD Athlon(tm) 7750 Dual-Core Processor
				spigni_nativo="$spigni_nativo -march=barcelona -mtune=barcelona"
			fi
			if test x"` echo $cpuinfomn | grep 'AMD Athlon.*64.*X2.*Dual Core Processor 6000.'`" != x ; then
                        	# K9 microarchitecture
				# this is Windsor, released May 24, 2006
				# rossini.ibspan.waw.pl
				# model name      : AMD Athlon(tm) 64 X2 Dual Core Processor 6000+
	                        spigni_nativo="$spigni_nativo -march=amdfam10 -mtune=amdfam10"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*[EXWL]3...\>'`" != x ; then
				# Wolfdale 	31..
				# Kentsfield 	32..
				# Yorkfield 	33..
				# Lynnfield 	34..
				# Bloomfield	35..
				spigni_nativo="$spigni_nativo -march=core2 -mtune=core2"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*X7...'`" != x ; then
				# Tigerton series, d.c.	72..
				# Tigerton series, q.c.	73..	1066MT/s
				# cresco1x .. portici.enea.it
				# model name      : Intel(R) Xeon(R) CPU           X7350  @ 2.93GHz
				# Tulsa series		71..
				# crescobf.brindisi.enea.it:
				# model name      : Intel(R) Xeon(R) CPU           X7350  @ 2.93GHz
				spigni_nativo="$spigni_nativo -march=core2 -mtune=core2"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*[EXWL]70..\>'`" != x ; then
				# Paxville (Netburst)
				spigni_nativo="$spigni_nativo -march=pentium4 -mtune=pentium4"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*[EXWL]50..\>'`" != x ; then
				# Dempsey (Netburst)
				spigni_nativo="$spigni_nativo -march=pentium4 -mtune=pentium4"
			fi
			if test x"` echo $cpuinfomn | grep 'Core(TM)2 Quad CPU'`" != x ; then
				# Conroe/Allendale
				spigni_nativo="$spigni_nativo -march=core2 -mtune=core2"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*[EXWL]51..\>'`" != x ; then
				# Woodcrest (Core2)
				spigni_nativo="$spigni_nativo -march=core2 -mtune=core2"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*[EXWL]52..\>'`" != x ; then
				# Wolfdale DP
				spigni_nativo="$spigni_nativo -march=core2 -mtune=core2"
			fi
			if test x"` echo $cpuinfomn | grep 'Xeon.*[EXWL]53..\>'`" != x ; then
				# Clovertown series, 1333MT/s, 2x4MB L2
				# ce1-cresco.portici.enea.it
				# model name     : Intel(R) Xeon(R) CPU           E5335  @ 2.00GHz
				# Clovertown series, 1333MT/s
				# cresco2-f3.portici.enea.it
				# model name      : Intel(R) Xeon(R) CPU           E5345  @ 2.33GHz
				# Harpertown series	54..	, 12 MB L2
				# Gainestown (Nehalem)s.55..	4x256kB L2, 8MB L3
				spigni_nativo="$spigni_nativo -march=core2 -mtune=core2"
			fi
#			if test x"` echo $cpuinfomn | grep Opteron `" != x ; then
#				spigni_nativo="$spigni_nativo -march=opteron -mtune=opteron"
#			fi
			if test x"` echo $cpuinfomn | grep 'Pentium(R).4' `" != x ; then
				spigni_nativo="$spigni_nativo -march=pentium4 -mtune=pentium4"
			fi
			if test x"` echo $cpuinfomn | grep 'Pentium III (Coppermine)' `" != x ; then
				spigni_nativo="$spigni_nativo -march=pentium3 -mtune=pentium3 -msse"
			fi
#			if test x"` echo $cpuinfomn | grep 'Xeon'`" != x ; then
#				Intel(R) Xeon(TM) CPU 3.00GHz
#				spigni_nativo="$spigni_nativo -march=pentium4 -mtune=pentium4"
#			fi
		else
			spigni_nativo="-march=native -mtune=native $spigni_nativo"
		fi
		# NOTE : compilers like gcc version 3.4.6 20060404 (Red Hat 3.4.6-10) do not accept native switch
		# -march=pentium3
		# on p4: --malign=double -march=pentium4 -mfpmath=sse -msse2 -
		walls="-Wall -Wredundant-decls -Wno-switch -Wdisabled-optimization -Wdeclaration-after-statement   "" -Wpointer-arith -Wstrict-prototypes "
		#" -pedantic"
		AC_MSG_NOTICE([Guessing the C compiler is gcc.])
	fi
else 
	true
fi

#dnl	***********************************************************************
#dnl	GNU FORTRAN runtime 
#dnl	***********************************************************************
if test "x$ac_cv_fc_compiler_gnu" = xyes ; then
	LIBS="${LIBS} -lgfortran"
	AC_MSG_NOTICE([Guessing the Fortran compiler is gfortran and adding -lgfortran to LIBS (invoke with ac_cv_fc_compiler_gnu=no to prevent this).])
fi
#dnl	***********************************************************************
#dnl	CFLAGS handling starts here
#dnl	***********************************************************************
if test "x$CFLAGS" = x ; then
if test "x$enable_optimize" = xyes && test x$enable_debug != xyes ; then
	if test "x$mio_spigni_forte" = "x" ; then
		true;
	else
		spigni_forte="$mio_spigni_forte";
		spigni_nativo="";
	fi
	if test "x$spigni_forte" = "x" ; then
		spigni="-O3 -malign-double $spigni_nativo"
		#spigni="-O3 -fomit-frame-pointer -malign-double $spigni_nativo"
	else
		spigni="$spigni_forte $spigni_nativo"
	fi

	AC_MSG_NOTICE([Adding ${spigni} to CFLAGS.])
	#CFLAGS="${CFLAGS} ${spigni}"
	AC_MSG_NOTICE([Overriding CFLAGS="$CFLAGS".. ])
	CFLAGS="${spigni}"
else 
	if test "x$enable_debug" = xyes; then
		if test "x$debug_flags" = "x" ; then
			CFLAGS="-O0 -g"
		else
			CFLAGS="${debug_flags}"
		fi
		AC_MSG_NOTICE([Optimization turned off. Debugging enabled. (CFLAGS overwritten)])
		# since we are allowed to suggest flags, we do so
		CFLAGS="${CFLAGS} ${SPCFLAGS}"
	else 
		true;
	fi
	true;
fi
#
fi
#dnl	***********************************************************************
# fix just for SP
if test x"${SPCFLAGS}" != x ; then
	CFLAGS="${CFLAGS} ${SPCFLAGS}"
fi
#dnl	***********************************************************************
if test x"$enable_dmalloc" == x"1"  ; then
	CFLAGS="$CFLAGS $DMALLOC_CFLAGS"
fi
#dnl	***********************************************************************
if test x"$want_mkl_libs" != x""  ; then
	dnl use --without-mkl to disable it
      	AC_MSG_NOTICE([Enabling MKL support in the benchmarking program (will add "$MKL_INCLUDE" to compilation flags; will link to "$want_mkl_libs").])
	AC_DEFINE([RSB_WANT_MKL],[1],[Enabling MKL support in the benchmarking program.])
	RSB_RSBENCH_LIBS="${RSB_RSBENCH_LIBS} $want_mkl_libs"
		if test -n "$MKL_INCLUDE" ; then
			RSB_RSBENCH_CFLAGS="$RSB_RSBENCH_CFLAGS -I $MKL_INCLUDE"
		fi
	else
	AC_MSG_NOTICE([Disabling MKL support in the benchmarking program.])
	AC_DEFINE([RSB_WANT_MKL],[0],[No MKL support wanted in the benchmarking program.])
fi
#dnl	***********************************************************************
if test x"$enable_openmp" = x"yes"; then
	AC_DEFINE([RSB_WANT_OMP_RECURSIVE_KERNELS],[1],[Recursive kernels parallelized with OpenMP.])
	if test x"$openmp_flags" != "x" ; then
dnl		AC_DEFINE([RSB_WANT_OMP_KERNELS],[1],[Kernels parallelized with OpenMP.])
		CFLAGS="${CFLAGS} $openmp_flags"
		if test "x$OPENMP_FCFLAGS" = "x" ; then
			OPENMP_FCFLAGS="$openmp_flags"
		fi
		FCFLAGS="${FCFLAGS} ${OPENMP_FCFLAGS}"
	else
		AC_MSG_NOTICE([We do not know an appropriate OpenMP-enabling flag but assume OpenMP is active.])
		CFLAGS="${CFLAGS}"
	fi
else 
	AC_DEFINE([RSB_WANT_OMP_RECURSIVE_KERNELS],[0],[Recursive kernels parallelized with OpenMP.])
	AC_MSG_NOTICE([OpenMP code disabled: 1 thread at most is allowed.])
        want_max_threads="1"
	true;
fi
#dnl	***********************************************************************
if test "x$enable_c99" = xyes; then
	if test "x$c99_flags" = "x" ; then
		AC_MSG_NOTICE([We do not know an appropriate c99-enabling flag..])
		CFLAGS="${CFLAGS}"
	else
		CFLAGS="${CFLAGS} $c99_flags"
	fi
else 
	true;
fi
#dnl	***********************************************************************
AH_TEMPLATE([OSKI_LUA_PATH])
if test "x$enable_oski" = xyes; then
      	AC_MSG_NOTICE([Looking for user set OSKI_INCLUDE, OSKI_LUA_PATH, OSKI_PATH environment variables..])
	save_CFLAGS="$CFLAGS"
	if test -n "$OSKI_INCLUDE" ; then
		CFLAGS="$CFLAGS -I $OSKI_INCLUDE"
	fi
	AC_CHECK_HEADERS([oski/oski.h], [true])
	CFLAGS="$save_CFLAGS"
	if test "x$ac_cv_header_oski_oski_h" != xyes; then
dnl AC_MSG_WARN
	AC_MSG_ERROR([Header file <oski/oski.h> not found, therefore we will not use it!])
	else
		# FIXME: this is temporary, for my own machines
		if test -d "~/usr/local/include/" ; then
			CFLAGS="$CFLAGS -I ~/usr/local/include/"
		fi

		if test -n "$OSKI_INCLUDE" ; then
			CFLAGS="$CFLAGS -I $OSKI_INCLUDE"
		fi

		if test x"$OSKI_PATH" = x && test -d "/usr/local/lib/oski" ; then
			OSKI_PATH=/usr/local/lib/oski
		fi
		if test x"$OSKI_LUA_PATH" = x ; then
			OSKI_LUA_PATH="$OSKI_PATH/?.lua"
			else
			true;
		fi
		if test x"$OSKI_LIBS" = x ; then
			# oski-1.0.1h works in this way
			#OSKI_LIBS=`cat $OSKI_PATH/site-modules-static.txt | tr '\n' ' '`
			OSKI_LIBS=`cat $OSKI_PATH/site-modules-shared.txt | tr '\n' ' '`
			# the following often fail due to the incorrect order of libs:
			#OSKI_LIBS=`cat $OSKI_PATH/site-modules-static.txt|sed 's/^\/.*\///g;s/^'/"$OSKI_PATH\/"/g | tr '\n' ' '`
		fi
		if test x"$OSKI_LIBS" = x ; then
			AC_MSG_ERROR([No linkable libraries for OSKI ? Disable OSKI support ot try setting OSKI_LIBS by hand.])
		fi
		if test x"${OSKI_CFLAGS}" = x && test -d "~/usr/local/include/"; then
			OSKI_CFLAGS="$OSKI_CFLAGS -I  /usr/local/include/"
			OSKI_CFLAGS="$OSKI_CFLAGS -I ~/usr/local/include/"
		fi
		#
		RSB_RSBENCH_LIBS="${RSB_RSBENCH_LIBS} -L${OSKI_PATH} ${OSKI_LIBS}"

		# FIXME: this is temporary, for my own machines
		if test -d "~/usr/local/lib/oski/" ; then
			RSB_RSBENCH_LIBS="${RSB_RSBENCH_LIBS} -L ~/usr/local/lib/oski/"
		fi

		RSB_RSBENCH_CFLAGS="${RSB_RSBENCH_CFLAGS} $OSKI_CFLAGS"
	      	AC_MSG_NOTICE(Enabling comparative OSKI benchmarking.)
		AC_DEFINE([RSB_WANT_OSKI_BENCHMARKING],[1],[OSKI comparative benchmarking.])
		AC_DEFINE_UNQUOTED([OSKI_LUA_PATH],"$OSKI_LUA_PATH",[OSKI path to installed lua modules. User set OSKI_LUA_PATH environment variable at runtime will override this one, however.])
	fi
else 
      	true;
fi
#dnl
AC_SUBST([RSB_CONST_MAX_SUPPORTED_THREADS],"${want_max_threads}")
#dnl	***********************************************************************
AC_DEFINE_UNQUOTED([RSB_INT_ERR_VERBOSITY],[$want_int_verrbosity],[Inner error verbosity (internal debug level).])
AC_DEFINE_UNQUOTED([RSB_OUT_ERR_VERBOSITY],[$want_ext_verrbosity],[Error verbosity (often known as debug level).])
AC_DEFINE_UNQUOTED([RSB_WANT_IO_LEVEL],[$want_io_level],[Supported input/output functionality.])
AC_DEFINE_UNQUOTED([RSB_USER_SET_MEM_HIERARCHY_INFO],["$RSB_USER_SET_MEM_HIERARCHY_INFO"],[If not null, the library will rely on this for memory hierarchy info.])
AC_DEFINE_UNQUOTED([RSB_DETECTED_MEM_HIERARCHY_INFO],["$RSB_DETECTED_MEM_HIERARCHY_INFO"],[If not null, the library will rely on this for memory hierarchy info, unless RSB_USER_SET_MEM_HIERARCHY_INFO is set.])
AC_DEFINE_UNQUOTED([RSB_CONST_MAX_SUPPORTED_THREADS],[$RSB_CONST_MAX_SUPPORTED_THREADS],[Maximal number of supported threads (default 64).])
AC_DEFINE_UNQUOTED([CFLAGS],["$CFLAGS"],[Compilation flags.])
AC_DEFINE_UNQUOTED([CC],["$CC"],[C compiler.])
if test x"$RSB_USE_ASSERT" != x ; then
AC_DEFINE_UNQUOTED([RSB_USE_ASSERT],["$RSB_USE_ASSERT"],[If undefined, NDEBUG will be defined.])
fi
#dnl	***********************************************************************
WANT_MATRIX_STORAGE=""
if test x"$enable_c" = xyes ; then
	WANT_MATRIX_BCOO_STORAGE=BCOR
#	WANT_MATRIX_BCOO_STORAGE=BCOR,BCOC
	WANT_MATRIX_STORAGE="$WANT_MATRIX_STORAGE,$WANT_MATRIX_BCOO_STORAGE"
fi
if test x"$enable_b" = xyes ; then
	WANT_MATRIX_BCSS_STORAGE=BCSR
#	WANT_MATRIX_BCSS_STORAGE=BCSR,BCSC
	WANT_MATRIX_STORAGE="$WANT_MATRIX_STORAGE,$WANT_MATRIX_BCSS_STORAGE"
fi
# we get rid of the comma
WANT_MATRIX_STORAGE="`echo $WANT_MATRIX_STORAGE| sed 's/^,//g'`"
#dnl	***********************************************************************
if test "x${userset_nounroll_cflag}" != x ; then
	no_unroll_flags="${userset_nounroll_cflag}"
fi
dnl	***********************************************************************
# for rsb_config.m4.in
AC_SUBST(enable_restrict)
AC_SUBST(enable_openmp)
AC_SUBST(CFLAGS)
AC_SUBST(FCFLAGS)
dnl AC_SUBST(NOUNROLLCFLAGS,"${CFLAGS} $no_unroll_flags")
AC_SUBST(NOUNROLLCFLAGS,"$no_unroll_flags")
AC_SUBST(RSB_RSBENCH_LIBS,"$RSB_RSBENCH_LIBS")
AC_SUBST(RSB_RSBENCH_CFLAGS,"$RSB_RSBENCH_CFLAGS")
AC_SUBST(WANT_ROW_UNLOOP_FACTORS,["$row_unrolls"])
AC_SUBST(WANT_LOOPING_KERNELS,["$want_looping_kernels"])
AC_SUBST(WANT_COLUMN_UNLOOP_FACTORS,["$column_unrolls"])
AC_SUBST(WANT_SIMPLE_LOOP_UNROLL_DEFAULT_FACTOR,["$util_unrolls"])
dnl AC_SUBST(WANT_HALFWORD_INDICES,["$want_halfword_indices"])
AC_SUBST(WANT_HALFWORD_INDICES,["yes"])
AC_SUBST(WANT_SPSM_DIAG_CHECK,["$want_spsm_diagonal_check"])
AC_SUBST(WANT_TYPES,["$want_matrix_types"])
AC_SUBST(WANT_MATRIX_BCSS_STORAGE,["$WANT_MATRIX_BCSS_STORAGE"])
AC_SUBST(WANT_MATRIX_BCOO_STORAGE,["$WANT_MATRIX_BCOO_STORAGE"])
AC_SUBST(WANT_MATRIX_LINKED_STORAGE,["$WANT_MATRIX_LINKED_STORAGE"])
AC_SUBST(WANT_MATRIX_VB_STORAGE,["$WANT_MATRIX_VB_STORAGE"])
AC_SUBST(WANT_MATRIX_STORAGE,["$WANT_MATRIX_STORAGE"])
AC_SUBST(WANT_MATRIX_OPS,["$want_matrix_ops"])
AC_SUBST(WANT_MATRIX_ALL_META_OPS,["spmv,spsv"])
AC_SUBST(WANT_MATRIX_ALL_OPS,["$all_matrix_ops"])
AC_SUBST(WANT_MATRIX_ALL_TYPES,["$all_matrix_types"])
dnl AC_SUBST(OT_SRCS,"`for o in $all_matrix_ops echo $o ; done`")
#dnl	***********************************************************************
if test x = x"$ARFLAGS" ; then ARFLAGS="cru" ; fi # damn AIX ar
AC_SUBST(ARFLAGS,"$ARFLAGS")
#dnl	***********************************************************************

if test x"${enable_fortran_examples}" = x"yes" -a x"${FC}" = x"" ;  then
	enable_fortran_examples=no
	AC_MSG_WARN([No Fortran compiler detected (FC environment variable). Will not build the Fortran examples.])
fi
AC_SUBST([HAVE_FORTRAN_EXAMPLES],["$enable_fortran_examples"])
AM_CONDITIONAL([HAVE_FORTRAN_EXAMPLES],[test x"$enable_fortran_examples" = xyes ])
AM_CONDITIONAL([HAVE_C_EXAMPLES],[test x"$enable_c_examples" = xyes ])
AM_CONDITIONAL([HAVE_OCTAVE],[test x"$OCTAVE" != xfalse ])
want_int=`echo "$want_matrix_types" | grep '\<int\>'`
if test x"$OCTAVE" != xfalse -a x"$enable_octave_testing" = xyes ; then want_octave_testing=yes; else want_octave_testing=no; fi
AM_CONDITIONAL([WANT_OCTAVE_TESTING],[test x"$want_octave_testing" = x"yes" ])
if test x"$OCTAVE" != xfalse -a x"$want_int" != x -a x"$enable_octave_testing" = xyes ; then want_octave_testing_and_int=yes; else want_octave_testing_and_int=no ; fi
AM_CONDITIONAL([WANT_OCTAVE_TESTING_AND_INT],[test x"$want_octave_testing_and_int" = x"yes" ])
if test x"${FC}" = x"" ;  then
if test x"${want_blas_sparse_mod_install}" = x"yes" -o x"${sparse_blas_interface}" = x"yes" ;  then
	want_blas_sparse_mod_install=no;
	sparse_blas_interface=no;
	AC_MSG_WARN([No Fortran compiler detected (FC environment variable). Will not build the BLAS interface.])
fi
fi
AM_CONDITIONAL([WANT_BLAS_SPARSE_MOD_INSTALL],[test x"$want_blas_sparse_mod_install" = x"yes"])
dnl AM_CONDITIONAL([WANT_BLAS_SPARSE_FI],[test x"$want_blas_sparse_fi_install" = x"yes"])
AM_CONDITIONAL([WANT_CXX_TEST_RSBENCH],[test x"$CXX" != x ])
AM_CONDITIONAL([HAVE_DOXYGEN],[test x"$DOXYGEN" != x"false" ])
AM_CONDITIONAL([WANT_BUILD_DOC],[test x"$want_build_doc" = x"yes" ])
AM_CONDITIONAL([HAVE_PKGCONFIG_INSTALL],[test x"$want_install_pkg_config" = x"yes" ])
AM_CONDITIONAL([HAVE_HELP2MAN],[test x"$HELP2MAN" != x"false" ])
AM_CONDITIONAL([HAVE_M4],[test x"$M4" != xfalse ])
AM_CONDITIONAL([HAVE_FC],[test x"$FC" != x ])
dnl AM_CONDITIONAL([WANT_DL],[test x"$want_rsb_dl" = x"yes"])
AM_CONDITIONAL([HAVE_SPARSE_BLAS_INTERFACE],[test x"$sparse_blas_interface" = xyes ])
AM_CONDITIONAL([WANT_INTERNAL_HEADERS_INSTALL],[test x"$enable_ihi" = xyes ])
AM_CONDITIONAL([WANT_OMPIO_SUPPORT],[test x"$want_ompio" = x"yes" && test x"$enable_openmp" = x"yes" ])
AC_SUBST(OCTAVE_FLAGS,"--no-history --no-line-editing  --no-site-file --norc   --silent")
#dnl	***********************************************************************
dnl AC_MSG_NOTICE([
dnl Will generate code for types in {$want_matrix_types}, for matrix ops in {$want_matrix_ops}.
dnl Will generate code for {$row_unrolls} x {$column_unrolls}-sized blocks, for types in {$want_matrix_types} for matrix ops in {$want_matrix_ops}.
dnl ])
dnl AC_MSG_NOTICE([FIXME : please run `make clean` as a first thing  ])
#dnl	***********************************************************************
AC_CONFIG_FILES([librsb-config:librsb-config.in],[chmod +x librsb-config])
AC_CONFIG_FILES([librsb.pc:librsb.pc.in],[])
AC_CONFIG_FILES([examples/make.sh:examples/make.sh.in],[chmod +x examples/make.sh])
AC_OUTPUT([rsb_config.m4] [Makefile] [bench/Makefile] [doc/Makefile] [examples/Makefile] [scripts/Makefile] [m4/Makefile] [blas_sparse/Makefile])
#dnl	***********************************************************************
AC_MSG_NOTICE([dnl
=============== Build Programs and Flags ===============================
(you can override these at build time; e.g.: 'make CC=cc')
dnl	ac_cv_c_compiler_gnu: ${ac_cv_c_compiler_gnu}
	CC                     : ${CC}
	CFLAGS                 : ${CFLAGS}
	NOUNROLLCFLAGS         : ${NOUNROLLCFLAGS}
	FC (to disable, FC='') : ${FC}
	FCFLAGS                : ${FCFLAGS}
	LD                     : ${LD}
	LDFLAGS                : ${LDFLAGS}
	LIBS                   : ${LIBS}
	AR                     : ${AR}
	ARFLAGS                : ${ARFLAGS}
	M4                     : ${M4}
	OCTAVE                 : ${OCTAVE}
dnl	WANT_OCTAVE_TESTING
dnl	Octave executable            : "$OCTAVE"
	DOXYGEN                : ${DOXYGEN}
	HELP2MAN               : ${HELP2MAN}
dnl	SED               : ${SED}
dnl	HAVE_DOXYGEN                 : "$DOXYGEN"
dnl	Doxygen executable           : "$DOXYGEN"
dnl	HAVE_HELP2MAN                : "$HELP2MAN"
dnl	help2man executable : "$HELP2MAN"
dnl	HAVE_M4
dnl	M4 executable       : "$M4"
dnl	WANT_CXX_TEST_RSBENCH        : "$CXX"
	CXX                    : ${CXX}
dnl		Misc info:
=== Additional flags affecting only the benchmark program (rsbench): ===
	RSB_RSBENCH_LIBS             : ${RSB_RSBENCH_LIBS}
	RSB_RSBENCH_CFLAGS           : ${RSB_RSBENCH_CFLAGS}
dnl
dnl		Main code generator values, predefined defaults
dnl	All Numerical types          : ${all_matrix_types}
dnl	Numerical types              : ${default_types}
dnl	default_matrix_ops  : ${default_matrix_ops}
dnl	default_unrolls	    : ${default_unrolls}
dnl	all_matrix_ops      : ${all_matrix_ops}
dnl	blas_matrix_ops     : ${blas_matrix_ops}
dnl	psblas_matrix_ops   : ${psblas_matrix_ops}
dnl	Build Sparse BLAS Interface  : "${sparse_blas_interface_default}"
dnl	Util. Kernels Unroll         : ${default_util_unrolls}
dnl	Triangular solve zero check  : "${want_spsm_diagonal_check_default}"
dnl
========= Main code generator values, this build vs defaults ===========
(if these differ from the defaults, you need to have M4 and run 'make cleanall' and 'make')
	All Numerical types          : "${all_matrix_types}"
	Numerical types              : "${want_matrix_types}" vs ["${default_types}"]
dnl	want_matrix_ops     : ${want_matrix_ops}
dnl	row_unrolls         : ${row_unrolls}
dnl	column_unrolls      : ${column_unrolls}
dnl	HAVE_SPARSE_BLAS_INTERFACE:
	Build Sparse BLAS Interface  : "${sparse_blas_interface}" vs ["${sparse_blas_interface_default}"]
	Util. Kernels Unroll         : "${util_unrolls}" vs ["${default_util_unrolls}"]
dnl	matrix storage      : ${WANT_MATRIX_STORAGE}
	Triangular solve zero check  : "${want_spsm_diagonal_check}" vs ["${want_spsm_diagonal_check_default}"]
============== Build Configuration, this build vs defaults =============
(if you reconfigure and change these, you need to run 'make clean' and 'make')
dnl	host_os		  : ${host_os}
dnl	host_cpu	  : ${host_cpu}
dnl	short indices     : ${want_halfword_indices}
dnl	Configured I/O level : ${want_io_level}
dnl	RSB_WANT_IO_LEVEL                   : ${want_io_level}
	Supported I/O functionality level   : "${want_io_level}" vs "${default_want_io_level}"
dnl	RSB_OUT_ERR_VERBOSITY               : ${want_ext_verrbosity}
	Interface Error Verbosity           : "${want_ext_verrbosity}" vs "${default_want_ext_verrbosity}"
dnl	RSB_INT_ERR_VERBOSITY               : ${want_int_verrbosity}
	Internals Error Verbosity           : "${want_int_verrbosity}" vs "${default_want_int_verrbosity}"
dnl		Host specific info:
dnl	RSB_USER_SET_MEM_HIERARCHY_INFO     : ${RSB_USER_SET_MEM_HIERARCHY_INFO}
dnl	RSB_DETECTED_MEM_HIERARCHY_INFO     : ${RSB_DETECTED_MEM_HIERARCHY_INFO}
dnl	RSB_CONST_MAX_SUPPORTED_THREADS     : ${RSB_CONST_MAX_SUPPORTED_THREADS}
	Memory hierarchy info, detected     : "${RSB_DETECTED_MEM_HIERARCHY_INFO}"
	Memory hierarchy info, selected     : "${RSB_USER_SET_MEM_HIERARCHY_INFO}"
	Maximum of Supported threads        : "${RSB_CONST_MAX_SUPPORTED_THREADS}"
dnl
dnl		Configured Makefile conditionals:
dnl	WANT_SPSM_DIAG_CHECK
dnl	HAVE_FORTRAN_EXAMPLES
dnl 	Build Fortran code                  : "$enable_fortran"
	Build Fortran examples              : "$enable_fortran_examples"
dnl	HAVE_C_EXAMPLES
	Build C examples                    : "$enable_c_examples"
dnl	WANT_OMPIO_SUPPORT
dnl	Want OpenMP + I/O            : "$want_ompio"
dnl	RSB_DISABLE_ALLOCATOR_WRAPPER: "$disable_allocator_wrapper"
dnl	WANT_BLAS_SPARSE_MOD_INSTALL
	Install Sparse BLAS Fortran modules : "$want_blas_sparse_mod_install"
	Install pkg-config "librsb.pc" file : "$want_install_pkg_config"
dnl	WANT_BLAS_SPARSE_FI
dnl	Install Sparse BLAS Fortran headers : "$want_blas_sparse_fi_install"
dnl	WANT_OCTAVE_TESTING                 : "$want_octave_testing"
	Build Octave generated tester       : "$want_octave_testing"
dnl	WANT_OCTAVE_TESTING_AND_INT
	Build Octave generated tester (int) : "$want_octave_testing_and_int"
	Build HTML and man documentation    : "$want_build_doc"
	gzipped matrices support via zlib   : "${want_zlib_support}"
	gather elapsed time in librsb       : "${enable_librsb_stats}"
dnl
dnl	WANT_INTERNAL_HEADERS_INSTALL: "$enable_ihi"
dnl	CPU  	          : ${CPU}
dnl	all types ...
])

if test x"$OCTAVE" != xfalse && test x"$want_int" != x  ; then
AC_MSG_NOTICE([You seem to have GNU Octave and enabled 'int' type. This will allow an additional part of the test suite to be generated.])
else
AC_MSG_NOTICE([You seem to not have GNU Octave or have disabled 'int' type. Part of the test suite will not be generated. If you want more testing capabilities, you should enable the 'int' type as well.])
fi
if test x"$M4" = x ; then
	AC_MSG_WARN([No m4 implementation detected. You will not be able to generate code.])
else
	true
fi
# FIXME : should warn the user in the case of opting out the 'int' type, as in this case there would be no GNU/octave based testing.

if test x"$default_types"  != x"$want_matrix_types" ; then
AC_MSG_WARN([You chose a custom matrix types selection. If you just unpacked from archive, you should issue "make cleanall" to delete the shipped code and then "make" will regenerate it by using m4.])
fi
if test x"$want_rsb_dl"  = x"yes" ; then
dnl AC_MSG_WARN([You chose dynamic linking of example executables. To execute them you will probably to update your environment; e.g.: export LD_LIBRARY_PATH=`pwd`/:\$LD_LIBRARY_PATH .])
AC_MSG_WARN([You chose an obsolete switch (--enable-shlib-linked-examples), which now on is disabled.])
fi
AC_MSG_NOTICE([Successfully configured librsb version "$LIBRSB_VERSION".])
