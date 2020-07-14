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
 * @brief
 * This source file contains functions for CSR handling.
 * */
#include "rsb_internals.h"

RSB_INTERNALS_COMMON_HEAD_DECLS

static rsb_err_t rsb_is_correctly_built_csr_matrix(const rsb_nnz_idx_t * PA, const rsb_coo_idx_t * JA, const rsb_coo_idx_t nrA, const rsb_coo_idx_t ncA, const rsb_nnz_idx_t nnz, const rsb_coo_idx_t ib /* index base */)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_nnz_idx_t ni;
	rsb_coo_idx_t ri;

	if(!PA ||!JA || RSB_INVALID_COO_INDEX(nrA)|| RSB_INVALID_COO_INDEX(ncA)|| RSB_INVALID_NNZ_INDEX(nnz))
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,"PA:%p JA:%p nrA:%d ncA:%d nnzA:%d\n",PA,JA,nrA,ncA,nnz);
	}

	if(PA[nrA]-ib!=nnz)
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,"PA[nrA]=%d vs nnzA=%d (ib=%d)\n",PA[nrA],nnz,ib);
	}

	for(ri=0;ri<nrA;++ri)
	{
#if 0
		if(!rsb__util_is_coo_array_sorted_up(JA+IP[nr],IP[nr+1]-IP[nr]))
		{
			RSB_PERR_GOTO(err,"bindx seems unsorted!\n");
		}
#endif
		if(PA[ri]>PA[ri+1])
		{
			errval = RSB_ERR_BADARGS;
			RSB_PERR_GOTO(err,"PA[%d]>PA[%d]: %d>%d (row off its bounds)\n",ri,ri+1,PA[ri],PA[ri+1]);
		}
		if(PA[ri+1]-PA[ri] > ncA)
		{
			errval = RSB_ERR_BADARGS;
			RSB_PERR_GOTO(err,RSB_ERRM_ES);
		}
		for(ni=PA[ri]-ib;ni<PA[ri+1]-ib;++ni)
		{
			if(ni+1<PA[ri+1]-ib)
			if(JA[ni]>=JA[ni+1])
		       	{
				errval = RSB_ERR_BADARGS;
				RSB_PERR_GOTO(err,"i=%d JA[%d]>=JA[%d]: %d>=%d (adjacent duplicates)\n",ri,ni,ni+1,JA[ni],JA[ni+1]);
			}
			if(JA[ni-ib]-ib>=ncA)
		       	{
				errval = RSB_ERR_BADARGS;
				RSB_PERR_GOTO(err,"i=%d  JA[%d]>=ncA: %d >= %d (column exceeding matrix)\n",ri,ni,JA[ni],ncA);
			}
		}
	}
err:
        RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__csr_chk(const rsb_nnz_idx_t * RSB_RESTRICT IP, const rsb_coo_idx_t * RSB_RESTRICT JA, rsb_coo_idx_t nrA, rsb_coo_idx_t ncA, rsb_nnz_idx_t nnzA, rsb_coo_idx_t ib)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	errval = rsb_is_correctly_built_csr_matrix(IP, JA, nrA, ncA, nnzA, ib);
	return errval;
}

rsb_err_t rsb__csc_chk(const rsb_nnz_idx_t * RSB_RESTRICT IP, const rsb_coo_idx_t * RSB_RESTRICT IA, rsb_coo_idx_t nrA, rsb_coo_idx_t ncA, rsb_nnz_idx_t nnzA, rsb_coo_idx_t ib)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	errval = rsb__csr_chk(IP,IA,nrA,ncA,nnzA,ib);
	return errval;
}

/* @endcond */
