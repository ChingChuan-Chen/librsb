/* @cond INNERDOC */
/*! 
 @file
 @brief 

 Matrix Operations testing code source file.
 This is NOT part of the library: only of companion programs.

 */

/*

Copyright (C) 2008-2018 Michele Martone

This file is part of librsb.

librsb is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

librsb is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public
License along with librsb; see the file COPYING.
If not, see <http://www.gnu.org/licenses/>.

*/
/*
 The code in this file was generated automatically by an M4 script. 
 It is not meant to be used as an API (Application Programming Interface).
 p.s.: right now, only row major matrix access is considered.

 */
/*!
 @file
 @brief
 Performance kernels dispatching code, for each type, submatrix size, operation.
 But for block compressed sparse stripes format.
 Kernels unrolled, with no loops, for only user-specified blockings.
 */

/*

Copyright (C) 2008-2018 Michele Martone

This file is part of librsb.

librsb is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

librsb is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public
License along with librsb; see the file COPYING.
If not, see <http://www.gnu.org/licenses/>.

*/
/*
 The code in this file was generated automatically by an M4 script. 
 It is not meant to be used as an API (Application Programming Interface).
 p.s.: right now, only row major matrix access is considered.

 */
#ifndef RSB_TEST_MATOPS_H_INCLUDED
#define RSB_TEST_MATOPS_H_INCLUDED

/* FIXME: necessary, until we use so many #ifdefs in this program */
#include "rsb-config.h"
#include "rsb_common.h"
#include "rsb_mkl.h"

#if RSB_WITH_LIKWID
#include <likwid.h>
#define RSB_LIKWID_MARKER_INIT	{RSBENCH_STDOUT("# Initializing the LIKWID API with likwid_markerInit().\n");likwid_markerInit();}
#define RSB_LIKWID_MARKER_EXIT {RSBENCH_STDOUT("# Finalizing the LIKWID API with likwid_markerClose().\n");likwid_markerClose();}
#define RSB_LIKWID_MARKER_R_START(R) likwid_markerStartRegion(R)
#define RSB_LIKWID_MARKER_R_STOP(R) likwid_markerStopRegion(R)
#else /* RSB_WITH_LIKWID */
#define RSB_LIKWID_MARKER_INIT
#define RSB_LIKWID_MARKER_EXIT
#define RSB_LIKWID_MARKER_R_START(R)
#define RSB_LIKWID_MARKER_R_STOP(R)
#endif /* RSB_WITH_LIKWID */

#define RSB_STDOUT_FD stdout
#define RSBENCH_STDOUT( ... ) fprintf(RSB_STDOUT_FD, __VA_ARGS__ )
#define RSB_WAT_FMT_H "Ss[Xx[Tt[V[V]]]]"
#define RSB_WAT_FMT "%lfs[%dx[%dt[%c]]]"

#define RSB_MIN_ABOVE_INF(X,Y,MIN) RSB_MAX(RSB_MIN(X,Y),MIN)
#define RSB_INT_MILLION 1000000
#define RSB_REAL_MILLION 1000000.0 
enum rsb_dumpvec_enum { rsb_dumpvec_no= 0, rsb_dumpvec_res= 1, rsb_dumpvec_rhs= 2 };

#if RSB_HAVE_LIBGEN_H
#include <libgen.h>	/* for basename (20101226 FIXME : superseded by rsb__basename usage)*/
#endif /* RSB_HAVE_LIBGEN_H */

#define RSB_HAVE_METIS 0 /* FIXME: unfinished */
#if RSB_HAVE_METIS
#include <metis/metis.h>
#endif /* RSB_HAVE_METIS */

#ifdef RSB_WANT_OSKI_BENCHMARKING 
#ifdef RSB_HAVE_OSKI_OSKI_H 
#include <oski/oski.h>
#else /* RSB_HAVE_OSKI_OSKI_H */
#error "you should disable oski benchmarking at configure time!"
#endif /* RSB_HAVE_OSKI_OSKI_H */
#endif /* RSB_WANT_OSKI_BENCHMARKING */
#ifdef RSB_HAVE_UNISTD_H
#include <unistd.h>
#endif /* RSB_HAVE_UNISTD_H */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef RSB_WANT_PERFORMANCE_COUNTERS
	#define RSB_PERFORMANCE_COUNTERS_DUMP_MEAN(MSG,PMSG,TIMES,PCIP) if(want_perf_counters>0){rsb_perf_counters_update(); if(PMSG)rsb_perf_counters_dump(MSG,NULL,TIMES,PCIP); rsb_perf_counters_reset();/* TEMPORARY */}
	#define RSB_PERFORMANCE_COUNTERS_DUMP(MSG,PMSG) if(want_perf_counters>1)RSB_PERFORMANCE_COUNTERS_DUMP_MEAN(MSG,PMSG,1,NULL) 
#else /* RSB_WANT_PERFORMANCE_COUNTERS */
	#define RSB_PERFORMANCE_COUNTERS_DUMP_MEAN(MSG,PMSG,TIMES,PCIP) 
	#define RSB_PERFORMANCE_COUNTERS_DUMP(MSG,PMSG)
#endif /* RSB_WANT_PERFORMANCE_COUNTERS */

#if RSB_WITH_LIKWID
#define RSB_TM_LIKWID_MARKER_R_START(R) if(want_likwid == RSB_BOOL_TRUE)RSB_LIKWID_MARKER_R_START(R)
#define RSB_TM_LIKWID_MARKER_R_STOP(R)  if(want_likwid == RSB_BOOL_TRUE)RSB_LIKWID_MARKER_R_STOP(R)
#else
#define RSB_TM_LIKWID_MARKER_R_START(R)
#define RSB_TM_LIKWID_MARKER_R_STOP(R)
#endif /* RSB_WITH_LIKWID */


#define RSB_WANT_PERFORMANCE_COUNTERS_IN_RSBENCH  defined(RSB_WANT_PERFORMANCE_COUNTERS) && (RSB_WANT_PERFORMANCE_COUNTERS==1)





int rsb_test_help_and_exit(rsb_char_t *argv0, rsb_option *o, int code);
/* one function for each of (spmv_uaua,spsv_uxua,mat_stats)*/
int rsb__main_block_partitioned_spmv_uaua(const int argc, rsb_char_t * const argv[])
;
int rsb__main_block_partitioned_spsv_uxua(const int argc, rsb_char_t * const argv[])
;
int rsb__main_block_partitioned_mat_stats(const int argc, rsb_char_t * const argv[])
;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif	/* RSB_TEST_MATOPS_H_INCLUDED */

/* @endcond */
