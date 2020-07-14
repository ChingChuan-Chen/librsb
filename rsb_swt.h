/*                                                                                                                            

Copyright (C) 2008-2015 Michele Martone

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
/*!
 * @file
 * @author Michele Martone
 * @brief This source file contains experimental functions 
 * */

#ifndef RSB_SWT_H_INCLUDED
#define RSB_SWT_H_INCLUDED
#define RSB_MATRIX_STORAGE_AUTO 0x0	/* TODO: move to rsb_types.h */
#include "rsb_internals.h"		/* */
/*#define RSB_CONST_MIN_NNZ_PER_ROW_FOR_COO_SWITCH 4*/
#define RSB_CONST_MIN_NNZ_PER_ROW_FOR_COO_SWITCH 2

typedef unsigned short int rsb_half_idx_t;

rsb_bool_t rsb__do_is_candidate_size_for_halfword_coo(rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_flags_t flags);
rsb_bool_t rsb__do_is_candidate_for_halfword_coo(const struct rsb_mtx_t * mtxAp);
rsb_err_t rsb__do_switch_to_halfword_coo(struct rsb_mtx_t * mtxAp);
rsb_err_t rsb__do_switch_to_fullword_coo(struct rsb_mtx_t * mtxAp);
#if 0
rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spmv_aa_double_sym(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);
rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spmv_aa_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);
rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spsv_uxua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);
#endif


rsb_err_t rsb__do_is_candidate_size_for_halfword(rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_nnz_idx_t nnz, rsb_flags_t flags);
rsb_err_t rsb__do_is_candidate_size_for_halfword_csr(rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_nnz_idx_t nnz, rsb_flags_t flags);
rsb_err_t rsb__do_is_candidate_for_halfword_csr(const struct rsb_mtx_t * mtxAp);
#define RSB_FLAG_USE_FULLWORD_INDICES	0x00000000
rsb_err_t rsb__do_switch_leaf(struct rsb_mtx_t * mtxAp, rsb_fmt_t matrix_storage, rsb_flags_t flags, rsb_coo_idx_t roff, rsb_coo_idx_t coff, rsb_coo_idx_t *TA);
rsb_err_t rsb__do_switch_to_halfword_csr(struct rsb_mtx_t * mtxAp);
rsb_err_t rsb__do_switch_to_fullword_csr(struct rsb_mtx_t * mtxAp);

#define RSB_COO_HALFWORDS_VALUES_PACK(LI,LJ)	((LJ)|((LI)<<RSB_COO_HALF_BITS_SIZE))/* logical row and column index pack   FIXME */
#define RSB_COO_HALFWORDS_VALUES_UNPACK_LI(LIJ)	((((LIJ))>>RSB_COO_HALF_BITS_SIZE)&~((-1)<<RSB_COO_HALF_BITS_SIZE))	/* logical row index unpack FIXME */
#define RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(LIJ)	(((LIJ) &~((-1)<<RSB_COO_HALF_BITS_SIZE)))	/* logical row index unpack FIXME */
void rsb__do_switch_array_to_halfword_coo(rsb_coo_idx_t  *p, rsb_nnz_idx_t n, const rsb_half_idx_t off);
void rsb__do_switch_array_to_fullword_coo(rsb_half_idx_t *p, rsb_nnz_idx_t n, const rsb_coo_idx_t off);


rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spmv_unua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spmv_aa_double_sym(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spmv_aa_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spsv_uxua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff);
rsb_err_t rsb_do_is_candidate_for_fullword_coo(const struct rsb_mtx_t * mtxAp);
#endif /* RSB_SWT_H_INCLUDED */
/* @endcond */
