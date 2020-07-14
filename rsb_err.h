/*                                                                                                                            
Copyright (C) 2008-2017 Michele Martone

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
/* @cond INNERDOC */
/*
 * @author Michele Martone
 */
#ifndef RSB_ERR_H_INCLUDED
#define RSB_ERR_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "rsb_common.h"

#define RSB_ERRM_E_MTXAP	"Supplied NULL  matrix structure pointer !"
#define RSB_ERRM_E_MTXAPP	"Supplied NULL  matrix structure pointer pointer !"
#define RSB_ERRM_E_VIJ	"Supplied NULL  VA, IA, JA arrays !"
#define RSB_ERRM_E_VIJP	"Supplied NULL  VAP, IAP, JAP arrays !"
#define RSB_ERRM_CNHEAF	"Cannot reuse arrays while passing them as const."
#define RSB_ERRM_EM	"!\n"
#define RSB_ERRM_NULL_VA "Supplied VA array pointer!\n"
#define RSB_ERRM_BCE	"Internal bounds computing error!\n"
#define RSB_ERRM_ZSM	"WARNING : zero sized malloc !\n"
#define RSB_ERRM_BFEANS	"Bad flags: RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS are not supported here!\n"
#define RSB_ERRM_SEOWS	"some error occurred while shuffling\n"
#define RSB_ERRM_ALSMINT	"a leaf submatrix is non terminal!\n"
#define RSB_ERRM_ANLSMIT	"a non leaf submatrix is terminal!\n"
#define RSB_ERRM_FAOTAFS	"failed allocation of temporary array for swap"
#define RSB_ERRM_ES	""
#define RSB_ERRM_NPSFF	"NULL pointer supplied for filename."
#define RSB_ERRM_CBAEM	"Cannot build and empty matrix.\n"
#define RSB_ERRM_CMOINIY	"Column Major order is not implemented yet.\n"
#define RSB_ERRM_MDNFARTS	"matrix does not fit as RCSR (too sparse)!\n"
#define RSB_ERRM_FYRYNS	"fatal : your rsb_coo_idx_t type is not supported.."
#define RSB_ERRM_WTC		"error : specified type code is not valid\n"
#define RSB_ERRM_COVMUINS	"control of virtual memory usage is not supported\n"
#define RSB_ERRM_FCOVMU		"failed control of virtual memory usage\n"
#define RSB_ERRM_PFTM		"Problems finalizing the matrix!\n"
#define RSB_ERRM_WUF		"WARNING: unfinished code!\n"
#define RSB_ERRM_PAL		"probable allocation problem\n"
#define RSB_ERRM_NAOL		"no array of leaves ?\n"
#define RSB_ERRM_NNTC		"no nonzeros to clone ?\n"
#define RSB_ERRM_NDIANN		"no diagonal implicit and no nonzeros ?\n"
#define RSB_ERRM_CP		"cleanup problem\n"
#define RSB_ERRM_BNCS		"blocking not correctly specified!\n"
#define RSB_ERRM_CIIAUF		"Clique insertion is an unfinished functionality!\n"
#define RSB_ERRM_FMMTDT		"failed matrix multiplication to dense test\n"
#define RSB_ERRM_FMATD		"failed matrix add to dense\n"
#define RSB_ERRM_FMM		"failed matrix multiplication\n"
#define RSB_ERRM_FMC		"failed matrix cloning\n"
#define RSB_ERRM_CMINBC		"cloned matrix is not built correctly\n"
#define RSB_ERRM_FCMS		"Failed computing matrix sum.\n"
#define RSB_ERRM_FMATDBC	"failed matrix add to dense basic checksum\n"
#define RSB_ERRM_FYCITINS	"fatal : your rsb_coo_idx_t type is not supported.."
#define RSB_ERRM_WOPSTASA	"WARNING : overflow possible. Switching to another sort algorithm.\n"
#define RSB_ERRM_SLIINS		"error : seems like input is not sorted\n"
#define RSB_ERRM_EWEFCFD	"error while estimating fillin (corrupt fillin data?)\n"
#define RSB_ERRM_EWEFCTD	"error while estimating fillin (corrupt timing data?)\n"
#define RSB_ERRM_ERROR		"error\n"
#define RSB_ERRM_ESIIB		"error : sorting input is bad\n"
#define RSB_ERRM_SLSIB		"error : seems like sorting is bugged\n"
#define RSB_ERRM_SIL		"error initializing library!\n"
#define RSB_ERRM_EDNC		"error during nonzeros compacting\n"
#define RSB_ERRM_ZCFRWAEC	"zero compacting function returned with an error code\n"
#define RSB_ERRM_EQRPF		"error reading performance file.\n"
#define RSB_ERRM_ELMPF		"error loading memory performance file.\n"
#define RSB_ERRM_AE		"allocation error\n"
#define RSB_ERRM_IE		"internal error ?\n"
#define RSB_ERRM_NL		"\n"
#define RSB_ERRM_MBE		"matrix build error!\n"
#define RSB_ERRM_BM		"bad mtxAp:"
#define RSB_ERRM_TS		"a problem occurred in triangular solve!\n"
#define RSB_ERRM_MV		"a problem occurred in sparse matrix-vector product!\n"
#define RSB_ERRM_NOLP		"timer-based profiling has not been enabled at configure time!\n"
#define RSB_ERRM_IMNIP		"input matrix is not an in place one!\n"
#define RSB_ERRM_DNSAMIWAFCB		"Did not supply a matrix initiated with rsb_mtx_alloc_from_coo_begin!\n"
#define RSB_ERRM_IPEWIEM	"Internal problem encounted when initiating an empty matrix\n"
#define RSB_ERRM_NO_XDR	"No XDR configured in: binary matrix I/O disabled.\n"

rsb_err_t rsb__do_perror(FILE *stream, rsb_err_t errval);
/*const rsb_char_t * rsb__do_strerror_r(rsb_err_t errval, rsb_char_t * buf, size_t buflen);*/
rsb_err_t rsb__do_strerror_r(rsb_err_t errval, rsb_char_t * buf, size_t buflen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* RSB_ERR_H_INCLUDED */
/* @endcond */
