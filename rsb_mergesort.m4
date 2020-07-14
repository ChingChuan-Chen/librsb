dnl
dnl
include(`rsb_misc.m4')dnl
include(`do_unroll.m4')dnl
include(`mergesort_macros.m4')dnl
dnl
/* @cond INNERDOC */
dnl
/**
 * @file
 * @brief
 * Sorting functions.
 */
RSB_M4_HEADER_MESSAGE()dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`
#ifndef RSB_MERGESORT_H_INCLUDED
#define RSB_MERGESORT_H_INCLUDED
')
dnl
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ifdef(`ONLY_WANT_HEADERS',`dnl
',`dnl
dnl
dnl /* We may use custom memcpy functions. */
dnl #define RSB_MEMCPY(DST,SRC,BYTES) rsb_memcpy((DST),(SRC),(BYTES))
')dnl
dnl


ifelse(`0',`1',`dnl 20121016 
ifdef(`RSB_M4_WANT_OMP',dnl
dnl	FIXME : this should be moved elsewhere
`#define RSB_WANT_OMP        '1
`#define RSB_MAX_OMP_THREADS 'RSB_M4_MAX_OMP_THREADS
#ifdef RSB_HAVE_OMP_H
#if RSB_WANT_OMP_RECURSIVE_KERNELS
#include <omp.h>       /* OpenMP parallelism (EXPERIMENTAL) */
#endif /* RSB_WANT_OMP_RECURSIVE_KERNELS */
#endif /* RSB_HAVE_OMP_H */
)dnl
')dnl

dnl
dnl #include "rsb_internals.h"
dnl #include "rsb_common.h"
RSB_M4_INCLUDE_HEADERS
dnl #include "types.h"
dnl 

ifdef(`ONLY_WANT_HEADERS',`',`dnl
extern struct rsb_session_handle_t rsb_global_session_handle;
')dnl

dnl
define(`blockorientations',`(CSR,BCSR,VBR)')dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`dnl
foreach(`blockoriented',blockorientations,`dnl
RSB_M4_MERGESORT_ON_COORDINATES_FUNCTION_DISPATCHER_PROTOTYPE(RSB_M4_TYPES,blockoriented);
')dnl
')dnl
dnl
dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
foreach(`blockoriented',blockorientations,`dnl
RSB_M4_MERGESORT_ON_COORDINATES_FUNCTION_DISPATCHER(RSB_M4_TYPES,blockoriented)
')dnl
')dnl
dnl
dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
foreach(`blockoriented',blockorientations,`dnl
foreach(`mtype',RSB_M4_TYPES,`dnl
RSB_M4_MERGESORT_ON_COORDINATES_FUNCTION(mtype,blockoriented)
RSB_M4_MERGESORT_ON_COORDINATES_MERGE_FUNCTION(mtype,blockoriented)
')dnl
')dnl
')dnl
dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`dnl
foreach(`blockoriented',blockorientations,`dnl
foreach(`mtype',RSB_M4_TYPES,`dnl
RSB_M4_MERGESORT_ON_COORDINATES_FUNCTION_PROTOTYPE(mtype,blockoriented);
RSB_M4_MERGESORT_ON_COORDINATES_MERGE_FUNCTION_PROTOTYPE(mtype,blockoriented);
')dnl
')dnl
')dnl
dnl
dnl
#ifdef __cplusplus
}
#endif  /* __cplusplus */
dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`
#endif /* RSB_MERGESORT_H_INCLUDED */
')
dnl
/* @endcond */
dnl
