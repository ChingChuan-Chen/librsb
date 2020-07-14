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
 * @brief Code for matrix format conversion. 
 * @author Michele Martone
 * */
#include "rsb_common.h"

rsb_err_t rsb__do_switch_recursive_in_place_matrix_to_in_place_csr(struct rsb_mtx_t * mtxAp, struct rsb_coo_matrix_t * coop)
{
	/**
		\ingroup gr_internals
		TODO: move somewhere else
		FIXME: UNTESTED,TEMPORARY, makes sense only for in place allocated
		this conversion gives you sorted coordinates.
		on exit, the pointer matrix is deallocated
		FIXME: error behaviour is undefined
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	//struct rsb_coo_matrix_t coo;
	//struct rsb_mtx_t *fsm=NULL;

	if(RSB_UNLIKELY(!mtxAp))
	{
		RSB_ERROR(RSB_ERRM_ES);
		return RSB_ERR_BADARGS;
	}
	if(RSB_DO_TOOFEWNNZFORCSR(mtxAp->nnz,mtxAp->nr))
	{
		errval = RSB_ERR_BADARGS;
		goto err;
	}
#if 1
	errval = rsb__do_switch_recursive_in_place_matrix_to_in_place_coo_sorted(mtxAp,coop);
	if(RSB_SOME_ERROR(errval))
	{
		RSB_ERROR(RSB_ERRM_ES);
		goto err;
	}
	errval = rsb__do_switch_fullword_array_to_compressed(coop->IA,coop->nnz,coop->nr);
	if(RSB_SOME_ERROR(errval))
	{
		RSB_ERROR(RSB_ERRM_ES);
		goto err;
	}
#else
#endif
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__do_switch_recursive_in_place_matrix_to_in_place_csc(struct rsb_mtx_t * mtxAp, struct rsb_coo_matrix_t * coop)
{
	/**
		\ingroup gr_internals
		TODO: move somewhere else
		FIXME: UNTESTED,TEMPORARY, makes sense only for in place allocated
		this conversion gives you sorted coordinates.
		on exit, the pointer matrix is deallocated
		FIXME: error behaviour is undefined
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	struct rsb_coo_matrix_t coo;
	//struct rsb_mtx_t *fsm=NULL;

	if(RSB_UNLIKELY(!mtxAp))
	{
		RSB_ERROR(RSB_ERRM_ES);
		return RSB_ERR_BADARGS;
	}
	if(RSB_DO_TOOFEWNNZFORCSR(mtxAp->nnz,mtxAp->nc))
	{
		errval = RSB_ERR_BADARGS;
		goto err;
	}
	RSB_INIT_CXX_FROM_MTX(&coo,mtxAp);
	coo.nr=coo.nc=0;/* FIXME: why ? */
	if(rsb__allocate_coo_matrix_t(&coo)!=&coo)
		goto err;
	rsb__util_coo_array_set(coo.IA,coo.nnz,0);
	errval = rsb__do_get_csc(mtxAp,(rsb_byte_t**)(&coo.VA),&coo.JA,&coo.IA);
	coo.nr=mtxAp->nr;
	coo.nc=mtxAp->nc;
	if(RSB_SOME_ERROR(errval))
	{
		RSB_ERROR(RSB_ERRM_ES);
		goto err;
	}
	rsb__do_mtx_free(mtxAp);
	coop->typecode=mtxAp->typecode;
	coop->nnz=coo.nnz;
	coop->VA=coo.VA;
	coop->IA=coo.IA;
	coop->JA=coo.JA;
	coop->nr=coo.nr;
	coop->nc=coo.nc;
err:
	RSB_DO_ERR_RETURN(errval)
}

/* @endcond */
