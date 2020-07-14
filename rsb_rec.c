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
/**
 * @file
 * @brief Recursion handling code
 * @author Michele Martone
 * */
#include "rsb_common.h"
#include <string.h>	/*memcmp, strchr*/

RSB_INTERNALS_COMMON_HEAD_DECLS

#define RSB_MTX_CMP_NNZ_ASC 0 /* ascending  */
#define RSB_MTX_CMP_NNZ_DES 1 /* descending */
#define RSB_MERGE_USE_TMP_COOMTX 1 /* */

/* Macros to locate a free submatrix pointer after merging has carved holes in the submatrices array. */
#define RSB_REC_FREE_SUBM_FLAG (!0x0) /* 0x0 is forbidden -- because it would rule out zeroing of the struct in RSB_MTX_INIT_LEAF ! */
#define RSB_REC_USED_SUBM_FLAG (!(RSB_REC_FREE_SUBM_FLAG))  /* Anything different from RSB_REC_FREE_SUBM_FLAG */
#define RSB_REC_MARK_SUBM_FREE(SM) if(SM)((SM)->flags=RSB_REC_FREE_SUBM_FLAG);(SM)=NULL; /* FIXME: for safety, one might BZERO former leaves here. */
#define RSB_REC_IS_SUBM_FREE(SM) ((SM)->flags==RSB_REC_FREE_SUBM_FLAG)
#define RSB_REC_MARK_SUBM_USED(SM) if(SM)(SM)->flags=RSB_REC_USED_SUBM_FLAG;

int rsb__compar_rcsr_matrix_for_spsvl(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
		Useful for Lower Triangular Solve.
	*/
	struct rsb_translated_matrix_t *mtxAp = (struct rsb_translated_matrix_t*)ap;
	struct rsb_translated_matrix_t *mtxBp = (struct rsb_translated_matrix_t*)bp;
	rsb_coo_idx_t aro = mtxAp->roff, aco = mtxAp->coff, ar = mtxAp->nr;
	rsb_coo_idx_t bro = mtxBp->roff, bco = mtxBp->coff, br = mtxBp->nr;

	// the one who ends before the other beginning wins
	if(  aro+ar <= bro )
		return -1;
	if(  bro+br <= aro )
		return 1;
	// the one beginning later comes later, unless the other matrix is on the diagonal
	if(  aro > bro )
		return bro==bco?-1:1;
	if(  aro < bro )
		return aro==aco?1:-1;
	// if aligned, the one beginning later comes after
	if(  aco > bco )
		return 1;
	else
		return -1;
}

static int rsb_compar_rcsr_matrix_for_get_csr(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
	*/
	struct rsb_translated_matrix_t *mtxAp = (struct rsb_translated_matrix_t*)ap;
	struct rsb_translated_matrix_t *mtxBp = (struct rsb_translated_matrix_t*)bp;
	rsb_coo_idx_t aro = mtxAp->roff, aco = mtxAp->coff;
	rsb_coo_idx_t bro = mtxBp->roff, bco = mtxBp->coff;

	if(  aro > bro )
		return 1;
	if(  aro < bro )
		return -1;
	if(  aco > bco )
		return 1;
	if(  aco < bco )
		return -1;
	return 0;
}

static int rsb_compar_rcsr_matrix_for_spsvut(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
		Useful for Lower Triangular Solve Transposed.

		it's in a way similar to
		rsb__compar_rcsr_matrix_for_spsvl, with substitutions:
			aro -> bco+bc
			bro -> aro+ar
			aco -> bco+bc
			bco -> aro+ar
			... <viceversa>
	*/
	struct rsb_translated_matrix_t *mtxAp = (struct rsb_translated_matrix_t*)ap;
	struct rsb_translated_matrix_t *mtxBp = (struct rsb_translated_matrix_t*)bp;
	rsb_coo_idx_t aro = mtxAp->roff, aco = mtxAp->coff, ac = mtxAp->nc;
	rsb_coo_idx_t bro = mtxBp->roff, bco = mtxBp->coff, bc = mtxBp->nc;

	// the one who ends before the other beginning wins
	if(  bco+bc <= aco )
		return 1;
	if(  aco+ac <= bco )
		return -1;
	// the one beginning later comes later, unless the other matrix is on the diagonal
	if(  aco > bco )
		return bro==bco?-1:1;
	if(  aco < bco )
		return aro==aco?1:-1;
	// if aligned, the one beginning later comes after
	if(  aro > bro )
		return 1;
	else
		return -1;
}

static int rsb__compar_rcsr_matrix_for_spsvlt(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
		Useful for Lower Triangular Solve Transposed.

		it's in a way similar to
		rsb__compar_rcsr_matrix_for_spsvl, with substitutions:
			aro -> bco+bc
			bro -> aro+ar
			aco -> bco+bc
			bco -> aro+ar
			... <viceversa>
	*/
	struct rsb_translated_matrix_t *mtxAp = (struct rsb_translated_matrix_t*)ap;
	struct rsb_translated_matrix_t *mtxBp = (struct rsb_translated_matrix_t*)bp;
	rsb_coo_idx_t aro = mtxAp->roff, aco = mtxAp->coff, ar = mtxAp->nr, ac = mtxAp->nc;
	rsb_coo_idx_t bro = mtxBp->roff, bco = mtxBp->coff, /*br = mtxBp->nr,*/ bc = mtxBp->nc;

	// the one who ends before the other beginning wins
	if(  aco >= bco+bc )
		return -1;
	if(  bco >= aco+ac )
		return 1;
	// the one beginning later comes later, unless the other matrix is on the diagonal
	if(  aco+ac < bco+bc )
		return bro==bco?-1:1;
	if(  aco+ac > bco+bc )
		return aro==aco?1:-1;
	// if aligned, the one beginning later comes after
	if(  aro+ar > bro+ar )
		return -1;
	else
		return 1;
}

static int rsb_compar_rcsr_matrix_for_spsvu(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
	*/
	struct rsb_translated_matrix_t *mtxAp = (struct rsb_translated_matrix_t*)ap;
	struct rsb_translated_matrix_t *mtxBp = (struct rsb_translated_matrix_t*)bp;
	rsb_coo_idx_t aro = mtxAp->roff, aco = mtxAp->coff, ar = mtxAp->nr, ac = mtxAp->nc;
	rsb_coo_idx_t bro = mtxBp->roff, bco = mtxBp->coff, br = mtxBp->nr;

	// the one who ends before the other beginning wins
	if(  aro >= bro+br )
		return -1;
	if(  bro >= aro+ar )
		return 1;
	// the one beginning later comes later, unless the other matrix is on the diagonal
	if(  aro+ar < bro+br )
		return bro==bco?-1:1;
	if(  aro+ar > bro+br )
		return aro==aco?1:-1;
	// if aligned, the one beginning later comes after
	if(  aco+ac > bco+ac )
		return -1;
	else
		return 1;
}

#define RSB_ASC_CMP_FOR_QSRT(A,B) ( ( (A) > (B) ) ? (1) : (( (A) == (B) ) ? 0 : -1) )

static int rsb_compar_nnz_idx_t(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
	*/
	rsb_nnz_idx_t a=*(rsb_nnz_idx_t*)ap;
	rsb_nnz_idx_t b=*(rsb_nnz_idx_t*)bp;

        return RSB_ASC_CMP_FOR_QSRT(a,b);
}


static int rsb__compar_mtx_nnz_des(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
		Compare submatrices pointers in descending order of nnz occupation.
	*/
	const struct rsb_mtx_t*mtxAp = *(struct rsb_mtx_t**)ap;
	const struct rsb_mtx_t*mtxBp = *(struct rsb_mtx_t**)bp;
	rsb_nnz_idx_t nnzA = mtxAp->nnz;
	rsb_nnz_idx_t nnzB = mtxBp->nnz;

	return -RSB_ASC_CMP_FOR_QSRT(nnzA,nnzB);
}

static int rsb__compar_mtx_nnz_asc(const void * ap, const void * bp)
{
	/**
		\ingroup gr_internals
		A compare function to be used with qsort.
		Compare submatrices pointers in ascending order of nnz occupation.
	*/
	const struct rsb_mtx_t*mtxAp = *(struct rsb_mtx_t**)ap;
	const struct rsb_mtx_t*mtxBp = *(struct rsb_mtx_t**)bp;
	rsb_nnz_idx_t nnzA = mtxAp->nnz;
	rsb_nnz_idx_t nnzB = mtxBp->nnz;

	return  RSB_ASC_CMP_FOR_QSRT(nnzA,nnzB);
}

rsb_err_t rsb__srt_subm_ptr_array(struct rsb_mtx_t ** mtxApp, rsb_submatrix_idx_t nsm, int criteria)
{
	/**
		\ingroup gr_internals
		Sort submatrices pointers.
		TODO: introduce other sorting criteria (e.g. index occupation, ...).
	*/
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	
	switch(criteria)
	{
		case(RSB_MTX_CMP_NNZ_ASC):
		qsort( mtxApp, (size_t) nsm, sizeof(struct rsb_mtx_t*), &rsb__compar_mtx_nnz_asc);
		break;

		case(RSB_MTX_CMP_NNZ_DES):
		qsort( mtxApp, (size_t) nsm, sizeof(struct rsb_mtx_t*), &rsb__compar_mtx_nnz_des);
		break;
	}

	return errval;
}

rsb_err_t rsb__sort_array_of_leaf_matrices_for_ussv(const struct rsb_mtx_t * mtxAp, struct rsb_translated_matrix_t *leaf_matrices, rsb_submatrix_idx_t n, rsb_trans_t transl)
{
	/**
		\ingroup gr_internals
		Sort rsb_translated_matrix_t structures.
	*/
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	if(rsb__is_upper_triangle(mtxAp->flags))
	{
		if(RSB_DOES_TRANSPOSE(transl))
			errval = rsb__sort_array_of_leaf_matrices(NULL,leaf_matrices,n,rsb_op_spsvut);
		else
			errval = rsb__sort_array_of_leaf_matrices(NULL,leaf_matrices,n,rsb_op_spsvu);
	}
	else
	if(rsb__is_lower_triangle(mtxAp->flags))
	{
		if(RSB_DOES_TRANSPOSE(transl))
			errval = rsb__sort_array_of_leaf_matrices(NULL,leaf_matrices,n,rsb_op_spsvlt);
		else
			errval = rsb__sort_array_of_leaf_matrices(NULL,leaf_matrices,n,rsb_op_spsvl);
	}
	else
	{
		/*
		RSB_ERROR(RSB_ERRM_ES);
		errval = RSB_ERR_BADARGS;
		*/
	}
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__sort_array_of_leaf_matrices(const struct rsb_translated_matrix_t *rmatrix,struct rsb_translated_matrix_t *matrices, rsb_submatrix_idx_t n, enum rsb_op_t op)
{
	/**
		\ingroup gr_internals
	  	Sorts an array of leaf matrices in an order which will be suitable for SpMV, SpSV, ... later on.
		FIXME: rmatrix is the root matrix, and is currently unused.
	*/
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_submatrix_idx_t ij;
	rsb_nnz_idx_t * idx = NULL;
	struct rsb_translated_matrix_t *smatrices=NULL;

	if(!matrices)
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_EM);
	}


	switch(op)
	{
	case(rsb_op_spmv):
	{
		/* NOTE : this code is braindead : should use qsort directly instead */
		smatrices = rsb__malloc(sizeof(struct rsb_translated_matrix_t) * n);
		idx = rsb__malloc(2*sizeof(rsb_nnz_idx_t) * n);
	
		if(!smatrices || !idx){errval = RSB_ERR_ENOMEM; RSB_PERR_GOTO(err,RSB_ERRM_EM); }
	
		for(ij=0;ij<n;++ij)
		{
			/* currently, the sorting criteria is the base row only */
			idx[2*ij+0]=matrices[ij].roff;
			idx[2*ij+1]=ij;
		}
		qsort( idx , (size_t) n, 2*sizeof(rsb_nnz_idx_t), &rsb_compar_nnz_idx_t );
		rsb__do_util_compact_permutation_nnz_idx_t_array(idx, n);
		/* permutation */
		for(ij=0;ij<n;++ij) smatrices[ij]=matrices[idx[ij]];
		rsb_memcpy(matrices,smatrices,sizeof(struct rsb_translated_matrix_t)*n);
	}
	break;
		case(rsb_op_spsvl):
		{
			qsort( matrices , (size_t) n, sizeof(struct rsb_translated_matrix_t), &rsb__compar_rcsr_matrix_for_spsvl);
		}
		break;
		case(rsb_op_spsvlt):
		{
			qsort( matrices , (size_t) n, sizeof(struct rsb_translated_matrix_t), &rsb__compar_rcsr_matrix_for_spsvlt);
		}
		break;
		case(rsb_op_spsvu):
		{
			qsort( matrices , (size_t) n, sizeof(struct rsb_translated_matrix_t), &rsb_compar_rcsr_matrix_for_spsvu);
		}
		break;
		case(rsb_op_spsvut):
		{
			qsort( matrices , (size_t) n, sizeof(struct rsb_translated_matrix_t), &rsb_compar_rcsr_matrix_for_spsvut);
		}
		break;
		case(rsb_op_get_csr):
		{
			qsort( matrices , (size_t) n, sizeof(struct rsb_translated_matrix_t), &rsb_compar_rcsr_matrix_for_get_csr);
		}
		break;
		default:
		errval = RSB_ERR_INTERNAL_ERROR;
		break;
	}
err:
	RSB_CONDITIONAL_FREE(idx);
	RSB_CONDITIONAL_FREE(smatrices);
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__fill_array_of_leaf_matrices(const struct rsb_translated_matrix_t *tmatrix, struct rsb_translated_matrix_t *matrices, rsb_submatrix_idx_t * sip)
{
	/**
		\ingroup gr_internals
		This function fills the input array with pointers to leaf matrices.
		The input order should matter to spmv performance, but it will be dealt with later.
	*/
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_submatrix_idx_t si = *sip;

	if(!tmatrix || !matrices || !tmatrix->mtxlp)
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	if(rsb__is_terminal_recursive_matrix(tmatrix->mtxlp))
	{
		/* if this is the only matrix  */
		matrices[si].mtxlp = tmatrix->mtxlp;
		matrices[si].roff = tmatrix->roff;
		matrices[si].coff = tmatrix->coff;
		matrices[si].nr = tmatrix->nr;
		matrices[si].nc = tmatrix->nc;
		matrices[si].level = tmatrix->level;
		++*sip;
	       	++si;
		goto ok;
	}
	else
	{
		/* if tmatrix has submatrices  */
		rsb_submatrix_idx_t i,j;
		struct rsb_mtx_t * submatrix = NULL;
//		rsb_coo_idx_t mB=(tmatrix->mtxlp->rpntr[rsb__recursive_middle_block_index(tmatrix->mtxlp->M_b)]);
//		rsb_coo_idx_t kB=(tmatrix->mtxlp->cpntr[rsb__recursive_middle_block_index(tmatrix->mtxlp->K_b)]);

		RSB_SUBMATRIX_FOREACH(tmatrix->mtxlp,submatrix,i,j)
		if(submatrix)
		{
			/* we update submatrices with positioning info */
			struct rsb_translated_matrix_t tsubmatrix;

			tsubmatrix.mtxlp = submatrix;
//			tsubmatrix.roff = tmatrix->roff+i*mB;
//			tsubmatrix.coff = tmatrix->coff+j*kB;
			tsubmatrix.nr = submatrix->nr;
			tsubmatrix.nc = submatrix->nc;
			tsubmatrix.roff = submatrix->roff;
			tsubmatrix.coff = submatrix->coff;
			tsubmatrix.level = tmatrix->level+1;

			errval = rsb__fill_array_of_leaf_matrices(&tsubmatrix, matrices, sip);
			if(RSB_SOME_ERROR(errval))
			{
				RSB_PERR_GOTO(err,RSB_ERRM_ES);
			}
		}
		goto ok;
	}
ok:
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__get_array_of_leaf_matrices(struct rsb_mtx_t *mtxAp, struct rsb_translated_matrix_t ** tmatricesp, rsb_submatrix_idx_t *countp)
{
	/**
		\ingroup gr_internals
	   	\return an array of leaf matrices, ordered in a way to ease workload balancing on multicore platforms.
		If *tmatricesp==NULL, will allocate it for us. Otherwise, it will use the given pointer.
	*/
	
	long lmc = 0; /* leaf matrices count */
	struct rsb_translated_matrix_t * tmatrices = NULL;
	struct rsb_translated_matrix_t tmatrix;
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_submatrix_idx_t count = 0;

	RSB_BZERO_P(&tmatrix);

	if(!tmatricesp /*|| !countp */|| !mtxAp)
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	tmatrix.mtxlp = mtxAp;
	tmatrix.roff = tmatrix.coff = 0;
	tmatrix.level = 0;

	lmc = rsb__terminal_recursive_matrix_count(mtxAp);

	if(lmc>0)
	{
	//	rsb_submatrix_idx_t i,j,ij;
	//	struct rsb_mtx_t * submatrix = NULL;

		if(*tmatricesp)
			tmatrices = *tmatricesp;
		else
		{
			tmatrices = rsb__malloc(sizeof(struct rsb_translated_matrix_t) * (lmc));
			if(!tmatrices)
			{
				errval = RSB_ERR_ENOMEM;
				RSB_PERR_GOTO(err,RSB_ERRM_ES);
			}
		}

		errval = rsb__fill_array_of_leaf_matrices(&tmatrix,tmatrices,&count);
		if(RSB_SOME_ERROR(errval))
	       	{
			RSB_PERR_GOTO(err,RSB_ERRM_ES);
		}
#if 0
		/*  sorting breaks Z ordering, really */
		errval = rsb__sort_array_of_leaf_matrices(&tmatrix,tmatrices, count, rsb_op_spmv );
		if(RSB_SOME_ERROR(errval))
		{
			RSB_PERR_GOTO(err,RSB_ERRM_ES);
	       	}
#endif
#if 0
		/* debug dump */
		for(ij=0;ij<count;++ij)
		{
			RSB_INFO("submatrix: %d @ (%d %d) (level %d) (nnz %d)\n",
				ij,tmatrices[ij].roff,tmatrices[ij].coff,tmatrices[ij].level,tmatrices[ij].mtxlp->nnz);
		}
#endif

	}
	goto ok;
ok:
	if(countp)
		*countp = count;
	else
		mtxAp->all_leaf_matrices_n = count;
	*tmatricesp = tmatrices;
	goto ret;
err:
	if(!*tmatricesp)
		RSB_CONDITIONAL_FREE(tmatrices);
ret:
	RSB_DO_ERR_RETURN(errval)
}

static rsb_err_t rsb__refresh_array_of_leaf_matrices(struct rsb_mtx_t *mtxAp)
{
	/*
	 * On error, matrix shall be unaffected.
	 * */

	rsb_err_t errval = RSB_ERR_NO_ERROR;
#if 1
	struct rsb_mtx_t mtxB  = *mtxAp;

	/* FIXME: this method is not efficient; would rather need realloc and pointers diff.  */
	mtxAp->all_leaf_matrices_n = 0;
	mtxAp->all_leaf_matrices = NULL;

	errval = rsb__get_array_of_leaf_matrices(mtxAp, &(mtxAp->all_leaf_matrices), &(mtxAp->all_leaf_matrices_n));

	if(RSB_SOME_ERROR(errval))
	{
		/* restore */
		mtxAp->all_leaf_matrices_n = mtxB.all_leaf_matrices_n;
		mtxAp->all_leaf_matrices = mtxB.all_leaf_matrices;
	}
	else
	{
		RSB_CONDITIONAL_FREE(mtxB.all_leaf_matrices);
	}

#else
		/* Note: this method (no reallocation) would be better, but is incomplete ...  */
		smu = 0;
		for(sml=0;sml<mtxAp->all_leaf_matrices_n;sml++)
			if( mtxAp->all_leaf_matrices[sml].mtxlp != NULL )
			{
				mtxAp->all_leaf_matrices[smu++] = mtxAp->all_leaf_matrices[sml];
				// mtxAp->all_leaf_matrices[smu++].mtxlp = mtxAp->all_leaf_matrices[sml].mtxlp;
			}
		printf("Merged %d leaves (from %d to %d).\n",sml-smu,sml,smu);
		mtxAp->all_leaf_matrices_n = smu;
		if(mtxAp->all_leaf_matrices_n==0)
			RSB_CONDITIONAL_FREE(mtxAp->all_leaf_matrices);
#endif

	return errval;
}
	
size_t rsb__get_index_storage_amount(const struct rsb_mtx_t *mtxAp)
{
	/**
		\ingroup gr_experimental
	   	\return the amount of allocated bytes for storage of the matrix
		NOTE: valid only for (recursive) CSR
		NOTE: we don't include the matrix struct size.
	 */
	rsb_submatrix_idx_t i,j;
	struct rsb_mtx_t * submatrix = NULL;
	size_t isa = 0;

	if(!mtxAp)
		goto done;

	if(rsb__is_terminal_recursive_matrix(mtxAp))
	{	
		size_t is;

		if(RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_USE_HALFWORD_INDICES))
			is=sizeof(rsb_half_idx_t);
		else
			is=sizeof(rsb_coo_idx_t);

//		if(RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_WANT_BCSS_STORAGE))
		if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCSR)
			isa += (is*mtxAp->nnz)+(sizeof(rsb_coo_idx_t)*(mtxAp->Mdim+1));
		else
		if(mtxAp->matrix_storage == RSB_MATRIX_STORAGE_BCOR)
//		if(RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_WANT_COO_STORAGE))
			isa += 2*(is*mtxAp->nnz);

		//isa += sizeof(struct rsb_mtx_t); // FIXME: should this be here ? NO: this is not index storage.
	}
	else
	{
		RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
		if(submatrix)
			isa += rsb__get_index_storage_amount(submatrix);
	}
done:
	return isa;
}

rsb_submatrix_idx_t rsb__get_diagonal_elements_count(const struct rsb_mtx_t *mtxAp)
{
	/**
		\ingroup gr_internals
	   	\return the number of nonzeros which are on diagonal aligned with the main diagonal
	 */
	rsb_submatrix_idx_t i,j;
	struct rsb_mtx_t * submatrix = NULL;
	rsb_submatrix_idx_t dse = 0;

	if(!mtxAp)
		goto done;

	if(rsb__is_terminal_recursive_matrix(mtxAp) && mtxAp->roff == mtxAp->coff)
	{
		dse=mtxAp->nnz;
	}
	else
	{
		RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
		if( submatrix && i==j && RSB_SUBMATRIX_IS_ON_DIAG(submatrix) )
			dse += rsb__get_diagonal_elements_count(submatrix);
	}
done:
	return dse;
}

static rsb_bool_t rsb_is_node_pre_last(const struct rsb_mtx_t *mtxAp)
{
	/* rsb_err_t errval = RSB_ERR_NO_ERROR; */
	rsb_bool_t inpl = RSB_BOOL_FALSE;

	if(rsb__is_terminal_recursive_matrix(mtxAp))
		goto ret;
	else
	{
		struct rsb_mtx_t * submatrix = NULL;
		rsb_submatrix_idx_t i,j;
		inpl = RSB_BOOL_TRUE;
		RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
		if(submatrix && !rsb__is_terminal_recursive_matrix(submatrix))
			inpl = RSB_BOOL_FALSE;
	}

ret:
	return inpl;
}

struct rsb_mtx_list_t
{
	/* size_t sa[10000];
	struct rsb_mtx_t*mp[10000]; */
	size_t*sa; /* submatrices/scores array */
	struct rsb_mtx_t**mp; /* matrices pointer array */
	rsb_submatrix_idx_t mc; /* (leaf) matrices count (0...) */
};

static rsb_err_t rsb__leaves_analysis_rec(struct rsb_mtx_t *mtxAp, struct rsb_mtx_list_t *mlp, const int wv, rsb_bool_t wpl)
{
	/**
		\ingroup gr_internals
		Analyze submatrices and compute a score.
		If (wpl) (want pre leaf) then pre-leaf groups will be considered (i.e. for merging); otherwise, leaves.
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_submatrix_idx_t i,j;
	struct rsb_mtx_t * submatrix = NULL;
	const int miac = 1; // merge in any case --- even when no saving is gained

	/* RSB_ASSERT(mtxAp->VA); RSB_ASSERT(mtxAp->bindx); RSB_ASSERT(mtxAp->bpntr); */

	if(rsb__is_terminal_recursive_matrix(mtxAp))
	{
		if(wpl)
			; /* merge case: nothing to do */
		else
		{
			/* split case */
			mlp->sa[mlp->mc  ]=mtxAp->nnz;
			mlp->mp[mlp->mc++]=mtxAp;
		}
		goto ret;
	}
	else
	{
		if(!wpl)
		{
			RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
				if(submatrix)
					errval |= rsb__leaves_analysis_rec(submatrix,mlp,wv,wpl);
			goto ret;
		}

		if(rsb_is_node_pre_last(mtxAp))
		{
			int sol = 0;
			const int vl = 2;
			rsb_coo_idx_t /*smnr[4]={0,0,0,0},*/nr=mtxAp->nr;
			rsb_coo_idx_t /*smnc[4]={0,0,0,0},*/nc=mtxAp->nc;
			rsb_nnz_idx_t /*smnz[4]={0,0,0,0},*/nz=mtxAp->nnz;
			size_t hcooio=0,hcsrio=0;
			size_t fcooio=0,fcsrio=0;
			size_t rsbio = rsb__get_index_storage_amount(mtxAp),bestio=rsbio;
			const rsb_char_t sp=' ', bettermark='.'/*, bestmark='*'*/;
			rsb_char_t hcoof=sp,hcsrf=sp,fcoof=sp,fcsrf=sp,rsbf=sp;
			double savepcnt = 0.0;

			RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
				if(submatrix)
				{
					/*
					int idx=2*i+j;
					smnr[idx]=submatrix->nr;
					smnc[idx]=submatrix->nc;
					smnz[idx]=submatrix->nnz; */
					++sol;
				}
			//sol = mtxAp->all_leaf_matrices_n;
			if(wv>vl)
			RSB_STDOUT("sub-leaf: %p is %d x %d and contains %d nnz in %d leaves ('.'=fewer indices)\n",(const void*)mtxAp,nr,nc,nz,sol),
			RSB_STDOUT("as   is:%10zu %c\n",rsbio ,rsbf);
			if(RSB_INDICES_FIT_IN_HALFWORD(nr,nc))
			{
				hcooio=sizeof(rsb_half_idx_t)*2*nz;
				hcsrio=sizeof(rsb_half_idx_t)*nz+sizeof(rsb_nnz_idx_t)*(1+nr);
				if(hcooio<rsbio )hcoof=bettermark;
				if(hcooio<bestio)bestio=hcooio;
				if(hcsrio<rsbio )hcsrf=bettermark;
				if(hcsrio<bestio)bestio=hcsrio;
				if(wv>vl)
				RSB_STDOUT("as HCOO:%10zu %c\n",hcooio,hcoof),
				RSB_STDOUT("as HCSR:%10zu %c\n",hcsrio,hcsrf);
			}
				fcooio=sizeof(rsb_coo_idx_t)*2*nz;
				if(fcooio<rsbio)fcoof=bettermark;
				if(fcooio<bestio)bestio=fcooio;
				fcsrio=sizeof(rsb_coo_idx_t)*nz+sizeof(rsb_nnz_idx_t)*(1+nr);
				if(fcsrio<rsbio )fcsrf=bettermark;
				if(fcsrio<bestio)bestio=fcsrio;
				if(wv>vl)
				RSB_STDOUT("as  COO:%10zu %c\n",fcooio,fcoof),
				RSB_STDOUT("as  CSR:%10zu %c\n",fcsrio,fcsrf);
				savepcnt=100.0*(((double)(rsbio-bestio))/(double)rsbio);
				if(savepcnt>0.0 || miac)
				{
					if(wv>vl)
					RSB_STDOUT("potential saving is: %3.2lg%% (%zu bytes out of %zu)\n",savepcnt,rsbio-bestio,rsbio);
					mlp->sa[mlp->mc  ]=rsbio-bestio;
					mlp->mp[mlp->mc++]=mtxAp;
				}
		}
		else
		{
			RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
				if(submatrix)
					rsb__leaves_analysis_rec(submatrix,mlp,wv,wpl);
		}
	}

ret:	return errval;
}

static rsb_err_t rsb__cor_merge(rsb_type_t typecode, void* RSB_RESTRICT VA, rsb_coo_idx_t * RSB_RESTRICT IA, rsb_coo_idx_t * RSB_RESTRICT JA, rsb_nnz_idx_t offB, rsb_nnz_idx_t nnzB, rsb_nnz_idx_t nnzC, const int wv, int wp, struct rsb_coo_matrix_t*RSB_RESTRICT coop)
{
	/**
	 * \ingroup gr_internals
	 * Merges two non overlapping totally ordered COO sequences.
	 * This is a naive version using a nnzB+nnzC temporary array.
	 * If coop is supplied, no allocation of a  RSB_MIN(nnzB,nnzC) buffer space will occur but coop's will be used.
	 * It would be nice to have a no-alloc version, but this can be very complicated.
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	void *VB = NULL, *VC = NULL, *VT = NULL;
	rsb_coo_idx_t * IB = NULL, *JB = NULL;
	rsb_coo_idx_t * IC = NULL, *JC = NULL;
	rsb_coo_idx_t * IT = NULL, *JT = NULL;
	rsb_nnz_idx_t bi = 0, ci = 0, ti = 0;
	rsb_nnz_idx_t b0 = 0, c0 = 0, t0 = 0;
	struct rsb_coo_matrix_t coo;
	size_t es = RSB_SIZEOF(typecode);

	if( nnzB == 0 || nnzC == 0 )
	{
		goto ret;
	}

	b0 = offB;
	c0 = offB + nnzB;
	VB = RSB_TYPED_OFF_PTR(typecode,VA,b0);
	VC = RSB_TYPED_OFF_PTR(typecode,VA,c0);
	IB = IA + b0;
	IC = IA + c0;
	JB = JA + b0;
	JC = JA + c0;

	RSB_BZERO_P(&coo);
	coo.nnz = nnzB + nnzC;
	coo.typecode = typecode;

	if( coop && coop->nnz)
	{
		coo = *coop;
		coo.nnz = nnzB + nnzC; /* necessary */
	}
	else
	{
		if( NULL == rsb__allocate_coo_matrix_t(&coo) )
			goto err;
	}

	IT = coo.IA;
	JT = coo.JA;
	VT = coo.VA;

again:
	t0 = ti;
       	while( bi<nnzB && ci<nnzC && RSB_COO_LT(IB[bi],JB[bi],IC[ci],JC[ci]) )
	{
		IT[ti] = IB[bi];
		JT[ti] = JB[bi];
		++bi,++ti;
	}
	//if(ti>t0) RSB_STDOUT("t0:%d t1:%d bi:%d ci:%d bnz:%d cnz:%d\n",t0,ti,bi,ci,nnzB,nnzC);
	if(ti>t0)
		RSB_A_MEMCPY(VT,VB,t0,(bi-(ti-t0)),(ti-t0),es);

	t0 = ti;
       	while( bi<nnzB && ci<nnzC && RSB_COO_GE(IB[bi],JB[bi],IC[ci],JC[ci]) )
	{
		IT[ti] = IC[ci];
		JT[ti] = JC[ci];
		++ci,++ti;
	}

	//if(ti>t0) RSB_STDOUT("t0:%d t1:%d bi:%d ci:%d bnz:%d cnz:%d\n",t0,ti,bi,ci,nnzB,nnzC);
	if(ti>t0)
		RSB_A_MEMCPY(VT,VC,t0,(ci-(ti-t0)),(ti-t0),es);

	if( ci < nnzC && bi < nnzB )
		goto again;

       	if( bi<nnzB && ci==nnzC )
	{
		RSB_COA_MEMCPY(IT,IB,ti,bi,(nnzB-bi));
		RSB_COA_MEMCPY(JT,JB,ti,bi,(nnzB-bi));
		RSB_A_MEMCPY  (VT,VB,ti,bi,(nnzB-bi),es);
		ti += (nnzB - bi);
		bi = nnzB;
	}

       	if( ci<nnzC && bi==nnzB )
	{
		RSB_COA_MEMCPY(IT,IC,ti,ci,(nnzC-ci));
		RSB_COA_MEMCPY(JT,JC,ti,ci,(nnzC-ci));
		RSB_A_MEMCPY  (VT,VC,ti,ci,(nnzC-ci),es);
		ti += (nnzC - ci);
		ci = nnzC;
	}

	RSB_COA_MEMCPY(IA,IT,offB,0,(coo.nnz));
	RSB_COA_MEMCPY(JA,JT,offB,0,(coo.nnz));
	if(wp)
	{
		RSB_A_MEMCPY_parallel(  VA,VT,offB,0,(coo.nnz),es);
	}
	else
	{
		RSB_A_MEMCPY(  VA,VT,offB,0,(coo.nnz),es);
	}

	RSB_ASSERT(rsb__util_is_coo_array_sorted_up_partial_order(IA,coo.nnz));
	goto done;
err:
	errval = RSB_ERR_ENOMEM;
done:
	if( coop && coop->nnz)
		;
	else
		rsb__destroy_coo_matrix_t(&coo);
ret:
	return errval;
}

rsb_err_t rsb__leaves_merge_multiple(struct rsb_mtx_t *mtxAp, rsb_time_t *stp, rsb_time_t *atp, rsb_time_t *ltp, const int wv, int kc)
{
	/* FIXME: is this used ? where ? why ? */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t st = RSB_TIME_ZERO, lt = RSB_TIME_ZERO, at = RSB_TIME_ZERO;

	while(mtxAp->all_leaf_matrices_n > 1)
	{
		rsb_time_t mst = RSB_TIME_ZERO, mlt = RSB_TIME_ZERO, mat = RSB_TIME_ZERO; /* merge: sort,elapsed,analysis time */

		errval = rsb__leaves_merge(mtxAp, mtxAp->all_leaf_matrices_n, &mst, &mat, &mlt, wv, kc);

		if(RSB_SOME_ERROR(errval))
		{
			RSB_PERR_GOTO(err, RSB_ERRM_ES);
		}
		st += mst;
	       	lt += mlt;
		at += mat;
	}

	RSB_DEBUG_ASSERT( mtxAp->all_leaf_matrices_n == 1 );

	if(kc)
	{
#ifdef RSB_USE_ASSERT
		struct rsb_mtx_t * submatrix = NULL;
		rsb_submatrix_idx_t smi;

		RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi) 
		{
			RSB_DEBUG_ASSERT( submatrix->matrix_storage == RSB_MATRIX_STORAGE_BCOR );
			RSB_DEBUG_ASSERT( RSB_DO_FLAG_HAS( submatrix->flags , RSB_FLAG_WANT_COO_STORAGE) );
		}
#endif /* RSB_USE_ASSERT */
	}

	if (mtxAp->all_leaf_matrices_n > 1)
	{
		RSB_ERROR("Merge did not work: matrix has still %d submatrices.\n", mtxAp->all_leaf_matrices_n);
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err, RSB_ERRM_ES);
	}
err:
	RSB_ASSIGN_IF(stp,st)
	RSB_ASSIGN_IF(atp,at)
	RSB_ASSIGN_IF(ltp,lt)
	return errval;
}

#if 0
static void rsb__mtx_list_print(struct rsb_mtx_list_t * mlp, const int wv)
{
	rsb_submatrix_idx_t smi;

	RSB_DEBUG_ASSERT(mlp);

	RSB_STDOUT("Selected %d matrices:\n", mlp->mc);

	for(smi=0;smi<mlp->mc;++smi)
	{
		struct rsb_mtx_t * mtxMp = mlp->mp[smi]; 
		//if(wv>1)
			RSB_STDOUT(RSB_PRINTF_MTX_SUMMARY_ARGS(mtxMp)),
			RSB_STDOUT(" -> %zd\n",mlp->sa[smi]);
	}
}
#endif

static void rsb__mtx_list_free(struct rsb_mtx_list_t * mlp)
{
	RSB_DEBUG_ASSERT(mlp);

	RSB_CONDITIONAL_FREE(mlp->mp);
	RSB_CONDITIONAL_FREE(mlp->sa);
}

static void rsb__mtx_list_init(struct rsb_mtx_list_t * mlp)
{
	RSB_DEBUG_ASSERT(mlp);

	RSB_BZERO_P(mlp);
}

static rsb_err_t rsb__mtx_list_bld(struct rsb_mtx_list_t * mlp, struct rsb_mtx_t *mtxAp)
{
	struct rsb_mtx_list_t ml; /* matrix list */
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	RSB_DEBUG_ASSERT(mtxAp);
	RSB_DEBUG_ASSERT(mlp);

	RSB_BZERO_P(&ml);

	ml.mc = rsb__submatrices(mtxAp);
	ml.sa = rsb__calloc(sizeof(*ml.sa) * ml.mc); /* after rsb__srt_subm_ptr_array, this won't make sense anymore */
	ml.mp = rsb__calloc(sizeof(*ml.mp) * ml.mc);
	ml.mc = 0; // for rsb__leaves_analysis_rec

	if(!ml.sa || !ml.mp)
       	{
		errval = RSB_ERR_ENOMEM;
		rsb__mtx_list_free(&ml);
	       	RSB_PERR_GOTO(err,RSB_ERRM_EM);
       	}

	*mlp = ml;
err:
	return errval;
}

#define RSB_SPLIT_IS_EXPERIMENTAL 1
#define RSB_LS_PARANOIA 0
#if ( RSB_LS_PARANOIA > 0 )
#define RSB_LS_ASSERT RSB_ASSERT
#else /* RSB_LS_PARANOIA */
#define RSB_LS_ASSERT 
#endif /* RSB_LS_PARANOIA */

#ifdef RSB_ALLOW_INTERNAL_GETENVS
#define RSB_AT_ALLOW_GETENV RSB_ALLOW_INTERNAL_GETENVS /* activate this only for testing purposes */
#else /* RSB_ALLOW_INTERNAL_GETENVS */
#define RSB_AT_ALLOW_GETENV 0 /* activate this only for testing purposes */
#endif /* RSB_ALLOW_INTERNAL_GETENVS */

static void rsb__scale_subm_idx_on_env_var(const char *envv, double * mftsp, rsb_submatrix_idx_t * mctsp, const int wv)
{
#if RSB_AT_ALLOW_GETENV
	const char * msss = NULL; /* matrix split specification string */
	if( ( msss = getenv(envv) ) != NULL )
	{
		int nom=0,den=0;
	       	char c=0;

		if( 2 == sscanf(msss,"%d/%d",&nom,&den) )
			*mftsp = ((double)(nom))/(den);
		else
		if( 1 == sscanf(msss,"0.%lf",mftsp ) )
			*mftsp /= 10.0;
		else
		if( 2 == sscanf(msss,"%lf%[%]",mftsp ,&c) )
		{
			*mftsp /= 100.0;
		}
		else
		if( 1 == sscanf(msss,"%d",mctsp) )
			;
		else
		{
			RSB_STDOUT("\"%s\" is a wrong value for %s. Use something as in e.g. 1/3 0.4 10%% 5\n",msss,envv);
		}
		if(wv>0)
			RSB_STDOUT("Will split/merge a fraction %g of the original submatrices (used env.v. %s).\n",*mftsp,envv);

	}
#endif /* RSB_AT_ALLOW_GETENV */
}

rsb_err_t rsb__mtx_split(struct rsb_mtx_t * RSB_RESTRICT mtxAp, rsb_submatrix_idx_t manp, rsb_time_t * RSB_RESTRICT stp, rsb_time_t * RSB_RESTRICT atp, rsb_time_t * RSB_RESTRICT ltp, const int wv, int kc)
{
	/* 
	 	Splits leaves of a matrix further.
	 	The matrix stays in a consistent state even on error.
		However, it may be in a different state than in the beginning.
		TODO: need to document work memory requirements.
		FIXME: need to use manp.
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t mt = - rsb_time(), st = RSB_TIME_ZERO, lt = RSB_TIME_ZERO, at = RSB_TIME_ZERO; /* merge,sort,elapsed,analysis time */
	struct rsb_mtx_list_t ml; /* matrix list */
	rsb_submatrix_idx_t smi = 0, nsm = 0;
	int sc = RSB_MTX_CMP_NNZ_DES;
	rsb_long_t smc = rsb__submatrices(mtxAp);
	rsb_submatrix_idx_t nsbs,nsas; /* number of submatrices [before/after] split */
	rsb_thread_t rnt = rsb_get_num_threads();
	rsb_thread_t nst = rnt; /* number of threads active during splitting */

	// rsb_submatrix_idx_t mmts = 0; /* max matrices to split */
	rsb_submatrix_idx_t mcts = 0; /* matrices count to split */
	double mfts = 0.5; /* matrices fraction to split */

	rsb_flags_t flags = RSB_FLAG_NOFLAGS;

	rsb__mtx_list_init(&ml);

	if(!mtxAp)
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(ret, RSB_ERRM_E_MTXAP);
	}

	nsbs = mtxAp->all_leaf_matrices_n;
	flags = mtxAp->flags & RSB_FLAG_USE_HALFWORD_INDICES;
	if(nsbs == 1)
		RSB_DO_FLAG_ADD(flags, RSB_FLAG_USE_HALFWORD_INDICES);
#if RSB_SPLIT_IS_EXPERIMENTAL
	if(! RSB_SOME_ERROR(errval) )
	if(!rsb__mtx_chk(mtxAp))
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_ERROR(RSB_ERRM_ES);
		goto ret;
	}
#endif

	if(wv>2)
		RSB_STDOUT("# experimental leaves analysis & split: "),
		RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp)),
		RSB_STDOUT("\n");
	if(wv>2)
		RSB_STDOUT("# max ptr diff is %zd units\n",rsb__submatrices_max_ptr_diff(mtxAp));
/*
	for(smi=0;smi<mtxAp->all_leaf_matrices_n;++smi)
	{
		int smj;
		for(smj=smi+1;smj<mtxAp->all_leaf_matrices_n;++smj)
		if(mtxAp->all_leaf_matrices[smi].mtxlp == mtxAp->all_leaf_matrices[smj].mtxlp)
		{
			printf("Duplicate submatrices: p:%p i:%d j:%d max:%d\n",0x0,smi,smj,mtxAp->all_leaf_matrices_n); 
			RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp->all_leaf_matrices[smj].mtxlp));
		}
	}
*/
	/* Determine largest / heaviest leaf. */
	at = -rsb_time();
	errval = rsb__mtx_list_bld(&ml, mtxAp);
	if(RSB_SOME_ERROR(errval))
	{
		RSB_PERR_GOTO(ret, RSB_ERRM_E_MTXAP);
	}
	errval = rsb__leaves_analysis_rec(mtxAp, &ml, wv, RSB_BOOL_FALSE);
	errval = rsb__srt_subm_ptr_array(ml.mp, ml.mc, sc);
	at += rsb_time();
	RSB_ASSERT(ml.mc>0);

	rsb__scale_subm_idx_on_env_var("RSB_SPLIT_SF",&mfts,&mcts,wv);
#if RSB_AT_ALLOW_GETENV
	nst = getenv("RSB_SPLIT_NT") ? rsb__util_atoi(getenv("RSB_SPLIT_NT")) : nst;
#endif /* RSB_AT_ALLOW_GETENV */
	/* TODO: can we have recursive split for corner-concentrated-nonzeroes matrices ? */

	if(mcts == 0)
		mcts = (rsb_submatrix_idx_t)(mfts*ml.mc);
	mcts = RSB_MIN(RSB_MAX(1, mcts), ml.mc); /* 1 ... ml.mc */
       	nst = RSB_MIN(mcts, nst);

	if(manp > 0)
	       	mcts = RSB_MIN(mcts, manp);

	#pragma omp parallel for schedule(static,1) reduction(|:errval) reduction(+:lt) reduction(+:st) shared(nsm) shared(ml)  num_threads(nst)
	for(smi=0;smi<mcts;++smi)
	{
		struct rsb_mtx_t * mtxMp = ml.mp[smi]; 
		rsb_coo_idx_t nrA = mtxMp->nr, ncA = mtxMp->nc;
		const rsb_coo_idx_t msz = 2;
		rsb_nnz_idx_t nzul = 0, nzur = 0, nzll = 0, nzlr = 0;
		rsb_nnz_idx_t nzu = 0, nzl = 0;
		struct rsb_coo_matrix_t coa, cot;
		rsb_coo_idx_t mr = RSB_MIDDLE(nrA), mc = RSB_MIDDLE(ncA);
		rsb_time_t lst = RSB_TIME_ZERO, llt = RSB_TIME_ZERO; /* local st,lt */
		rsb_coo_idx_t * TA = NULL;
		/* RSB_STDOUT("thread %d handles matrix %d\n",omp_get_thread_num(),smi); */

		llt = -rsb_time();

		/* Skip processing if no split possible or convenient. */
		if( nrA < msz || ncA < msz || mtxMp->nnz < 4 )
		{
			/* TODO: shall we communicate this somehow to the outside ? */
			if(wv>2)
			RSB_ERROR("Matrix is too small for splitting:"),
			RSB_ERROR(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxMp)),
			RSB_ERROR("\n");
			goto nerr;
		}

		if( nsm + 3 >= RSB_TMP_OVERALLOC_MTX * ml.mc )
		{
			RSB_PERR_GOTO(lerr,"Exceeded inner limits !");
		}

		/* Switch to COO, then split. */
		RSB_LS_ASSERT( rsb__mtx_chk(mtxMp)==RSB_BOOL_TRUE);
		#pragma omp critical (rsb__mtx_split_cr)
		{
			/* TODO: one can minimize this number further */
			TA = rsb__malloc( sizeof(rsb_coo_idx_t) * RSB_MIN(mtxMp->nnz,1+nrA) );
			if(!TA)
			{
				errval = RSB_ERR_ENOMEM;
			}
		}
		if(RSB_SOME_ERROR(errval))
		{
		       	RSB_PERR_GOTO(lerr,RSB_ERRM_PAL	); /* !! */
		}
		errval = rsb__do_switch_leaf(mtxMp, RSB_MATRIX_STORAGE_BCOR, RSB_FLAG_USE_FULLWORD_INDICES, 0, 0, TA);
		RSB_LS_ASSERT(!RSB_SOME_ERROR(errval));
		RSB_LS_ASSERT( rsb__mtx_chk(mtxMp)==RSB_BOOL_TRUE);
		/*if( rsb__util_is_nnz_array_sorted_up_partial_order(mtxMp->bpntr,mtxMp->nnz)!=RSB_BOOL_TRUE)
			rsb__mtx_chk(mtxMp); */

		RSB_LS_ASSERT( rsb__util_is_nnz_array_sorted_up_partial_order(mtxMp->bpntr,mtxMp->nnz)==RSB_BOOL_TRUE);
		if(wv>2)
			RSB_STDOUT("# switched the largest leaf to COO: "),
			RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxMp)),
			RSB_STDOUT("\n");
		rsb__project_rsb_to_coo(mtxMp,&coa);

		/* Count elements in each quadrant */
		nzu = rsb__nnz_split_coo_bsearch(coa.IA,mr,mtxMp->nnz);
		nzl = mtxMp->nnz - nzu;

		cot.typecode = mtxMp->typecode;
		cot.nnz = RSB_MAX(nzu,nzl); /* TODO: one can find better solutions ... */

		#pragma omp critical (rsb__mtx_split_cr)
		{
			if( NULL == rsb__allocate_coo_matrix_t(&cot) )
			{
				errval = RSB_ERR_ENOMEM;
			}
		}
		if(RSB_SOME_ERROR(errval))
		{
		       	RSB_PERR_GOTO(lerr,RSB_ERRM_PAL	); /* !! */
		}
		lst = -rsb_time();
		RSB_LS_ASSERT( rsb__mtx_chk(mtxMp)==RSB_BOOL_TRUE);
		if( RSB_LS_PARANOIA )
		{
			/* FIXME: DO NOT COMMIT THIS */
			rsb_coo_idx_t li, ui;
			rsb_coo_idx_t lj, uj;
			rsb__util_find_extremal_full_index_val(coa.IA,nzu+nzl,0,-1,&li,&ui);
			rsb__util_find_extremal_full_index_val(coa.JA,nzu+nzl,0,-1,&lj,&uj);
			//RSB_STDOUT("1 nr=%d nc=%d li=%d ui=%d lj=%d uj=%d mr=%d mc=%d nzu=%d nzl=%d.\n",nrA,ncA,li,ui,lj,uj,mr,mc,nzu,nzl);
			RSB_ASSERT( (ui>=mr && nzl>0) || (ui<mr && nzl==0) );
		}
		if( RSB_LS_PARANOIA )
		{
			/* FIXME: DO NOT COMMIT THIS */
			rsb_coo_idx_t li, ui;
			rsb_coo_idx_t lj, uj;
			rsb__util_find_extremal_full_index_val(coa.IA,nzu,0,-1,&li,&ui);
			rsb__util_find_extremal_full_index_val(coa.JA,nzu,0,-1,&lj,&uj);
			//RSB_STDOUT("2 nr=%d nc=%d li=%d ui=%d lj=%d uj=%d mr=%d mc=%d nzu=%d nzl=%d.\n",nrA,ncA,li,ui,lj,uj,mr,mc,nzu,nzl);
			//RSB_ASSERT( (ui>=mr && nzl>0) || (ui<mr && nzl==0) );
		}
		rsb__coo_to_lr( cot.VA, cot.IA, cot.JA, coa.VA, coa.IA, coa.JA, mc, nzu, 0, 0,   &nzul, &nzur,  0,-mc, mtxAp->typecode);
		if( RSB_LS_PARANOIA )
		{
			/* FIXME: DO NOT COMMIT THIS */
			rsb_coo_idx_t li, ui;
			rsb_coo_idx_t lj, uj;
			rsb__util_find_extremal_full_index_val(coa.IA,nzu,0,-1,&li,&ui);
			rsb__util_find_extremal_full_index_val(coa.JA,nzu,0,-1,&lj,&uj);
			//RSB_STDOUT("3 nr=%d nc=%d li=%d ui=%d lj=%d uj=%d mr=%d mc=%d nzu=%d nzl=%d.\n",nrA,ncA,li,ui,lj,uj,mr,mc,nzu,nzl);
			RSB_ASSERT( ui < mr );
		}
		if( RSB_LS_PARANOIA )
		{
			/* FIXME: DO NOT COMMIT THIS */
			rsb_coo_idx_t li, ui;
			rsb_coo_idx_t lj, uj;
			rsb__util_find_extremal_full_index_val(coa.IA,nzul,0,-1,&li,&ui);
			rsb__util_find_extremal_full_index_val(coa.JA,nzul,0,-1,&lj,&uj);
			//RSB_STDOUT("3 nr=%d nc=%d li=%d ui=%d lj=%d uj=%d mr=%d mc=%d nzu=%d nzl=%d.\n",nrA,ncA,li,ui,lj,uj,mr,mc,nzu,nzl);
			RSB_ASSERT( ui < mr );
			RSB_ASSERT( uj < mc );
		}
		rsb__coo_to_lr( cot.VA, cot.IA, cot.JA, coa.VA, coa.IA, coa.JA, mc, nzl, 0, nzu, &nzll, &nzlr,-mr,-mc, mtxAp->typecode);
		/* RSB_ASSERT( rsb__mtx_chk(mtxMp)==RSB_BOOL_TRUE); */
		lst += rsb_time();
		#pragma omp critical (rsb__mtx_split_cr)
		{
			rsb__destroy_coo_matrix_t(&cot);
		}
		RSB_ASSERT( mtxMp->nnz == nzu + nzl );
		RSB_ASSERT( mtxMp->nnz == nzul + nzur + nzll + nzlr );
		RSB_ASSERT( nzul + nzur == nzu );
		RSB_ASSERT( nzll + nzlr == nzl );

		if(wv>2)
			RSB_STDOUT("# nzu=%d nzl=%d nzul=%d nzur=%d nzll=%d nzlr=%d.\n",nzu,nzl,nzul,nzur,nzll,nzlr);
		/* make sure we have further one to four submatrices */

		if(1)
		{
			struct rsb_mtx_t * mtxQp = NULL;
			struct rsb_mtx_t *mtxQ1p = NULL,*mtxQ2p = NULL, *mtxQ3p = NULL, *mtxQ4p = NULL;
			/* rename this in fashion of rsb__do_set_in_place_submatrices_offsets */
#define RSB_MTX_INIT_LEAF(MTXAP,MTXLP,NNZ,NZOFF,NR,NC,ROFF,COFF)	\
			RSB_BZERO_P((MTXLP));				\
			rsb__set_init_flags_and_stuff(MTXLP,NULL,NULL,NR,NC,NNZ,NNZ,NNZ,(MTXAP)->typecode,(MTXAP)->flags); \
			(MTXLP)->matrix_storage = RSB_MATRIX_STORAGE_BCOR;			\
			(MTXLP)->roff =  (MTXAP)->roff + (ROFF),				\
			(MTXLP)->coff =  (MTXAP)->coff + (COFF),				\
			(MTXLP)->nzoff = (MTXAP)->nzoff + NZOFF, 			\
			(MTXLP)->bpntr = (MTXAP)->bpntr + NZOFF, 			\
			(MTXLP)->bindx = (MTXAP)->bindx + NZOFF,			\
			(MTXLP)->VA = RSB_VA_OFFSET_POINTER((MTXAP)->VA, RSB_SIZEOF((MTXAP)->typecode), (NZOFF)),	\
			RSB_DO_FLAG_ADD((MTXLP)->flags,RSB_FLAG_DEFAULT_RSB_MATRIX_FLAGS );	\
			RSB_DO_FLAG_DEL((MTXLP)->flags,RSB_FLAG_USE_HALFWORD_INDICES);	/* COO */	\
			rsb__compute_bounded_box((MTXLP));				\
			errval = rsb__do_switch_leaf((MTXLP), RSB_MATRIX_STORAGE_AUTO, flags, 0, 0, TA);	\
			RSB_DO_FLAG_ADD((MTXLP)->flags,RSB_FLAG_NON_ROOT_MATRIX);	\
			if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(lerr,RSB_ERRM_PAL	);}

			#pragma omp critical (rsb__mtx_split_cr)
			{
			       	/* this is a 'matrix reservation' mechanism */
				/* mtxQp = (mtxAp) + smc + nsm; */ /* can use this only if there are no holes */
				mtxQp = mtxAp;
#define RSB_REC_NEXT_FREE(NSM) while((!RSB_REC_IS_SUBM_FREE(mtxQp)) && ((mtxQp)-mtxAp<(smc+NSM)))++mtxQp; /* TODO: move out of here */
				RSB_REC_NEXT_FREE(nsm);
				/* RSB_STDOUT("%d starts at %d\n",omp_get_thread_num(),(mtxQp-mtxAp)); */
				if(nzul){ mtxQ1p = mtxQp; RSB_REC_MARK_SUBM_USED(mtxQp); mtxQp++; RSB_REC_NEXT_FREE(nsm); }
				if(nzur){ mtxQ2p = mtxQp; RSB_REC_MARK_SUBM_USED(mtxQp); mtxQp++; RSB_REC_NEXT_FREE(nsm); }
				if(nzll){ mtxQ3p = mtxQp; RSB_REC_MARK_SUBM_USED(mtxQp); mtxQp++; RSB_REC_NEXT_FREE(nsm); }
				if(nzlr){ mtxQ4p = mtxQp; RSB_REC_MARK_SUBM_USED(mtxQp); mtxQp++; RSB_REC_NEXT_FREE(nsm); }
				nsm += nzul ? 1 : 0;
				nsm += nzur ? 1 : 0;
				nsm += nzll ? 1 : 0;
				nsm += nzlr ? 1 : 0;
#undef RSB_REC_NEXT_FREE
			}

			if(nzul)
			{
				mtxQp=mtxQ1p;
				/* RSB_STDOUT("thread %d divides quadrant %p / subquadrant %p = %d\n",omp_get_thread_num(),mtxMp,mtxQp,(mtxQp-mtxAp)); */
				/* mtxMp->sm[0] = mtxQp++; */
				mtxMp->sm[0] = mtxQp;
				RSB_MTX_INIT_LEAF(mtxMp,mtxMp->sm[0],nzul,0        ,    mr,    mc, 0,0 );
				RSB_LS_ASSERT( rsb__mtx_chk(mtxMp->sm[0])==RSB_BOOL_TRUE);
			}
			if(nzur)
			{
				mtxQp=mtxQ2p;
				/* RSB_STDOUT("thread %d divides quadrant %p / subquadrant %p = %d\n",omp_get_thread_num(),mtxMp,mtxQp,(mtxQp-mtxAp)); */
				/* mtxMp->sm[1] = mtxQp++; */
				mtxMp->sm[1] = mtxQp;
				RSB_MTX_INIT_LEAF(mtxMp,mtxMp->sm[1],nzur,nzul     ,    mr,ncA-mc, 0,mc);
				RSB_LS_ASSERT( rsb__mtx_chk(mtxMp->sm[1])==RSB_BOOL_TRUE);
				RSB_DO_FLAG_DEL((mtxMp->sm[1])->flags,RSB_FLAG_UPPTRI|RSB_FLAG_TRIANGULAR);
			}
			if(nzll)
			{
				mtxQp=mtxQ3p;
				/* RSB_STDOUT("thread %d divides quadrant %p / subquadrant %p = %d\n",omp_get_thread_num(),mtxMp,mtxQp,(mtxQp-mtxAp)); */
				/* mtxMp->sm[2] = mtxQp++; */
				mtxMp->sm[2] = mtxQp;
				RSB_MTX_INIT_LEAF(mtxMp,mtxMp->sm[2],nzll,nzu      ,nrA-mr,    mc,mr,0 );
				RSB_LS_ASSERT( rsb__mtx_chk(mtxMp->sm[2])==RSB_BOOL_TRUE);
				RSB_DO_FLAG_DEL((mtxMp->sm[2])->flags,RSB_FLAG_UPPTRI|RSB_FLAG_TRIANGULAR);
			}
			if(nzlr)
			{
				mtxQp=mtxQ4p;
				/* RSB_STDOUT("thread %d divides quadrant %p / subquadrant %p = %d\n",omp_get_thread_num(),mtxMp,mtxQp,(mtxQp-mtxAp)); */
				/* mtxMp->sm[3] = mtxQp++; */
				mtxMp->sm[3] = mtxQp;
				RSB_MTX_INIT_LEAF(mtxMp,mtxMp->sm[3],nzlr,nzu+nzll ,nrA-mr,ncA-mc,mr,mc);
				RSB_LS_ASSERT( rsb__mtx_chk(mtxMp->sm[3])==RSB_BOOL_TRUE);
			}
#undef RSB_MTX_INIT_LEAF
			if(wv>2)
				RSB_STDOUT("# just split %ld -> %ld subms (max %ld splits allowed (max +%ld then)).\n",(long int)(smc/*+nsm*/),(long int)(mtxQp-mtxAp),(long int)mcts,4*(long int)mcts);
			/* leaf recompression, bounds have been recomputed on each leaf */
			/* marked present matrix as non terminal and assign nonzeroes to leaves */
		}
		/* RSB_DO_FLAG_DEL(mtxMp->flags,RSB_FLAG_NON_ROOT_MATRIX); */
		RSB_DO_FLAG_ADD(mtxMp->flags,RSB_FLAG_QUAD_PARTITIONING);
		/* TODO: what if nonzeroes are concentrated in a corner ? EXPERIMENTAL OVER-SUBDIVIDE !? */
		mtxMp->VA = NULL;
		mtxMp->bindx = NULL;
		mtxMp->bpntr = NULL;
		llt += rsb_time();
		lt += llt;
		st += lst;

		mtxMp->est = lst;
		mtxMp->tat = llt;
		mtxMp->sat = llt - lst; /* TODO: this is to be completed */
		#pragma omp critical (rsb__mtx_split_cr)
	       	{
			RSB_CONDITIONAL_FREE(TA);
	       	}
		continue;
lerr:
		RSB_NULL_STATEMENT_FOR_COMPILER_HAPPINESS
		RSB_ASSERT(0); /* critical */
nerr:		/* Not an error condition. */
		RSB_NULL_STATEMENT_FOR_COMPILER_HAPPINESS
		/* FIXME: Good quality code would provide here a mechanism for reverting the matrix to the original state. */
	} /* smi */

	if(nsm > 0)
		RSB_DO_FLAG_ADD(mtxAp->flags, RSB_FLAG_USE_HALFWORD_INDICES);

	errval = rsb__refresh_array_of_leaf_matrices(mtxAp);

	if(RSB_SOME_ERROR(errval))
	{
	       	RSB_ERROR(RSB_ERRM_EM);
	       	/*
		 * TODO: complete this part !
		 * One would need a matrix revert/recovery mechanism!
		 * */
	}	
	mt += rsb_time();
	nsas = mtxAp->all_leaf_matrices_n;
#if RSB_STORE_IDXSA
	mtxAp->idxsa = rsb__get_index_storage_amount(mtxAp);
#endif
	if(wv>0)
		RSB_STDOUT("Split (%d -> %d leaves, %d -> %d subms) took %0.4lg s.\n",nsbs,nsas,(int)smc,(int)smc+nsm,mt);

/* The following shall be used to locate bugs in the split loop locking mechanism. */
/*
	for(smi=0;smi<mtxAp->all_leaf_matrices_n;++smi)
	{
		printf("%p : ",mtxAp->all_leaf_matrices[smi].mtxlp);
		RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp->all_leaf_matrices[smi].mtxlp));
		printf("\n");
	}
	for(smi=0;smi<mtxAp->all_leaf_matrices_n;++smi)
	{
		int smj;
		for(smj=smi+1;smj<mtxAp->all_leaf_matrices_n;++smj)
		if(mtxAp->all_leaf_matrices[smi].mtxlp == mtxAp->all_leaf_matrices[smj].mtxlp)
		{
			printf("Oops. Duplicate submatrices: p: %p  i:%d j:%d max:%d\n",mtxAp->all_leaf_matrices[smj].mtxlp,smi,smj,mtxAp->all_leaf_matrices_n); 
			RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp));
			printf("\n");
			RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp->all_leaf_matrices[smj].mtxlp));
			printf("\n");
			printf("This is BAD. Terminating\n");
			RSB_DEBUG_ASSERT(0);
			exit(-1);
		}
	}
*/

#if RSB_SPLIT_IS_EXPERIMENTAL
	if(! RSB_SOME_ERROR(errval) )
	if(!rsb__mtx_chk(mtxAp))
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_ERROR(RSB_ERRM_ES);
		goto ret;
	}
#endif
ret:
	rsb__mtx_list_free(&ml);
	RSB_ASSIGN_IF(stp, st)
	RSB_ASSIGN_IF(atp, at)
	RSB_ASSIGN_IF(ltp, lt)
	return errval;
}

rsb_err_t rsb__leaves_merge(struct rsb_mtx_t * RSB_RESTRICT mtxAp, rsb_submatrix_idx_t manp, rsb_time_t * RSB_RESTRICT stp, rsb_time_t *RSB_RESTRICT atp, rsb_time_t *RSB_RESTRICT ltp, const int wv, int kc)
{
	/**
		\ingroup gr_internals
		Merges leaf level sparse blocks, one level.
		It preserves the original VA,IA,JA arrays.

	 	TODO: rename to rsb__mtx_merge
	 	TODO: need to document memory requirements.
		TODO: at the moment, errors are considered to be critical (matrix destructive).
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t mt = - rsb_time(), 
		   st = RSB_TIME_ZERO, 
		   lt = RSB_TIME_ZERO, 
		   at = RSB_TIME_ZERO; /* merge,sort,elapsed,analysis time */
	/*const int wv = 0;*/ /* want verbose */
	rsb_submatrix_idx_t nsbp, nsap; /* number of submatrices before and after merge  */
	struct rsb_mtx_list_t ml; /* matrix list */
	const int vl = 2; /* ?? */
	/* int kc = 0; */ /* keep coo */

	rsb__mtx_list_init(&ml);

	if(!mtxAp)
	{
		RSB_PERR_GOTO(err, RSB_ERRM_E_MTXAP);
		goto ret;
	}

	nsbp = mtxAp->all_leaf_matrices_n;
	if(wv>vl)
		RSB_STDOUT("# experimental leaves analysis: "),
		RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp)),
		RSB_STDOUT("\n");

	if(! rsb__is_terminal_recursive_matrix(mtxAp))
	{
		rsb_submatrix_idx_t smi;
		const size_t rsbio = rsb__get_index_storage_amount(mtxAp);
		int rnt = rsb_get_num_threads();
		int nmt = rnt;
		int sc = RSB_MTX_CMP_NNZ_ASC;
		rsb_submatrix_idx_t mctm = 0; /* matrices count to merge */
		double mftm = 0.5; /* matrices fraction to merge */

#if RSB_AT_ALLOW_GETENV
		nmt = getenv("RSB_MERGE_NT") ? rsb__util_atoi(getenv("RSB_MERGE_NT")) : nmt;
		sc = getenv("RSB_MERGE_SC")  ? rsb__util_atoi(getenv("RSB_MERGE_SC")) : sc;
#endif /* RSB_AT_ALLOW_GETENV */

		RSB_DEBUG_ASSERT(nsbp>0);

		at = -rsb_time();
		errval = rsb__mtx_list_bld(&ml, mtxAp);
		if(RSB_SOME_ERROR(errval))
		{
			RSB_PERR_GOTO(ret, RSB_ERRM_EM);
		}
		errval = rsb__leaves_analysis_rec(mtxAp, &ml, wv, RSB_BOOL_TRUE);
		if(RSB_SOME_ERROR(errval))
		{
			RSB_PERR_GOTO(err, RSB_ERRM_EM);
		}
		errval = rsb__srt_subm_ptr_array(ml.mp, ml.mc, sc);
		if(RSB_SOME_ERROR(errval))
		{
			RSB_PERR_GOTO(err, RSB_ERRM_EM);
		}
		at += rsb_time();

		if(manp > 0)
		       	ml.mc = RSB_MIN( ml.mc, manp);

		rsb__scale_subm_idx_on_env_var("RSB_MERGE_SF",&mftm,&mctm,wv);
		if(mctm == 0)
			mctm = (rsb_submatrix_idx_t)(mftm*ml.mc);
		mctm = RSB_MIN(RSB_MAX(1, mctm), ml.mc); /* 1 ... ml.mc */
       		nmt = RSB_MIN(mctm, nmt);

		if(wv>vl)
			RSB_STDOUT("Basic storage uses %zu bytes (%2.3lf bpnz).\n", rsbio, ((double)rsbio)/mtxAp->nnz),
			RSB_STDOUT("We have %d merge candidate pre-leaves of which %d will be processed with %d threads.\n",ml.mc,mctm,nmt);

		/* In the following parallel loop, omitting the RSB_NTC thread specification. */
		#pragma omp parallel for schedule(static,1) reduction(|:errval) reduction(+:lt) reduction(+:st) shared(ml)  num_threads(nmt)
		for(smi=0;smi</*ml.mc*/mctm;++smi)
		{
			struct rsb_mtx_t * submatrix = NULL;
			rsb_submatrix_idx_t sml;
			rsb_submatrix_idx_t i,j;
			struct rsb_mtx_t * mtxMp = ml.mp[smi];
			size_t rsbio = rsb__get_index_storage_amount(mtxAp); /* rsb indices occupation */
			size_t subio = rsb__get_index_storage_amount(mtxMp); /* submatrices indices occupation */
			size_t jmios = ml.sa[smi]; /* join matrix index occupation save */
			double rsavepcnt = 100.0*(((double)(jmios))/(double)subio);
			double asavepcnt = 100.0*(((double)(jmios))/(double)rsbio);
			rsb_coo_idx_t roffM = mtxMp->roff, coffM = mtxMp->coff;
			rsb_coo_idx_t broffM = mtxMp->roff+mtxMp->nr, bcoffM = mtxMp->coff+mtxMp->nc;
			rsb_nnz_idx_t nzul = RSB_NNZ_OF(mtxMp->sm[0]), nzur = RSB_NNZ_OF(mtxMp->sm[1]);
			rsb_nnz_idx_t nzll = RSB_NNZ_OF(mtxMp->sm[2]), nzlr = RSB_NNZ_OF(mtxMp->sm[3]);
			rsb_bool_t ifq = RSB_BOOL_FALSE; /* is first quadrant ? */
			rsb_time_t lst = RSB_TIME_ZERO, llt = RSB_TIME_ZERO; /* local st,lt */
			rsb_coo_idx_t * TA = NULL;
			struct rsb_coo_matrix_t tcoo;

			RSB_BZERO_P(&tcoo);

#if RSB_AT_ALLOW_GETENV
			if(getenv("RSB_MERGE_KEEP_COO"))
				kc = rsb__util_atoi(getenv("RSB_MERGE_KEEP_COO"));
#endif /* RSB_AT_ALLOW_GETENV */

			if(wv>vl)
				RSB_STDOUT(RSB_PRINTF_MTX_SUMMARY_ARGS(mtxMp)),
				RSB_STDOUT("\n");

			llt = -rsb_time();
			if(wv>vl)
				RSB_STDOUT("By merging %p [%d+%d+%d+%d=%d], gain relative %3.2lg%% or absolute %3.2lg%% (%zu bytes)\n", (const void*)mtxMp, nzul, nzur, nzll, nzlr, mtxMp->nnz, rsavepcnt, asavepcnt, jmios);
			RSB_ASSERT( nzul + nzur + nzll + nzlr == mtxMp->nnz );

			mtxMp->bpntr = NULL;
			mtxMp->bindx = NULL;
			mtxMp->VA = NULL;

			#pragma omp critical (rsb__mtx_split_cr)
			{
				/* TODO: one can minimize this number further */
				rsb_nnz_idx_t tamnz = RSB_MIN(mtxMp->nnz,1+mtxMp->nr);
#if RSB_MERGE_USE_TMP_COOMTX
				tcoo.nnz = RSB_MAX( RSB_MAX(nzul+nzur,nzll+nzlr), tamnz );
				tcoo.typecode = mtxMp->typecode;
				if( NULL == rsb__allocate_coo_matrix_t(&tcoo) )
					errval = RSB_ERR_ENOMEM;
				TA = tcoo.IA;
#else /* RSB_MERGE_USE_TMP_COOMTX */
				TA = rsb__malloc( sizeof(rsb_coo_idx_t) * tamnz );
				if(!TA)
					errval = RSB_ERR_ENOMEM;
#endif /* RSB_MERGE_USE_TMP_COOMTX */
			}

			RSB_SUBMATRIX_FOREACH(mtxMp, submatrix, i, j)
			if(submatrix)
			{
				ifq = RSB_BOOL_FALSE;
				if(wv>3)
					RSB_STDOUT("lmax in IA/JA: %d/%d.\n", rsb__util_find_max_index_val(submatrix->bpntr, submatrix->nnz), rsb__util_find_max_index_val(submatrix->bindx, submatrix->nnz));
				errval = rsb__do_switch_leaf(submatrix, RSB_MATRIX_STORAGE_BCOR, RSB_FLAG_USE_FULLWORD_INDICES, submatrix->roff-roffM, submatrix->coff-coffM, TA);
				if(wv>3)
					RSB_STDOUT("smax in IA/JA: %d/%d.\n", rsb__util_find_max_index_val(submatrix->bpntr, submatrix->nnz), rsb__util_find_max_index_val(submatrix->bindx, submatrix->nnz));

				if(RSB_SOME_ERROR(errval))
				{
				       	/* RSB_PERR_GOTO(done,RSB_ERRM_ES); */
					RSB_ASSERT(!(RSB_SOME_ERROR(errval)));
				}
				if( mtxMp->bpntr == NULL && mtxMp->bindx == NULL )
				{
					ifq = RSB_BOOL_TRUE;
					mtxMp->bpntr = submatrix->bpntr;
					mtxMp->bindx = submatrix->bindx;
					mtxMp->VA = submatrix->VA;
					mtxMp->flags = submatrix->flags;
					mtxMp->matrix_storage = submatrix->matrix_storage;
				}
				mtxMp->roff = RSB_MIN(mtxMp->roff, submatrix->roff);
				mtxMp->coff = RSB_MIN(mtxMp->coff, submatrix->coff);
				bcoffM = RSB_MIN(bcoffM, submatrix->bcoff);
				broffM = RSB_MIN(broffM, submatrix->broff);
				/* broff, bcoff remain the same (local indices) */
				/* base bm, bk */
				mtxMp->bm   = mtxMp->nr;
				mtxMp->bk   = mtxMp->nc;
				/* tighten bm, bk (local indices) */
				mtxMp->bm   = RSB_MAX(mtxMp->bm  ,roffM+submatrix->bm-submatrix->roff  );
				mtxMp->bk   = RSB_MAX(mtxMp->bk  ,coffM+submatrix->bk-submatrix->coff  );
				/* br, bc remain the same */
				/*RSB_STDOUT("br/bc : %d/%d: %d/%d.\n", mtxMp->br,mtxMp->bc,submatrix->br,submatrix->bc); */

				for(sml=0;sml<mtxAp->all_leaf_matrices_n;sml++)
				if(mtxAp->all_leaf_matrices[sml].mtxlp == submatrix)
				{
					/* In order to get rid of this loop one shall reorder the submatrices appropriately */
					if(ifq == RSB_BOOL_TRUE)
					{
						mtxAp->all_leaf_matrices[sml].mtxlp = NULL;
						if(wv>3)
							RSB_STDOUT("Nullified leaf %d [%d,%d] and substituted with merged (%d).\n",sml,i,j,smi);
					}
					else
					{
						mtxAp->all_leaf_matrices[sml].mtxlp = NULL;
						if(wv>3)
							RSB_STDOUT("Nullified leaf %d [%d,%d].\n",sml,i,j);
					}
				}
			} /* submatrix */
			RSB_SUBMATRIX_FOREACH(mtxMp, submatrix, i, j)
			if(submatrix)
			{
				RSB_BZERO_P(submatrix);
			}
			RSB_DO_FLAG_DEL(mtxMp->flags, RSB_FLAG_WANT_BCSS_STORAGE);
			RSB_DO_FLAG_DEL(mtxMp->flags, RSB_FLAG_USE_HALFWORD_INDICES);
			RSB_DO_FLAG_ADD(mtxMp->flags, RSB_FLAG_WANT_COO_STORAGE);
			mtxMp->matrix_storage = RSB_MATRIX_STORAGE_BCOR;
			mtxMp->broff = broffM;
			mtxMp->bcoff = bcoffM;
			lst = -rsb_time();
			{
				int mo = 0, wp = 0;
#if RSB_AT_ALLOW_GETENV
				if(getenv("RSB_MERGE_USRT"))
					mo = rsb__util_atoi(getenv("RSB_MERGE_USRT"));
#endif /* RSB_AT_ALLOW_GETENV */
				/* Note: we are inside a parallel outer loop: therefore here the sort algorithm can only be serial */
				if(mo == 0)
				{
					errval += rsb__cor_merge(mtxMp->typecode, mtxMp->VA, mtxMp->bpntr, mtxMp->bindx, 0        , nzul, nzur, 1, wp, &tcoo);
					RSB_ASSERT(!(RSB_SOME_ERROR(errval)));
					errval += rsb__cor_merge(mtxMp->typecode, mtxMp->VA, mtxMp->bpntr, mtxMp->bindx, nzul+nzur, nzll, nzlr, 1, wp, &tcoo);
					RSB_ASSERT(!(RSB_SOME_ERROR(errval)));
				}

				if(mo == 1)
				{
					struct rsb_mtx_t * mtxCp = mtxMp->sm[2];

					// RSB_STDOUT("Merging of %p %p %p %p\n",mtxMp->sm[0],mtxMp->sm[1],mtxMp->sm[2],mtxMp->sm[3]);
					/* in the below invocations, I've to use nr of M, not of the submatrices */
					if( mtxMp->sm[0] && mtxMp->sm[1] )
					{
						rsb_nnz_idx_t nnzB = nzul + nzur;
						rsb_coo_idx_t*IB = mtxMp->bpntr, *JB = mtxMp->bindx;
						void * VB = mtxMp->VA;
						errval = rsb_util_sort_row_major_inner(VB, IB, JB, nnzB,mtxMp->nr,mtxMp->nc,mtxMp->typecode,mtxMp->flags);
						RSB_ASSERT(!(RSB_SOME_ERROR(errval)));
					}

					if( mtxMp->sm[2] && mtxMp->sm[3] )
					{
						rsb_nnz_idx_t nnzC = nzll + nzlr;
						rsb_coo_idx_t*IC = mtxMp->bpntr + mtxCp->nzoff - mtxMp->nzoff,*JC = mtxMp->bindx + mtxCp->nzoff - mtxMp->nzoff;
						void * VC = RSB_TYPED_OFF_PTR(mtxMp->typecode, mtxMp->VA, (mtxCp->nzoff-mtxMp->nzoff));
						errval = rsb_util_sort_row_major_inner(VC, IC, JC, nnzC, mtxMp->nr, mtxMp->nc, mtxMp->typecode, mtxMp->flags);
						RSB_ASSERT(!(RSB_SOME_ERROR(errval)));
					}
				}

				if(mo >= 2)
				{
					errval = rsb_util_sort_row_major_inner(mtxMp->VA, mtxMp->bpntr, mtxMp->bindx, mtxMp->nnz, mtxMp->nr, mtxMp->nc, mtxMp->typecode, mtxMp->flags);
				}
				RSB_ASSERT(rsb__util_is_coo_array_sorted_up_partial_order(mtxMp->bpntr, mtxMp->nnz));
				RSB_ASSERT(nzul+nzur+nzll+nzlr == mtxMp->nnz);
			}
			lst += rsb_time();
			st += lst;
			if(RSB_SOME_ERROR(errval))
		       	{
			       	RSB_PERR_GOTO(lerr, RSB_ERRM_ES);
		       	}

			RSB_REC_MARK_SUBM_FREE(mtxMp->sm[0]);
			RSB_REC_MARK_SUBM_FREE(mtxMp->sm[1]);
			RSB_REC_MARK_SUBM_FREE(mtxMp->sm[2]);
			RSB_REC_MARK_SUBM_FREE(mtxMp->sm[3]);

			if(!kc)
			{
				/* FIXME: shall implement a format selection policy right here */
				rsb_fmt_t dms = RSB_MATRIX_STORAGE_BCOR;
				rsb_flags_t flagsM = RSB_FLAG_USE_FULLWORD_INDICES;
				if(RSB_INDICES_FIT_IN_HALFWORD(mtxMp->nr, mtxMp->nc))
					flagsM = RSB_FLAG_USE_HALFWORD_INDICES;
				if( mtxMp->nr+1 <= mtxMp->nnz )
					dms = RSB_MATRIX_STORAGE_BCSR;
				errval = rsb__do_switch_leaf(mtxMp, dms, flagsM, 0, 0, TA);
				RSB_ASSERT(!(RSB_SOME_ERROR(errval)));
				/* TODO: shall harmonize with rsb_do_switch_fresh_recursive_matrix_to_halfword_storages_parallel */
			}

			if(RSB_SOME_ERROR(errval))
			{ 
				/* TODO:error reporting is missing */
				RSB_PERR_GOTO(lerr, RSB_ERRM_ES);
		       	}
			if(wv>3)
				RSB_STDOUT("tmax in IA/JA: %d/%d.\n", rsb__util_find_max_index_val(mtxMp->bpntr, mtxMp->nnz), rsb__util_find_max_index_val(mtxMp->bindx, mtxMp->nnz) );

			llt += rsb_time();
			lt += llt;

			mtxMp->est = lst;
			mtxMp->tat = llt;
			mtxMp->sat = llt - lst; /* TODO: this is to be completed */

			#pragma omp critical (rsb__mtx_split_cr)
		       	{
#if RSB_MERGE_USE_TMP_COOMTX
				rsb__destroy_coo_matrix_t(&tcoo);
#else
				RSB_CONDITIONAL_FREE(TA);
#endif /* RSB_MERGE_USE_TMP_COOMTX */
		       	}
			continue;
lerr:
			RSB_NULL_STATEMENT_FOR_COMPILER_HAPPINESS
			// goto done;
		} /* smi */

		if(wv>vl)
			/* RSB_STDOUT("Now: %2.3lf bpnz\n", ((double)rsb__get_index_storage_amount(mtxAp))/mtxAp->nnz), */
			RSB_STDOUT("Now: "),
			RSB_STDOUT(RSB_PRINTF_MATRIX_AT_SUMMARY_ARGS(mtxAp)),
			RSB_STDOUT("\n");

		errval = rsb__refresh_array_of_leaf_matrices(mtxAp);
		if(RSB_SOME_ERROR(errval))
	       	{
			RSB_ERROR("Need code to recover the old (pre-merge) matrix here !\n"); /* FIXME */
			/* Essentially, reorder the coefficients and restore the old order */
		       	RSB_PERR_GOTO(cer,RSB_ERRM_ES);
		}
		RSB_DO_FLAG_DEL(mtxAp->flags, RSB_FLAG_NON_ROOT_MATRIX);
#if RSB_STORE_IDXSA
		mtxAp->idxsa = rsb__get_index_storage_amount(mtxAp);
#endif
	}
	mt += rsb_time(); 
	nsap = mtxAp->all_leaf_matrices_n;
	RSB_ASSIGN_IF(stp, st)
	RSB_ASSIGN_IF(atp, at)
	RSB_ASSIGN_IF(ltp, lt)
	if(wv>0)
		RSB_STDOUT("Merge (%d -> %d leaves) took w.c.t. of %0.4lgs, ~%0.4lgs of computing time (of which %0.4lgs sorting, %0.4lgs analysis)\n", nsbp, nsap, mt, lt, st, at);
	goto err;
cer:
	RSB_ERROR("Critical Merge Error: cannot proceed. Merged matrix in inconsistent state !\n");
err:
	rsb__mtx_list_free(&ml);
ret:
	return errval;
}

static rsb_err_t rsb__mtx_adjust_subm_ptrs(struct rsb_mtx_t *RSB_RESTRICT  mtxCp, const struct rsb_mtx_t *RSB_RESTRICT  mtxAp, rsb_long_t smc)
{
	/* 
	 * Adjusts pointers displacements in the matrix tree and in the pointers list.
	 */

	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_submatrix_idx_t smi;
	rsb_submatrix_idx_t n,si;

	for(n=0;n<smc;++n)
	{
		if(mtxCp[n].nnz) /* If valid. FIXME: IF NOT (E.G. MERGED), SHALL BE COMPLETELY ZEROED. */
		for(si=0;si<RSB_FOUR;++si)
			if(mtxCp[n].sm[si])
			{
				RSB_PTR_SHIFT( mtxCp[n].sm[si], mtxAp, mtxCp, (struct rsb_mtx_t*) );
			/*	RSB_STDOUT("%03d/%03d: %p\n",n,si,mtxCp[n].sm[si]); */
			}
	} /* n */

	for(	smi=0; smi < mtxCp->all_leaf_matrices_n; ++smi )
	{
		RSB_PTR_SHIFT( mtxCp->all_leaf_matrices[smi].mtxlp, mtxAp, mtxCp, (struct rsb_mtx_t*)  );
	}

	return errval;
}

rsb_err_t rsb__mtx_realloc_with_spare_leaves(struct rsb_mtx_t **mtxApp, rsb_submatrix_idx_t slc)
{
	/*
	 * Will return RSB_ERR_ENOMEM on failure, in which the matrix will stay unchanged.
	 * TODO: to get rid of this, need guaranteed preallocation in rsb__allocate_recursive_sparse_matrix_from_row_major_coo
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	struct rsb_mtx_t *mtxAp = NULL;
	rsb_submatrix_idx_t smc = 1 + rsb__submatrices_max_ptr_diff(*mtxApp);

	if( slc <= 0 )
	{
		errval = RSB_ERR_BADARGS;
		goto ret;
	}

	RSB_DEBUG_ASSERT( mtxApp);
	RSB_DEBUG_ASSERT(*mtxApp);

	mtxAp = rsb__realloc(*mtxApp,sizeof(*mtxAp)*(smc+slc));

	if(mtxAp == NULL)
	{
		errval = RSB_ERR_ENOMEM;
		goto ret;
	}

	if( mtxAp == *mtxApp )
		goto ret;

	if(0)
		RSB_STDOUT("in (%d -> %d) realloc, pointers of %d matrices have to be readjusted: %p -> %p  (%+d bytes)\n",smc,slc,smc,*mtxApp,mtxAp,(int)((rsb_byte_t*)mtxAp-(rsb_byte_t*)*mtxApp));
	errval = rsb__mtx_adjust_subm_ptrs( mtxAp, *mtxApp, smc );
ret:
	*mtxApp = mtxAp;
	return errval;
}

rsb_submatrix_idx_t rsb__get_diagonal_submatrices_count(const struct rsb_mtx_t *mtxAp)
{
	/**
	  \ingroup gr_internals
	  \return the count of submatrices which are on the diagonal, if the matrix is recursive. zero, otherwise.
	  */
	rsb_submatrix_idx_t i,j;
	struct rsb_mtx_t * submatrix = NULL;
	rsb_submatrix_idx_t dsc = 0;

	if(!mtxAp)
		goto done;

	if(rsb__is_terminal_recursive_matrix(mtxAp))
	{
		dsc = 1;	
	}
	else
	{
		RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
		if( submatrix && i==j && RSB_SUBMATRIX_IS_ON_DIAG(submatrix) )
			dsc += rsb__get_diagonal_submatrices_count(submatrix);
	}
done:
	return dsc;
}

rsb_err_t rsb__init_set_quad_submatrices_info(const struct rsb_mtx_partitioning_info_t * pinfop, struct rsb_mtx_t ** matrices, rsb_nnz_idx_t uuk, rsb_nnz_idx_t mk, rsb_nnz_idx_t uk, rsb_nnz_idx_t lk, rsb_nnz_idx_t llk, rsb_coo_idx_t mB, rsb_coo_idx_t kB, rsb_coo_idx_t roff, rsb_coo_idx_t coff)
{
	/**
	 *	\ingroup gr_internals
	 *
	 * */
	rsb_submatrix_idx_t i,j,ij=0;
	rsb_nnz_idx_t nzoff[RSB_FOUR+1]={uuk,uk,mk,lk,llk};
	rsb_nnz_idx_t hnnz = 0;
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	for(i=0;i<2;++i)
	for(j=0;j<2;++j)
		if((hnnz=nzoff[i*2+j+1]-nzoff[i*2+j])>0)
		{
			RSB_DEBUG_ASSERT(hnnz>0);

			matrices[ij]->roff=i*mB+roff;
			matrices[ij]->coff=j*kB+coff;
			matrices[ij]->nr=i?pinfop->nr-mB:mB;
			matrices[ij]->nc=j?pinfop->nc-kB:kB;

			RSB_DEBUG_ASSERT(matrices[i*2+j]->nr>0);
			RSB_DEBUG_ASSERT(matrices[i*2+j]->nc>0);

			matrices[ij]->M_b=0;
			matrices[ij]->K_b=0;

			matrices[ij]->br=pinfop->br;
			matrices[ij]->bc=pinfop->bc;

			matrices[ij]->nnz=nzoff[i*2+j+1]-nzoff[i*2+j];
			matrices[ij]->block_count=nzoff[i*2+j]; /* TODO:this is a hack. we will use block_count as first nnz index, in this function */

			//RSB_INFO("+\n");
			++ij;
		}
		else
			;//RSB_INFO("-\n");

	RSB_DO_ERR_RETURN(errval)
}

/* @endcond */
