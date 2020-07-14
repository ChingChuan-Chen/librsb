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
/* @cond INNERDOC  */
/*!
 * @file
 * @author Michele Martone
 * @brief This source file contains experimental functions 
 * */
#include "rsb_internals.h"		/* */
#include "rsb_swt.h"		/* */
#define RSB_INNER_CAST(X) (X)

rsb_err_t rsb__do_switch_leaf(struct rsb_mtx_t * mtxAp, rsb_fmt_t matrix_storage, rsb_flags_t flags, rsb_coo_idx_t roff, rsb_coo_idx_t coff, rsb_coo_idx_t *TA)
{
	/*
	 * In place switch of rows ordered COO to COO or CSR, either halfword-compressed or not.
	 * Does not require submatrix bounds to be computed.
	 * If *TA, no allocations shall originate from here.
	 * If a reasonable conversion (e.g. no to-CSR conversion with nnzA<nrA+1) is being requested, (sizeof(rsb_coo_idx_t) * RSB_MIN(mtxMp->nnz,1+mtxMp->nr) ) should suffice for TA.
	 * TODO: this function calls OpenMP-enabled functions (e.g.: rsb__util_compress_to_row_pointers_array); fix this in an appropriate way.
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	void * VA = mtxAp->VA, *IA = mtxAp->bpntr, *JA = mtxAp->bindx;
	rsb_nnz_idx_t nnzA = mtxAp->nnz;
	rsb_coo_idx_t nrA = mtxAp->nr, ncA = mtxAp->nc;

	/* RSB_STDOUT("switch with off %d/%d, flag %d, ms %d.\n", roff, coff, flags & RSB_FLAG_USE_HALFWORD_INDICES, matrix_storage); */

	if(matrix_storage == RSB_MATRIX_STORAGE_AUTO )
	{
		matrix_storage = RSB_MATRIX_STORAGE_BCOR;
		if( nnzA >= nrA+1 && nnzA >= ncA+1 )
			matrix_storage = RSB_MATRIX_STORAGE_BCSR;

		/*
		RSB_DO_FLAG_DEL(flags,RSB_FLAG_USE_HALFWORD_INDICES);
		matrix_storage = RSB_MATRIX_STORAGE_BCOR;
		*/

		/*
		 * Todo: enable:
		 *
		if( RSB_INDICES_FIT_IN_HALFWORD(nrA, ncA))
			RSB_DO_FLAG_ADD(flags,RSB_FLAG_USE_HALFWORD_INDICES);
		*/
	}

	if(!RSB_INDICES_FIT_IN_HALFWORD(nrA, ncA))
		RSB_DO_FLAG_DEL(flags,RSB_FLAG_USE_HALFWORD_INDICES);

	switch(matrix_storage)
	{
		case( RSB_MATRIX_STORAGE_BCSR ):	/* ... -> CSR */
		if( roff != 0 || coff != 0 )
		{
			errval = RSB_ERR_BADARGS;
			RSB_PERR_GOTO(err,RSB_ERRM_ES);
		}
		if( nnzA < nrA+1 )
		{
			errval = RSB_ERR_BADARGS;
			RSB_PERR_GOTO(err,RSB_ERRM_ES);
		}
		switch(flags & RSB_FLAG_USE_HALFWORD_INDICES)
		{
			case(RSB_FLAG_USE_HALFWORD_INDICES): /* ... -> HCSR */

			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCSR) /* CSR -> HCSR */
			{
				/* row pointers are ok */
				if(!(mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
					rsb__do_switch_array_to_halfword_coo(JA,nnzA,0);
				if( (mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
					; /* columns indices are ok */
			}
			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCOR) /* COO -> HCSR */
			{
				if( (mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
				{
					rsb__do_switch_array_to_fullword_coo(RSB_INNER_CAST(rsb_half_idx_t*) IA,nnzA,0);
				}
				errval = rsb__util_compress_to_row_pointers_array(TA,nnzA,nrA,RSB_FLAG_C_INDICES_INTERFACE,RSB_FLAG_C_INDICES_INTERFACE,IA);
				if(!(mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
				{
					rsb__do_switch_array_to_halfword_coo(JA,nnzA,0);
				}
			}
			mtxAp->matrix_storage = RSB_MATRIX_STORAGE_BCSR;
			RSB_DO_FLAG_SUBST(mtxAp->flags,RSB_FLAG_DEFAULT_COO_MATRIX_FLAGS,RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS|RSB_FLAG_USE_HALFWORD_INDICES);
			RSB_DO_FLAG_ADD(mtxAp->flags,(RSB_FLAG_USE_HALFWORD_INDICES_CSR));
			break;

			case(RSB_FLAG_USE_FULLWORD_INDICES):	/* -> FCSR */

			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCSR) /* CSR -> FCSR */
			{
				/* row pointers are ok */
				if(!(mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
					; /* all done: CSR -> FCSR */
				if( (mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
					rsb__do_switch_array_to_fullword_coo(RSB_INNER_CAST(rsb_half_idx_t*) JA,nnzA,0); /* HCSR -> FCSR */
			}

			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCOR) /* COO -> FCSR */
			{
				if( (mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES)) /* HCOO -> FCSR */
				{
					rsb__do_switch_array_to_fullword_coo(RSB_INNER_CAST(rsb_half_idx_t*) IA,nnzA,0); /* HCSR -> FCSR */
					rsb__do_switch_array_to_fullword_coo(RSB_INNER_CAST(rsb_half_idx_t*) JA,nnzA,0); /* HCSR -> FCSR */
				}
 				/* FCOO -> FCSR */
				errval = rsb__util_compress_to_row_pointers_array(TA,nnzA,nrA,RSB_FLAG_C_INDICES_INTERFACE,RSB_FLAG_C_INDICES_INTERFACE,IA);
			}
			mtxAp->matrix_storage = RSB_MATRIX_STORAGE_BCSR;
			RSB_DO_FLAG_SUBST(mtxAp->flags,RSB_FLAG_DEFAULT_COO_MATRIX_FLAGS|RSB_FLAG_USE_HALFWORD_INDICES,RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS);
			RSB_DO_FLAG_ADD(mtxAp->flags,(RSB_FLAG_USE_CSR_RESERVED)); /* ! */
			break;
			default:
			errval = RSB_ERR_UNIMPLEMENTED_YET;
			RSB_PERR_GOTO(err,RSB_ERRM_ES);
			break;
		}
		if(nnzA)
		{
			RSB_ASSERT( mtxAp->bpntr[0] == 0 );
			RSB_ASSERT( mtxAp->bpntr[nrA] == nnzA );
		}
		break;

		case( RSB_MATRIX_STORAGE_BCOR ): /* COO -> ... */
		switch(flags & RSB_FLAG_USE_HALFWORD_INDICES) /* COO -> H... */
		{
			case(RSB_FLAG_USE_HALFWORD_INDICES):	/* -> HCOO */
			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCSR)
			{
				errval = rsb__do_switch_compressed_array_to_fullword_coo(IA,nrA,roff,TA);
				rsb__do_switch_array_to_halfword_coo(IA,nnzA,0);
			}
			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCOR)
			{
				if( (mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
					rsb__util_hcoo_array_add(RSB_INNER_CAST(rsb_half_idx_t*) IA,nnzA,roff);
				else
					rsb__do_switch_array_to_halfword_coo(IA,nnzA,roff);
			}

			if(!(mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
			{
				rsb__do_switch_array_to_halfword_coo(JA,nnzA,coff);
			}
			else
			if( (mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES))
			{
				rsb__util_hcoo_array_add(RSB_INNER_CAST(rsb_half_idx_t*) JA,nnzA,coff);
			}
			mtxAp->matrix_storage = RSB_MATRIX_STORAGE_BCOR;
			RSB_DO_FLAG_SUBST(mtxAp->flags,RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS,RSB_FLAG_DEFAULT_COO_MATRIX_FLAGS|RSB_FLAG_USE_HALFWORD_INDICES_COO);
			break;

			case(RSB_FLAG_USE_FULLWORD_INDICES):	/* -> FCOO */
			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCSR)
			{
				errval = rsb__do_switch_compressed_array_to_fullword_coo(IA,nrA,roff,TA);
			}
			if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCOR)
			{
				if(mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES)
					rsb__do_switch_array_to_fullword_coo(RSB_INNER_CAST(rsb_half_idx_t*) IA,nnzA,roff);
				else
					rsb__util_coo_array_add(IA,nnzA,roff);
			}
			if(mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES)
				rsb__do_switch_array_to_fullword_coo(RSB_INNER_CAST(rsb_half_idx_t*) JA,nnzA,coff);
			else
				rsb__util_coo_array_add(JA,nnzA,coff);
			mtxAp->matrix_storage = RSB_MATRIX_STORAGE_BCOR;
			RSB_DO_FLAG_SUBST(mtxAp->flags,RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS|RSB_FLAG_USE_HALFWORD_INDICES,RSB_FLAG_DEFAULT_COO_MATRIX_FLAGS);
			break;
			default:
			errval = RSB_ERR_UNIMPLEMENTED_YET;
			RSB_PERR_GOTO(ret,RSB_ERRM_ES);
			break;
		}
		break;
	}
ret:
	RSB_DO_FLAG_DEL(mtxAp->flags,RSB_FLAG_QUAD_PARTITIONING);
err:
	return errval;
}

rsb_bool_t rsb__do_is_candidate_size_for_halfword_coo(rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_flags_t flags)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
#if 0
{
	rsb_coo_idx_t i,j,ij;
	i=m;
	j=k;
	ij = RSB_COO_HALFWORDS_VALUES_PACK(i,j);
	i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
	j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
	RSB_INFO("(%d %d) -> (%d %d) (%d)\n",m,k,i,j,ij);
}
#endif
	rsb_bool_t is = RSB_BOOL_FALSE;

	if( RSB_DO_FLAG_HAS(flags,RSB_FLAG_USE_HALFWORD_INDICES_COO))
		is=(!RSB_IS_COO_VALUE_MORE_THAN_HALF_BITS_LONG(m) && !RSB_IS_COO_VALUE_MORE_THAN_HALF_BITS_LONG(k));
	else
		is = RSB_BOOL_FALSE;
	return is;
}

rsb_bool_t rsb__do_is_candidate_size_for_halfword_csr(rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_nnz_idx_t nnz, rsb_flags_t flags)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	rsb_bool_t is = RSB_BOOL_FALSE;
	if( RSB_DO_FLAG_HAS(flags,RSB_FLAG_USE_HALFWORD_INDICES))
		is=(/*!RSB_IS_COO_VALUE_MORE_THAN_HALF_BITS_LONG(m) && */!RSB_IS_COO_VALUE_MORE_THAN_HALF_BITS_LONG(k));
	else
		is = RSB_BOOL_FALSE;
	return is;
}

rsb_bool_t rsb__do_is_candidate_size_for_halfword(rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_nnz_idx_t nnz, rsb_flags_t flags)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	rsb_bool_t is = RSB_BOOL_FALSE;
	is = rsb__do_is_candidate_size_for_halfword_csr(m,k,nnz,flags) || rsb__do_is_candidate_size_for_halfword_coo(m,k,flags);
	return is;
}

rsb_err_t rsb_do_is_candidate_for_fullword_coo(const struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	rsb_bool_t is = RSB_BOOL_FALSE;
	if(!mtxAp || !rsb__is_terminal_recursive_matrix(mtxAp) || !rsb__is_css_matrix(mtxAp) /* || rsb__is_not_unsymmetric(mtxAp)*/)
		return RSB_BOOL_FALSE;

	if( RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_WANT_COO_STORAGE))
		is = RSB_BOOL_TRUE;
	else
		is = RSB_BOOL_FALSE;
	return is;
}

rsb_err_t rsb__do_is_candidate_for_halfword_coo(const struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	if(!mtxAp || !rsb__is_terminal_recursive_matrix(mtxAp) || !rsb__is_coo_matrix(mtxAp) )
		return RSB_BOOL_FALSE;

	if((mtxAp->nnz/mtxAp->Mdim) > RSB_CONST_MIN_NNZ_PER_ROW_FOR_COO_SWITCH)
		return RSB_BOOL_FALSE;

	return rsb__do_is_candidate_size_for_halfword_coo(mtxAp->nr,mtxAp->nc,mtxAp->flags);
}

rsb_err_t rsb__do_is_candidate_for_halfword_csr(const struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	if(!mtxAp || !rsb__is_terminal_recursive_matrix(mtxAp) || (!rsb__is_css_matrix(mtxAp)/* || rsb__is_not_unsymmetric(mtxAp)*/
			&& !rsb__is_bcss_matrix(mtxAp)))
		return RSB_BOOL_FALSE;

	return rsb__do_is_candidate_size_for_halfword_csr(mtxAp->nr,mtxAp->nc,mtxAp->nnz,mtxAp->flags);
}

void rsb__do_switch_array_to_fullword_coo(rsb_half_idx_t *hp, rsb_nnz_idx_t n, const rsb_coo_idx_t off)
{
        /*! 
         * \ingroup gr_experimental
         * */
#if 0
        /* FIXME: with icc -fast, this produce bad results (on an array of length 2 with [0,1], produces zeros)! */
        rsb_coo_idx_t *p=(rsb_coo_idx_t*)hp;
        register rsb_nnz_idx_t k;
        for(k=n;k>0;--k)
                p[k-1]=(rsb_coo_idx_t) hp[k-1];
#else
#if !defined(__INTEL_COMPILER)
	register	/* with debug compile mode on, icc -O0 had problems here, too */ 
#endif /* __INTEL_COMPILER */
        rsb_nnz_idx_t k;
	
        if(n<1)
		return;
	if(off==0)
#if defined(__INTEL_COMPILER)
	/* using Intel(R) C Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.0.0.084 Build 20101006 we noticed a wrong operation (zeroes and/or junk ones were computed), if not using the 'novector' pragma. */
	#pragma novector
#endif /* __INTEL_COMPILER */
        for(k=n;RSB_LIKELY(k>1);--k)
        {   
                ((rsb_coo_idx_t*)hp)[k-1]=hp[k-1];
        }   
	else
#if defined(__INTEL_COMPILER)
	/* using Intel(R) C Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.0.0.084 Build 20101006 we noticed a wrong operation (zeroes and/or junk ones were computed), if not using the 'novector' pragma. */
	#pragma novector
#endif /* __INTEL_COMPILER */
        for(k=n;RSB_LIKELY(k>1);--k)
        {   
                ((rsb_coo_idx_t*)hp)[k-1]=off+hp[k-1];
        }   
        ((rsb_coo_idx_t*)hp)[0]=off+hp[0];
#endif
}

void rsb__do_switch_array_to_halfword_coo(rsb_coo_idx_t *p, rsb_nnz_idx_t n, const rsb_half_idx_t off)
{
	/*!
	 * \ingroup gr_experimental
	 * */
	rsb_half_idx_t *hp=(rsb_half_idx_t*)p;
	register rsb_nnz_idx_t k;
	if(off)
	for(k=0;RSB_LIKELY(k<n);++k)
		hp[k]=((rsb_half_idx_t)p[k])+off;
	else
	for(k=0;RSB_LIKELY(k<n);++k)
		hp[k]=((rsb_half_idx_t)p[k]);
}

rsb_err_t rsb__do_switch_to_halfword_csr(struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	if(!mtxAp || !rsb__do_is_candidate_for_halfword_csr(mtxAp))
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
/*	RSB_INFO("HCSR for %d %d\n",mtxAp->roff,mtxAp->coff); */
	rsb__do_switch_array_to_halfword_coo(mtxAp->bindx,mtxAp->nnz,0);
	RSB_DO_FLAG_SUBST(mtxAp->flags,RSB_FLAG_MUTUALLY_EXCLUSIVE_SWITCHES,RSB_FLAG_USE_HALFWORD_INDICES_CSR);
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__do_switch_to_halfword_coo(struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	if(!mtxAp || !rsb__do_is_candidate_for_halfword_coo(mtxAp))
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
#if 0
  	/* RSB_INFO("HCOO for %d %d\n",mtxAp->roff,mtxAp->coff); */
	for(i=0;i<mtxAp->Mdim;++i)
	{
		for(k=mtxAp->bpntr[i];k<mtxAp->bpntr[i+1]  ;++k)
		{
		       	j=mtxAp->bindx[k];
			ij = RSB_COO_HALFWORDS_VALUES_PACK(i,j);
			mtxAp->bindx[k]=ij;
#if 0
			RSB_ASSERT(RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij)==i);
			RSB_ASSERT(RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij)==j);
#endif
		}
	}
#else
	rsb__do_switch_array_to_halfword_coo(mtxAp->bindx,mtxAp->nnz,0);
	rsb__do_switch_array_to_halfword_coo(mtxAp->bpntr,mtxAp->nnz,0);
#endif
	RSB_DO_FLAG_SUBST(mtxAp->flags,RSB_FLAG_MUTUALLY_EXCLUSIVE_SWITCHES,RSB_FLAG_USE_HALFWORD_INDICES_COO);
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__do_switch_to_fullword_csr(struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * TODO:RENAME: rsb__do_switch_to_fullword_csr -> rsb__mtx_rsb2csr
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_half_idx_t *hbindx;

	if(!mtxAp || !rsb__do_is_candidate_for_halfword_csr(mtxAp))
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
	hbindx=(rsb_half_idx_t*)mtxAp->bindx;

	rsb__do_switch_array_to_fullword_coo(hbindx,mtxAp->nnz,0);
	RSB_DO_FLAG_DEL(mtxAp->flags,RSB_FLAG_MUTUALLY_EXCLUSIVE_SWITCHES);
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__do_switch_to_fullword_coo(struct rsb_mtx_t * mtxAp)
{
	/*!
	 * \ingroup gr_internals
	 * TODO: get rid of this.
	 * TODO:RENAME: rsb__do_switch_to_fullword_coo -> rsb__do_switch_to_fullword_csr_from_halfword_coo/rsb__mtx_rsb2coo
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	register rsb_nnz_idx_t k;

	if(!mtxAp || !rsb__do_is_candidate_for_halfword_coo(mtxAp))
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	for(k=0;k<mtxAp->nnz;++k)
	{
		mtxAp->bindx[k]=RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(mtxAp->bindx[k]);
	}
	mtxAp->bindx[mtxAp->nnz]=0;
	RSB_DO_FLAG_DEL(mtxAp->flags,RSB_FLAG_MUTUALLY_EXCLUSIVE_SWITCHES);
err:
	RSB_DO_ERR_RETURN(errval)
}

#if 0

rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spmv_unua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_internals
	 * FIXME : UNFINISHED,EXPERIMENTAL
	 *

	if (flags & RSB_FLAG_USE_HALFWORD_INDICES_COO)
	in rsb_BCSR_spmv_uaua_double_N_r1_c1_u_U :
	if(rsb__do_is_candidate_size_for_halfword_coo(Mdim,mdim))
		return rsb__do_EXPERIMENTAL_halfword_coo_spmv_uaua_double( VA, rhs, out, Mdim, mdim, bindx, bpntr, indptr, rpntr, cpntr, br, bc, roff, coff);

	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j,ij;
	double acc=0;
	register rsb_coo_idx_t i0;
	nnz=bpntr[Mdim];

#if 0
	for(k=0;k<nnz;++k)
	{
		ij=bindx[k];
		j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
		out[i]-=rhs[j]*VA[k];
	}
#else

	if(nnz<1)
		goto err;
	k=0;
	ij=bindx[k];
	i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
	j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
	i0=i;

	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		acc=0;
		for(;i==i0;)
		{
			acc += rhs[j]*VA[k];
			++k;
			ij=bindx[k];
			i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
			j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		}
		out[i0]-=acc;
		i0=i;
	}
#endif
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spmv_uaua_double_sym(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_experimental
	 * FIXME : EXPERIMENTAL
	 *

	if (flags & RSB_FLAG_USE_HALFWORD_INDICES_COO)
	in rsb_BCSR_spmv_uaua_double_N_r1_c1_u_U :
	if(rsb__do_is_candidate_size_for_halfword_coo(Mdim,mdim))
		return rsb__do_EXPERIMENTAL_halfword_coo_spmv_uaua_double( VA, rhs, out, Mdim, mdim, bindx, bpntr, indptr, rpntr, cpntr, br, bc, roff, coff);

	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j,ij;
	double acc=0,lacc=0;
	double tacc=0,tlacc=0;
	register rsb_coo_idx_t i0,j0;
	nnz=bpntr[Mdim];

#if 0
	for(k=0;k<nnz;++k)
	{
		ij=bindx[k];
		j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
		out[i]+=rhs[j]*VA[k];
	}
#else

	if(nnz<1)
		goto err;
	k=0;
	ij=bindx[k];
	i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
	j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
	i0=i;

	if(roff==coff)
	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		tacc=0;acc=0;lacc=0;tlacc=0;
		for(;i==i0;)
		{
			lacc =rhs[j]*VA[k];
			tlacc = rhs[i]*VA[k];
			acc +=lacc;
			++k;
			ij=bindx[k];
			i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
			j0=j;
			j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
			out[j0]+=tlacc;
		}
		if(i0==j0)
			acc -= lacc,// on diag diagonal
			tacc-=tlacc;// on diag diagonal
		out[i0]+= acc;
		i0=i;
	}
	else
	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		double * tout=(out+coff)-roff;
		tacc=0;acc=0;lacc=0;tlacc=0;
		for(;i==i0;)
		{
			const double * trhs=(rhs+roff)-coff;
			lacc = rhs[j]*VA[k];
			tlacc=trhs[i]*VA[k];
			acc +=lacc;
			++k;
			ij=bindx[k];
			i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
			j0=j;
			j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
			tout[j0]+=tlacc;
		}
		out[i0] += acc;
		i0=i;
	}
#endif
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spmv_uaua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_experimental
	 * FIXME : EXPERIMENTAL
	 *

	if (flags & RSB_FLAG_USE_HALFWORD_INDICES_COO)
	in rsb_BCSR_spmv_uaua_double_N_r1_c1_u_U :
	if(rsb__do_is_candidate_size_for_halfword_coo(Mdim,mdim))
		return rsb__do_EXPERIMENTAL_halfword_coo_spmv_uaua_double( VA, rhs, out, Mdim, mdim, bindx, bpntr, indptr, rpntr, cpntr, br, bc, roff, coff);

	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j,ij;
	nnz=bpntr[Mdim];

#if 0
	for(k=0;k<nnz;++k)
	{
		ij=bindx[k];
		j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
		out[i]+=rhs[j]*VA[k];
	}
#else
	double acc=0;
	register rsb_coo_idx_t i0;

	if(nnz<1)
		goto err;
	k=0;
	ij=bindx[k];
	i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
	j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
	i0=i;

	while(k<nnz)
	//while(ij != RSB_MARKER_COO_VALUE)
	{
		acc=0;
		for(;i==i0;)
		{
			acc += rhs[j]*VA[k];
			++k;
			ij=bindx[k];
			i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
			j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		}
		out[i0]+=acc;
		i0=i;
	}
#endif
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_halfword_coo_spsv_uxua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_internals
	 * FIXME : UNFINISHED,EXPERIMENTAL
	 *

	if (flags & RSB_FLAG_USE_HALFWORD_INDICES_COO)
	in rsb_BCSR_spmv_uaua_double_N_r1_c1_u_U :
	if(rsb__do_is_candidate_size_for_halfword_coo(Mdim,mdim))
		return rsb__do_EXPERIMENTAL_halfword_coo_spmv_uaua_double( VA, rhs, out, Mdim, mdim, bindx, bpntr, indptr, rpntr, cpntr, br, bc, roff, coff);

	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j,ij;
#if 0
	nnz=bpntr[Mdim];
	for(k=0;k<nnz;++k)
	{
		ij=bindx[k];
		j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
		out[i]+=rhs[j]*VA[k];
	}
#else
	register double acc=0;
	register rsb_coo_idx_t i0;
	nnz=bpntr[Mdim];

	if(nnz<1)
		goto err;
	k=0;
	ij=bindx[k];
	i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
	j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
	i0=i;
	RSB_ASSERT(!i);
	RSB_ASSERT(!j);

	out[i]=(out[i])/VA[k];

	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		acc=0;
		for(;j<i;)
		{
			acc += rhs[j]*VA[k];
	//		RSB_INFO("%d %d\n",i,j);
			++k;
			ij=bindx[k];
			i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
			j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		}
		/* j==i */
//		RSB_ASSERT(j==i);
		out[i]=(out[i]-acc)/VA[k];
	//	RSB_INFO("%d %d\n",i,j);

		++k;
		ij=bindx[k];
		i = RSB_COO_HALFWORDS_VALUES_UNPACK_LI(ij);
		j = RSB_COO_HALFWORDS_VALUES_UNPACK_UJ(ij);
		i0=i;
	}
#endif
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spmv_unua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_internals
	 * FIXME : UNFINISHED,EXPERIMENTAL
	 *
	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j;
	double acc=0;
	register rsb_coo_idx_t i0;
	register rsb_coo_idx_t * IA;
	register rsb_coo_idx_t * JA;
	nnz=*indptr;//FIXME: a trick

	if(nnz<1)
		goto err;
	k=0;
	i=IA[k];
	j=JA[k];
	i0=i;

	while(k<nnz)
	{
		acc=0;
		for(;i==i0;)
		{
			acc += rhs[j]*VA[k];
			++k;
			i=IA[k];
			j=JA[k];
		}
		out[i0]-=acc;
		i0=i;
	}
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spmv_uaua_double_sym(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_experimental
	 * FIXME : EXPERIMENTAL
	 *
	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j;
	double acc=0,lacc=0;
	double tacc=0,tlacc=0;
	register rsb_coo_idx_t i0,j0;
	register rsb_coo_idx_t * IA;
	register rsb_coo_idx_t * JA;
	nnz=*indptr;//FIXME: a trick

	if(nnz<1)
		goto err;
	k=0;
	i=IA[k];
	j=JA[k];
	i0=i;

	if(roff==coff)
	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		tacc=0;acc=0;lacc=0;tlacc=0;
		for(;i==i0;)
		{
			lacc =rhs[j]*VA[k];
			tlacc = rhs[i]*VA[k];
			acc +=lacc;
			++k;
			i=IA[k];
			j0=j;
			j=JA[k];
			out[j0]+=tlacc;
		}
		if(i0==j0)
			acc -= lacc,// on diag diagonal
			tacc-=tlacc;// on diag diagonal
		out[i0]+= acc;
		i0=i;
	}
	else
	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		double * tout=(out+coff)-roff;
		tacc=0;acc=0;lacc=0;tlacc=0;
		for(;i==i0;)
		{
			const double * trhs=(rhs+roff)-coff;
			lacc = rhs[j]*VA[k];
			tlacc=trhs[i]*VA[k];
			acc +=lacc;
			++k;
			i=IA[k];
			j0=j;
			j=JA[k];
			tout[j0]+=tlacc;
		}
		out[i0] += acc;
		i0=i;
	}
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spmv_uaua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_experimental
	 * FIXME : EXPERIMENTAL
	 *
	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j;
	register rsb_coo_idx_t * IA;
	register rsb_coo_idx_t * JA;
	double acc=0;
	register rsb_coo_idx_t i0;
	nnz=*indptr;//FIXME: a trick

	if(nnz<1)
		goto err;
	k=0;
	i=IA[k];
	j=JA[k];
	i0=i;

	while(k<nnz)
	//while(ij != RSB_MARKER_COO_VALUE)
	{
		acc=0;
		for(;i==i0;)
		{
			acc += rhs[j]*VA[k];
			++k;
			i=IA[k];
			j=JA[k];
		}
		out[i0]+=acc;
		i0=i;
	}
err:
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__do_EXPERIMENTAL_fullword_coo_spsv_uxua_double(
	const double * restrict VA, const double * restrict rhs, double * restrict out,
	const rsb_coo_idx_t  Mdim, const rsb_coo_idx_t  mdim, const rsb_nnz_idx_t * restrict bindx, const rsb_nnz_idx_t * restrict bpntr, const rsb_nnz_idx_t *restrict indptr, const rsb_coo_idx_t * restrict rpntr, const rsb_coo_idx_t * restrict cpntr, const rsb_coo_idx_t br, const rsb_coo_idx_t bc, const rsb_coo_idx_t roff, const rsb_coo_idx_t coff)
{
	/*!
	 * \ingroup gr_internals
	 * FIXME : UNFINISHED,EXPERIMENTAL
	 *
	 *
	 * */
	register rsb_nnz_idx_t k,nnz;
	register rsb_coo_idx_t i,j;
	register double acc=0;
	register rsb_coo_idx_t i0;
	register rsb_coo_idx_t * IA;
	register rsb_coo_idx_t * JA;
	nnz=*indptr;//FIXME: a trick

	if(nnz<1)
		goto err;
	k=0;
	i=IA[k];
	j=JA[k];
	i0=i;
	RSB_ASSERT(!i);
	RSB_ASSERT(!j);

	out[i]=(out[i])/VA[k];

	//while(ij != RSB_MARKER_COO_VALUE)
	while(k<nnz)
	{
		acc=0;
		for(;j<i;)
		{
			acc += rhs[j]*VA[k];
	//		RSB_INFO("%d %d\n",i,j);
			++k;
			i=IA[k];
			j=JA[k];
		}
		/* j==i */
//		RSB_ASSERT(j==i);
		out[i]=(out[i]-acc)/VA[k];
	//	RSB_INFO("%d %d\n",i,j);

		++k;
		i=IA[k];
		j=JA[k];
		i0=i;
	}
err:
	return RSB_ERR_NO_ERROR;
}


#endif
/* @endcond */
