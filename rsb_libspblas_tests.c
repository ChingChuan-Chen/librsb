/*                                                                                                                            

Copyright (C) 2008-2016 Michele Martone

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
 * @author Michele Martone
 * @brief  Sparse BLAS interface testing code
 * */
#include "rsb_common.h"
#include "blas_sparse/blas_enum.h"
#include "rsb_libspblas.h"
/* #include "rsb_libspblas_handle.h" */
#include "rsb_psblas.h" /* (in rsb) header for rsb_do_psblas_trans_to_rsb_trans */
#include <stdio.h>	/* fileno */
#include <unistd.h>	/* isatty */
#include "rsb_libspblas_tests.h"
#define RSB_WANT_SPGEMM_TESTING_FOR_ONLY_FIRST_DIMI 3
#define RSB_WANT_VERBOSE_FAILURES 1
#define RSB_TESTER_ALLOW_TIMEOUT 1
#define RSB_BLAS_INVALID_MATRIX (-1)
#define RSB_INVALID_BLAS_INT_IDX_VAL -1
#define RSB_WANT_AUTOTUNING_TESTING 1

RSB_INTERNALS_COMMON_HEAD_DECLS
RSB_INTERNALS_RSBENCH_HEAD_DECLS

/* TODO: shall use the following throughout the tester routine */
#define RSB_LSTERR(MSG) {RSB_ERROR(MSG);goto err;} 
#define RSB_LSTPROBE(EXP,MSG) if( RSB_SOME_ERROR(errval=(EXP))){RSB_ERROR(MSG);goto err;} /* error is not expected here */
#define RSB_LSTPROBI(EXP,MSG) if(!RSB_SOME_ERROR(errval=(EXP))){errval=RSB_ERR_NO_ERROR;RSB_ERROR(MSG);goto err;}else{errval = RSB_ERR_INTERNAL_ERROR;} /* error is expected here but ignore: better use it after a non-zealot-error-reporting function, but rather, an internal one */
#define RSB_EMPTY_STRING ""

#define RSB_BLAS_MT_STR(MT) ((MT)==blas_lower_triangular?"LT":	\
			((MT)==blas_upper_triangular?"UT":	\
			((MT)==blas_lower_symmetric?"LS":	\
			((MT)==blas_upper_symmetric?"US":	\
			((MT)==blas_lower_hermitian?"LH":	\
			((MT)==blas_upper_hermitian?"UH":	\
			((MT)==blas_general?"GE":"??")		\
			 ))))))

rsb_err_t rsb_blas_tester_options_init(struct rsb_tester_options_t * top)
{
	/* This function shall not need any library initialization to work. */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	RSB_BZERO_P(top);
	top->mtt = RSB_TIME_ZERO;
	top->rrm = RSB_BOOL_FALSE;
	top->tur = RSB_BOOL_FALSE;
	top->wqt = RSB_BOOL_FALSE;
	top->wqc = RSB_BOOL_FALSE;
	top->wcs = RSB_BOOL_FALSE;
	return errval;
}

static blas_sparse_matrix rsb_blas_single_allocation_tester(void)
{
	/**
	 * \ingroup gr_internals
	 * */
	blas_sparse_matrix A = RSB_BLAS_INVALID_MATRIX;
	const rsb_coo_idx_t IA[]={0};
	const rsb_coo_idx_t JA[]={0};
	const RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE VA[]={11};
	const RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE X[]={1};
	RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE Y[]={0};
	const RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE alpha = 1.0, beta = 1.0;
	const rsb_coo_idx_t m=1,n=1;
	const int nz=1;
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_FIRST_BLAS;
	if( RSB_NUMERICAL_TYPE_FIRST_BLAS == RSB_NUMERICAL_TYPE_INVALID_TYPE ) 
	{ RSB_INFO("SKIPPING A TEST (no BLAS types in)\n"); goto err; }
	if( (A= rsb__BLAS_Xuscr_begin( m, n, typecode )) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR("error calling BLAS_duscr_begin\n"); goto err;}
	if( rsb__BLAS_Xuscr_insert_entries( A, nz, VA, IA, JA) == RSB_BLAS_ERROR )
	{RSB_ERROR("error calling BLAS_duscr_insert_entries\n"); goto err;}
	if( rsb__BLAS_Xuscr_end(A) == RSB_BLAS_ERROR )
	{RSB_ERROR("error calling BLAS_duscr_end\n"); goto err;}
	if( rsb__BLAS_Xusmv( blas_no_trans, &alpha, A, X, 1, &beta, Y, 1) == RSB_BLAS_ERROR)
	{RSB_ERROR("error calling BLAS_dusmv\n"); goto err;}

	return A;
err:
	if(A != RSB_BLAS_INVALID_MATRIX)
	{
		if (rsb__BLAS_Xusds(A)!=RSB_BLAS_NO_ERROR)
		{RSB_ERROR("error destroying the matrix after error\n"); goto err;}

	}
	return RSB_BLAS_INVALID_MATRIX;
}


static rsb_err_t rsb_blas_allocation_tester(void)
{
	/**
	 * \ingroup gr_internals
	 *  Descriptor handling machinery tester.
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	const rsb_submatrix_idx_t mcount = RSB_MIN(1024,RSB_BLAS_MATRICES_MAX);
	rsb_submatrix_idx_t count=0;
	blas_sparse_matrix bsms[mcount];
	blas_sparse_matrix A = RSB_BLAS_INVALID_MATRIX;

	if( RSB_NUMERICAL_TYPE_FIRST_BLAS == RSB_NUMERICAL_TYPE_INVALID_TYPE ) 
	{ RSB_INFO("SKIPPING A TEST (no BLAS types in)\n"); goto err; }

	for(count=0;count<mcount;++count)
		bsms[count] = RSB_BLAS_INVALID_MATRIX;

	for(count=0;count<mcount;++count)
	{
		A = rsb_blas_single_allocation_tester();
		if(A == RSB_BLAS_INVALID_MATRIX)
		{
			RSB_ERROR(RSB_ERRM_ES);
			errval = RSB_ERR_INTERNAL_ERROR;
			goto out;
		}
		else
			bsms[count]=A;
	}
out:
	if(count<mcount)
		RSB_ERROR("failed allocating %d matrices: only allocated %d!\n",mcount,count);

	for(count=mcount-1;count+1>0;--count)
	{
		if((bsms[count] != RSB_BLAS_INVALID_MATRIX) && (rsb__BLAS_Xusds(bsms[count])==RSB_BLAS_ERROR))
		{
			RSB_ERROR(RSB_ERRM_ES);
			RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_INTERNAL_ERROR);
		}
		bsms[count] = RSB_BLAS_INVALID_MATRIX;
	}
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb_blas_mini_tester(void)
{
	/**
	 * \ingroup gr_internals
	 * */
	blas_sparse_matrix A = RSB_BLAS_INVALID_MATRIX;
	const rsb_coo_idx_t IA[]={0,1,2,3};
	const rsb_coo_idx_t JA[]={0,1,2,3};
	const rsb_coo_idx_t BR[]={2};
	const rsb_coo_idx_t BC[]={2};
	const RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE VA[]={0,11,22,33};
	const RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE X[]={4,3,2,1};
	RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE Y[]={0,0,0,0};
	const RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE alpha = 1.0, beta = 1.0;
	const rsb_coo_idx_t m=4,n=4;
	const int nz=4;
	rsb_char_t optstr[RSB_MAX_LINE_LENGTH];
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_FIRST_BLAS;
	/* const char*tsep="*\n"; */
	const char*tsep="%s";

	if( RSB_NUMERICAL_TYPE_FIRST_BLAS == RSB_NUMERICAL_TYPE_INVALID_TYPE ) 
	{ RSB_INFO("SKIPPING A TEST (no BLAS types in)\n"); goto ret; }

#ifndef RSB_NUMERICAL_TYPE_DOUBLE
	RSB_INFO("SKIPPING BASIC SPARSE BLAS TEST (UNFINISHED TESTING SUITE)\n");
	goto ret; /* FIXME: we are being overly tolerant here, because we don't want to break int-type-only cases */
#endif /* RSB_NUMERICAL_TYPE_DOUBLE */

#if 1
	RSB_INFO("BASIC SPARSE BLAS TEST: BEGIN\n");
	if((A= rsb__BLAS_Xuscr_begin( m, n, typecode )) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xuscr_insert_entries( A, nz, VA, IA, JA) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xuscr_end(A) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_rows ) != m ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_cols ) != n ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_nonzeros ) != nz ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xusmv( blas_no_trans, &alpha, A, X, 1, &beta, Y, 1) != RSB_BLAS_NO_ERROR )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
#if 0
	/* missing lower triangular mark */
	if( BLAS_dussv( blas_no_trans, alpha, A, X, 1) != RSB_BLAS_NO_ERROR )
		goto err;
	RSB_INFO("*\n");
#endif

	
#if RSB_WANT_ALLOCATOR_LIMITS
	if(1)
{
	/* TODO: in the future, may constrain the whole test within memory limits */
	size_t sval=0;
	sval=0;
	RSB_DO_REINIT_SINGLE_VALUE_SET(RSB_IO_WANT_MAX_MEMORY_ALLOCATIONS,&sval,errval); RSB_LSTPROBE(errval,"");
	RSB_DO_REINIT_SINGLE_VALUE_SET(RSB_IO_WANT_MAX_MEMORY_ALLOCATED,&sval,errval); RSB_LSTPROBE(errval,"");
}
#endif /* RSB_WANT_ALLOCATOR_LIMITS */
	if(1)
{
	rsb_int val=0;
	enum rsb_opt_t key=RSB_IO_WANT_EXTRA_VERBOSE_INTERFACE;

	RSB_INFO("INIT INTERFACE TEST: BEGIN\n");
	RSB_DO_REINIT_SINGLE_VALUE_GET(key,&val,errval); RSB_LSTPROBE(errval,"");
	if(val!=-1)
	{ RSB_DO_REINIT_SINGLE_VALUE_SET(key,&val,errval); }
       	RSB_LSTPROBE(errval,"");
	rsb__sprintf(optstr,"got RSB_IO_WANT_EXTRA_VERBOSE_INTERFACE: %d",val);
	if(val!=-1)
	{ RSB_LSTPROBE(rsb__do_set_initopt_as_string("RSB_IO_WANT_EXTRA_VERBOSE_INTERFACE",optstr),""); }
	RSB_INFO("%s\n",optstr);
	
	key=RSB_IO_WANT_IS_INITIALIZED_MARKER;
	RSB_DO_REINIT_SINGLE_VALUE_GET(key,&val,errval); RSB_LSTPROBE(errval,"");
	RSB_DO_REINIT_SINGLE_VALUE_SET(key,&val,errval); RSB_LSTPROBE(errval,"");
	rsb__sprintf(optstr,"%d",val);
	RSB_LSTPROBE(rsb__do_set_initopt_as_string("RSB_IO_WANT_IS_INITIALIZED_MARKER",optstr),"");
	RSB_INFO("got RSB_IO_WANT_IS_INITIALIZED_MARKER: %s\n",optstr);

	RSB_INFO("INIT INTERFACE TEST: END (SUCCESS)\n");
}
	RSB_INFO("PRINT TEST: BEGIN\n");
	errval = rsb__do_file_mtx_save(rsb__BLAS_inner_matrix_retrieve(A),NULL);
	/* rsb_mtx_file_render(rsb__BLAS_inner_matrix_retrieve(A)); */
	errval = rsb__debug_print_vectors_diff(VA,VA+1,nz-1,typecode,1,1,RSB_VECTORS_DIFF_DISPLAY_N);
	errval = rsb__do_mtx_render(NULL,rsb__BLAS_inner_matrix_retrieve(A), 100, 100, RSB_MARF_EPS | RSB_MARF_EPS_B);
	/* rsb__do_file_mtx_rndr(void * pmp, const char * filename, rsb_coo_idx_t pmlWidth, rsb_coo_idx_t pmWidth, rsb_coo_idx_t pmHeight, rsb_marf_t rflags) */
	RSB_INFO("PRINT TEST: END (SUCCESS)\n");

	if(rsb__BLAS_Xusds( A ) == RSB_BLAS_ERROR)
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
#endif
#if 1
	if((A= rsb__BLAS_Xuscr_block_begin( 1, 1, 2, 2, typecode )) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xuscr_insert_block( A, VA, 1, 1, 0, 0) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xuscr_end(A) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}

	if( rsb__BLAS_usgp( A, blas_num_rows ) != 2 ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_cols ) != 2 ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_nonzeros ) != 4 ) {RSB_ERROR(RSB_ERRM_NL); goto err;}

	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xusmv( blas_no_trans, &alpha, A, X, 1, &beta, Y, 1) != RSB_BLAS_NO_ERROR )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if(rsb__BLAS_Xusds( A ) == RSB_BLAS_ERROR)
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
#endif
#if 1
	if((A= rsb__BLAS_Xuscr_variable_block_begin( 1, 1, &BR[0], &BC[0], typecode )) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xuscr_insert_block( A, VA, 1, 1, 0, 0) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xuscr_end(A) == RSB_BLAS_INVALID_MATRIX )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_rows ) != 2 ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_cols ) != 2 ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	if( rsb__BLAS_usgp( A, blas_num_nonzeros ) != 4 ) {RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if( rsb__BLAS_Xusmv( blas_no_trans, &alpha, A, X, 1, &beta, Y, 1) != RSB_BLAS_NO_ERROR )
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
	if(rsb__BLAS_Xusds( A ) == RSB_BLAS_ERROR)
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO(tsep,"");
#endif
	RSB_INFO("BASIC SPARSE BLAS TEST: END (SUCCESS)\n");

#if 0
	RSB_INFO("BIGGER MATRICES SPARSE BLAS TEST: BEGIN\n");
	if(rsb_blas_bigger_matrices_tester(NULL))
		goto err;
	RSB_INFO("BIGGER MATRICES SPARSE BLAS TEST: END\n");
#endif

	RSB_INFO("STRESS SPARSE BLAS TEST: BEGIN\n");
	if(RSB_SOME_ERROR(rsb_blas_allocation_tester()))
	{RSB_ERROR(RSB_ERRM_NL); goto err;}
	RSB_INFO("STRESS SPARSE BLAS TEST: END (SUCCESS)\n");

	RSB_INFO("SPARSE BLAS TESTS: END (SUCCESS)\n");
	goto ret;
err:
	RSB_INFO("SPARSE BLAS TESTS: FAILURE!\n");
	errval = RSB_ERR_GENERIC_ERROR;
ret:
	return errval;
}

static rsb_err_t rsb_basic_primitives_tester(void)
{
	/**
	 * \ingroup gr_internals
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	const size_t n=1024; // FIXME
	rsb_nnz_idx_t i=0;
	rsb_coo_idx_t *cp = rsb__calloc(sizeof(rsb_coo_idx_t)*n);
	rsb_half_idx_t*hp=(rsb_half_idx_t*)cp;
	RSB_INFO("BASIC PRIMITIVES TEST: BEGIN\n");
	if(cp==NULL){ RSB_ERROR(RSB_ERRM_ES); errval = RSB_ERR_ENOMEM; goto err; }
	// RSB_XCOO_ISET(hp,0,n);
       	for(i=0;i<n;++i) hp[i]=i;
	for(i=0;i<n;++i)if(hp[i]!=i){ RSB_ERROR("half word assignment is broken");errval = RSB_ERR_INTERNAL_ERROR; goto err;}
	rsb__do_switch_array_to_fullword_coo(hp,n,0);
	for(i=0;i<n;++i)if(cp[i]!=i){ RSB_ERROR("half to full word conversion is broken (has %d instead of %d)",cp[i],i);errval = RSB_ERR_INTERNAL_ERROR; }
	if(RSB_SOME_ERROR(errval))
		goto err;
	rsb__do_switch_array_to_halfword_coo(cp,n,0);
	for(i=0;i<n;++i)if(hp[i]!=i){ RSB_ERROR("full to half word conversion is broken");errval = RSB_ERR_INTERNAL_ERROR; goto err;}
	
	RSB_CONDITIONAL_FREE(cp);
	
err:
	if(RSB_SOME_ERROR(errval))
	{
		rsb__do_perror(NULL,errval);
		RSB_INFO("BASIC PRIMITIVES TEST: END (FAILURE)\n");
	}
	else
	{
		RSB_INFO("BASIC PRIMITIVES TEST: END (SUCCESS)\n");
	}
	return errval;
}

static rsb_err_t rsb_blas_limit_mul_tester(
		const rsb_coo_idx_t*aIA, const rsb_coo_idx_t*aJA, const void* aVA,
		const rsb_coo_idx_t*bIA, const rsb_coo_idx_t*bJA, const void* bVA,
	       	const rsb_coo_idx_t m, const rsb_coo_idx_t k, const rsb_coo_idx_t n,
	       	const rsb_nnz_idx_t annz, const rsb_nnz_idx_t bnnz, rsb_type_t typecode)
{
	/* FIXME: need a complete checking suite, here.  */
	blas_sparse_matrix A = RSB_BLAS_INVALID_MATRIX;
	blas_sparse_matrix B = RSB_BLAS_INVALID_MATRIX;
	//blas_sparse_matrix C = RSB_BLAS_INVALID_MATRIX;
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	struct rsb_mtx_t * mtxAp=NULL;
	struct rsb_mtx_t * mtxBp=NULL;
	struct rsb_mtx_t * mtxCp=NULL;
	rsb_trans_t trans = RSB_TRANSPOSITION_N;
	if((A = rsb__BLAS_Xuscr_begin( m, k, typecode )) == RSB_BLAS_INVALID_MATRIX ) goto err;
	if( rsb__BLAS_Xuscr_insert_entries( A, annz, aVA, aIA, aJA) == RSB_BLAS_INVALID_MATRIX ) goto err;
	if( rsb__BLAS_Xuscr_end(A) == RSB_BLAS_INVALID_MATRIX ) goto err;
	if((B= rsb__BLAS_Xuscr_begin( k, n, typecode )) == RSB_BLAS_INVALID_MATRIX ) goto err;
	if( rsb__BLAS_Xuscr_insert_entries( B, bnnz, bVA, bIA, bJA) == RSB_BLAS_INVALID_MATRIX ) goto err;
	if( rsb__BLAS_Xuscr_end(B) == RSB_BLAS_INVALID_MATRIX ) goto err;
#if 1
	mtxAp = rsb__BLAS_inner_matrix_retrieve(A);
	mtxBp = rsb__BLAS_inner_matrix_retrieve(B);
	if(!mtxAp || !mtxBp)
	{
		RSB_ERROR(RSB_ERRM_EM);
		goto err;// it's not ok.
	}


#endif
	/* TODO: need a complete checking suite, here.  */
	if((mtxCp = rsb__do_matrix_mul(typecode,RSB_TRANSPOSITION_N,NULL,mtxAp,trans,NULL,mtxBp,&errval))==NULL)
	{
		if(errval == RSB_ERR_LIMITS)
		{
			RSB_INFO("failed computing a dense %d x %d matrix (for internal memory limits reasons--it's ok)!\n",m,n);
			errval = RSB_ERR_NO_ERROR;
		}
		else
		{
			RSB_INFO("failed computing a dense %d x %d matrix (unknown reasons--it's not ok)!\n",m,n);
		}
	}
	else
	{
		if(!rsb__mtx_chk(mtxCp))
		{
			RSB_ERROR("matrix does not seem to be correctly built\n");
		       	RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_INTERNAL_ERROR);
		}
		RSB_MTX_FREE(mtxCp);
	}

	if(rsb__BLAS_Xusds( A ) == RSB_BLAS_ERROR) goto err;
	if(rsb__BLAS_Xusds( B ) == RSB_BLAS_ERROR) goto err;
	goto ret;
err:
	errval = RSB_ERR_INTERNAL_ERROR;
ret:
	return errval;
}

static rsb_err_t rsb_blas_limit_instancing_tester(const rsb_coo_idx_t*IA, const rsb_coo_idx_t*JA, const void* VA, const rsb_coo_idx_t m, const rsb_coo_idx_t k, const rsb_nnz_idx_t nnz, rsb_type_t typecode)
{
	/* FIXME: need a complete checking suite, here.  */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	struct rsb_mtx_t * mtxAp=NULL;
#if 0
	blas_sparse_matrix A = RSB_BLAS_INVALID_MATRIX;
	if((A=BLAS_duscr_begin( m, k )) == RSB_BLAS_INVALID_MATRIX )
		goto err;
//	RSB_INFO("*\n");
	if( rsb__BLAS_Xuscr_insert_entries( A, nnz, VA, IA, JA) == RSB_BLAS_INVALID_MATRIX )
		goto err;
//	RSB_INFO("*\n");
	if( rsb__BLAS_Xuscr_end(A) == RSB_BLAS_INVALID_MATRIX )
		goto err;

	mtxAp = rsb__BLAS_inner_matrix_retrieve(A);
#else
	mtxAp = rsb__do_mtx_alloc_from_coo_const(VA,IA,JA,nnz,typecode,m,k,RSB_DEFAULT_ROW_BLOCKING,RSB_DEFAULT_COL_BLOCKING,RSB_FLAG_NOFLAGS,&errval);
#endif
	if(!mtxAp)
	{
		//RSB_ERROR(RSB_ERRM_EM);
		//goto err;// it's ok.
		if(RSB_SOME_ERROR(errval)) { RSB_ERROR("failed allocating a %d x %d matrix !\n",m,k);  }
	}
	else
	{
		if(!rsb__mtx_chk(mtxAp))
		{
			RSB_ERROR("matrix does not seem to be correctly built\n");
		       	RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_INTERNAL_ERROR);
		}
	}
	/* FIXME: need a complete checking suite, here.  */
#if 0
	if(rsb__BLAS_Xusds( A ) == RSB_BLAS_INVALID_MATRIX)
		goto err;
#else
	RSB_MTX_FREE(mtxAp);
	if(errval == RSB_ERR_LIMITS)
	{
		RSB_INFO("failed instancing of (dense?) %d x %d matrix (it's ok)!\n",m,k);
		errval = RSB_ERR_NO_ERROR;
	}
	else
	{
		errval = RSB_ERR_INTERNAL_ERROR;// goto err;
		goto err;
	}
#endif
//	RSB_INFO("*\n");
	RSB_INFO("instancing %d x %d, %d nnz succeeded\n",m,k,nnz);
	goto ret;
err:
	errval = RSB_ERR_INTERNAL_ERROR;
ret:
	return errval;
}

rsb_err_t rsb_blas_limit_cases_tester(void)
{
	/**
	 * \ingroup gr_internals
	 *
	 * FIXME: shall perform some serious (more iterations) test, here.
	 * FIXME: shall test on limits nonzeroes for various operations (sort, dups, etc)
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
#ifndef RSB_NUMERICAL_TYPE_DOUBLE
	RSB_INFO("SKIPPING BASIC LIMIT CASES TEST (UNFINISHED TESTING SUITE)\n");
#else /* RSB_NUMERICAL_TYPE_DOUBLE */
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_DOUBLE;
	const rsb_nnz_idx_t nnz=4;
	const rsb_coo_idx_t dima[]={
		4,RSB_MAX_SHORTIDX_MATRIX_DIM
		//,RSB_MAX_MATRIX_DIM-1 
		,2<<18 ,2<<20 
		//,RSB_MAX_MATRIX_DIM 
		//RSB_MAX_MATRIX_DIM+1000000+RSB_NNZ_BLK_MAX-3 
		,2<<22 ,2<<24 
		//,2<<26 ,2<<28 
		//,RSB_MAX_MATRIX_DIM-1000000 
		, RSB_MAX_MATRIX_DIM 
		};
	//const rsb_coo_idx_t dima[]={2};
	rsb_int_t dimi;
	RSB_INFO("BASIC LIMIT CASES TEST: BEGIN\n");
	RSB_INFO("BASIC LIMIT CASES TEST: BEGIN\n");
	RSB_INFO("(please do not worry if some tests fail due to insufficient memory)\n");
#if 1
	RSB_INFO("(forcing allocations to be memory resident)\n");
	rsb__lock_as_memory_resident(RSB_BOOL_TRUE); /* TODO: check return value here! */
#else
#endif

	if(1)
	for(dimi=0;dimi<sizeof(dima)/sizeof(dima[0]);++dimi)
	{
		const rsb_coo_idx_t dim=dima[dimi];
		rsb_coo_idx_t IA[nnz];
		rsb_coo_idx_t JA[nnz];
		const double VA[]={11,22,33,44};
		//const double X[dim],Y[dim]; const double alpha = 1.0;
		//rsb__util_set_array_to_converted_integer(X,typecode,m,1,1);
		//rsb__util_set_array_to_converted_integer(Y,typecode,k,1,0);
		rsb_coo_idx_t m,k;
		RSB_INFO("testing instantiation %d-sized, %d nnz\n",dim,nnz);
		/* * * * * * * * * * * * * * * * * * * * * * * */
		/* FIXME: need a `rotation' routine, here      */
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=dim,k=dim;
		IA[0]=0; IA[1]=0; IA[2]=dim-1; IA[3]=dim-1;
		JA[0]=0; JA[1]=dim-1; JA[2]=0; JA[3]=dim-1;
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, nnz, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=1,k=dim;
		IA[0]=0; IA[1]=0; IA[2]=0; IA[3]=0;
		JA[0]=0; JA[1]=1; JA[2]=dim-2; JA[3]=dim-1;
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, nnz, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=dim,k=1;
		IA[0]=0; IA[1]=1; IA[2]=dim-2; IA[3]=dim-1;
		JA[0]=0; JA[1]=0; JA[2]=0; JA[3]=0;
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, nnz, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=dim,k=dim; IA[0]=0; JA[0]=0; 
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, 1, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=dim,k=dim; IA[0]=dim-1; JA[0]=0; 
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, 1, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=dim,k=dim; IA[0]=0; JA[0]=dim-1; 
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, 1, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
		m=dim,k=dim; IA[0]=dim-1; JA[0]=dim-1; 
		errval = rsb_blas_limit_instancing_tester(IA, JA, VA, m, k, 1, typecode);
		if(RSB_SOME_ERROR(errval)) {   }
		/* * * * * * * * * * * * * * * * * * * * * * * */
	}

	if(1)
	{
		const rsb_nnz_idx_t dim = RSB_MAX_SHORTIDX_MATRIX_DIM+1;
		//const rsb_nnz_idx_t dim=10;
		rsb_coo_idx_t*aIA=NULL, *aJA=NULL, *bIA=NULL, *bJA=NULL;
		void* aVA=NULL, * bVA=NULL;
	       	const rsb_coo_idx_t m=dim, k=dim, n=dim;
	       	const rsb_nnz_idx_t annz=dim+1, bnnz=dim+1;
		rsb_type_t typecode = RSB_NUMERICAL_TYPE_DOUBLE;
		/* size_t el_size = RSB_NUMERICAL_TYPE_SIZE(typecode); */
		RSB_INFO("testing spmult for %d-sized, %d nnz\n",dim,nnz);
		if(RSB_SOME_ERROR(errval = rsb_util_coo_alloc(&aVA,&aIA,&aJA,annz,typecode,RSB_BOOL_TRUE))){goto erra;}
		if(RSB_SOME_ERROR(errval = rsb_util_coo_alloc(&bVA,&bIA,&bJA,bnnz,typecode,RSB_BOOL_TRUE))){goto erra;}
		rsb__util_coo_array_set(aJA,annz,0);
		rsb__util_coo_array_set_sequence(aIA,annz,0,1);
		rsb__util_coo_array_set(bIA,bnnz,0);
		rsb__util_coo_array_set_sequence(bJA,bnnz,0,1);
		aIA[annz-1]=dim/2; aJA[bnnz-1]=dim/2;
		bIA[annz-1]=dim/2; bJA[bnnz-1]=dim/2;
		if(RSB_SOME_ERROR(rsb__fill_with_ones (aVA,typecode,dim,1))){ errval = RSB_ERR_INTERNAL_ERROR; goto erra; }
		if(RSB_SOME_ERROR(rsb__fill_with_ones (bVA,typecode,dim,1))){ errval = RSB_ERR_INTERNAL_ERROR; goto erra; }
		errval = rsb_blas_limit_mul_tester( aIA, aJA, aVA, bIA, bJA, bVA, m, k, n, annz, bnnz, typecode);
	erra:
		RSB_CONDITIONAL_FREE(aIA);
		RSB_CONDITIONAL_FREE(aJA);
		RSB_CONDITIONAL_FREE(aVA);
		RSB_CONDITIONAL_FREE(bIA);
		RSB_CONDITIONAL_FREE(bJA);
		RSB_CONDITIONAL_FREE(bVA);
		if(RSB_SOME_ERROR(errval)) {RSB_ERROR("!\n"); goto err;}
	}

	RSB_INFO("BASIC LIMIT CASES TEST: END\n");
	goto ret;
err:
	RSB_INFO("BASIC LIMIT CASES TEST: END : FAILURE\n");
ret:
#endif /* RSB_NUMERICAL_TYPE_DOUBLE */
	return errval;
}

	
#if RSB_WANT_COO_BEGIN 
static rsb_err_t rsb_mtx_alloc_from_coo_test(void)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	const rsb_nnz_idx_t nnzA=4;		/* matrix nonzeroes count */
	const rsb_coo_idx_t  nrA=3;		/* matrix rows count */
	const rsb_coo_idx_t  ncA=3;		/* matrix columns count */
	rsb_coo_idx_t IA[]={0,1,2,2};
	rsb_coo_idx_t JA[]={0,1,2,2};
	RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE VA[]={11,22,32,1};/* values of nonzeroes */
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_FIRST_BLAS;
	struct rsb_mtx_t * mtxAp = NULL;

	if( RSB_NUMERICAL_TYPE_FIRST_BLAS == RSB_NUMERICAL_TYPE_INVALID_TYPE ) 
	{ RSB_INFO("SKIPPING A TEST (no BLAS types in)\n"); goto ret; }

	mtxAp = rsb__do_mtx_alloc_from_coo_begin(nnzA,typecode,nrA,ncA,RSB_FLAG_NOFLAGS,&errval);
	if(RSB_SOME_ERROR(errval))goto err;
	if( mtxAp == NULL ){ errval = RSB_ERR_INTERNAL_ERROR;goto err; }
	if(RSB_SOME_ERROR(errval = rsb__do_set_elements(mtxAp,VA,IA,JA,nnzA,RSB_FLAG_NOFLAGS)))goto err;
	if(RSB_SOME_ERROR(errval = rsb__do_mtx_alloc_from_coo_end(&mtxAp)))goto err;
	RSB_MTX_FREE(mtxAp);
	goto ret;
err:
	RSB_INFO("!\n");
ret:
	return errval;
}
#endif /* RSB_WANT_COO_BEGIN  */

#if RSB_ALLOW_INTERNAL_GETENVS
static void rsb__txt_ar(const char*c, int* ap, int*lp)
{
	int nul = 0,ci,l=0;

	if(!c)
		goto err;

	do
	{
		while(*c!=nul && !isdigit(*c))++c;
		ci = rsb__util_atoi(c);/* Flawfinder: ignore */

		if(isdigit(*c))
			ap[l++] = ci;
		while(*c && isdigit(*c))++c;
	}
	while(*c);

       	RSB_ASSIGN_IF(lp,l)
err:
	return;
}
#endif /* RSB_ALLOW_INTERNAL_GETENVS */

rsb_err_t rsb_blas_bigger_matrices_tester(struct rsb_tester_options_t * top)
{
	/**
	 * \ingroup gr_internals
	 * */
	rsb_err_t errvalf = RSB_ERR_NO_ERROR;
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	//  full blas compliance:
	enum blas_trans_type transTa[]={blas_no_trans,blas_trans,blas_conj_trans};
	enum blas_symmetry_type stypea[]={
		blas_lower_triangular
		,blas_upper_triangular	
		,blas_lower_symmetric
		,blas_general
		/* ,blas_upper_symmetric*/	/* one symmetry is enough for testing purposes ... */
		,blas_lower_hermitian
		//,blas_upper_hermitian
	};
	rsb_blas_int_t incXa[]={1,2};
	rsb_blas_int_t incBa[]={1,2};
	rsb_blas_int_t alphaa[]={-2,-1,1,2};
#if (RSB_IMPLEMENTED_SOME_BLAS_TYPES>0)
	rsb_type_t typecodea[]=RSB_MATRIX_SPBLAS_TYPE_CODES_ARRAY;
#else /* RSB_IMPLEMENTED_SOME_BLAS_TYPES */
	rsb_type_t typecodea[]={RSB_NUMERICAL_TYPE_INVALID_TYPE};/* bogus definition */
#endif /* RSB_IMPLEMENTED_SOME_BLAS_TYPES */
	enum blas_diag_type diaga[]={blas_non_unit_diag,blas_unit_diag};

	// FIXME: should implement a routine to conjugate complex matrices before testing !

	//enum blas_trans_type transTa[]={blas_no_trans};
	//enum blas_trans_type transTa[]={blas_trans};
	//enum blas_trans_type transTa[]={blas_conj_trans};
	//enum blas_trans_type transTa[]={blas_no_trans,blas_trans};
	//enum blas_symmetry_type stypea[]={blas_lower_triangular};
	//enum blas_symmetry_type stypea[]={blas_upper_triangular};
	//rsb_blas_int_t incXa[]={1};
	//rsb_blas_int_t incBa[]={2};
	//rsb_blas_int_t alphaa[]={1};
	//rsb_blas_int_t alphaa[]={-1};
	//rsb_blas_int_t incXa[]={2};
	//rsb_blas_int_t alphaa[]={-1,1};
	//rsb_blas_int_t alphaa[]={-2};
	rsb_blas_int_t betaa[]={1,0};// FIXME: there the Sparse BLAS interface works only with beta=1
	//rsb_type_t typecodea[]={RSB_NUMERICAL_TYPE_DOUBLE};
	//rsb_type_t typecodea[]={RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX};
	//rsb_type_t typecodea[]={RSB_NUMERICAL_TYPE_FLOAT};
	//rsb_type_t typecodea[]=RSB_MATRIX_TYPE_CODES_ARRAY;

	//const rsb_blas_int_t dima_lcl=1;
	const rsb_blas_int_t dima_lcl=2;
	const rsb_blas_int_t dima_pcl=dima_lcl;
	const rsb_blas_int_t dima_tcl=dima_lcl;
	//enum blas_diag_type diaga[]={blas_unit_diag};
	//enum blas_diag_type diaga[]={blas_non_unit_diag};
	//enum blas_diag_type diaga[]={blas_unit_diag};
	const rsb_blas_int_t dimas=dima_pcl+dima_tcl*RSB_MAX_SUPPORTED_CACHE_LEVELS+dima_lcl;
	rsb_blas_int_t dima[dimas];
	rsb_blas_int_t dims=0;
	rsb_blas_int_t diagi=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t transTi=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t alphai=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t betai=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t stypei=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t incXi=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t dimi=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t typecodei=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t incBi=RSB_INVALID_BLAS_INT_IDX_VAL;
	rsb_blas_int_t cl=RSB_INVALID_BLAS_INT_IDX_VAL,cln = rsb__get_cache_levels_num();
	rsb_blas_int_t passed=0,failed=0;
	rsb_blas_int_t instantiated_some_recursive=0;
#if RSB_TESTER_ALLOW_TIMEOUT
	rsb_time_t tt = RSB_TIME_ZERO,tt0=rsb_time();
	struct rsb_tester_options_t to;
#endif /* RSB_TESTER_ALLOW_TIMEOUT */
	rsb_blas_int_t isempty=0,isinvertible=1;
/* FIXME: in the future, may use these indices (isemptym) to fill the matrix with a particular value */
#if RSB_ALLOW_EMPTY_MATRICES
#if RSB_ALLOW_ZERO_DIM
	rsb_blas_int_t isemptym=3;
#else
	rsb_blas_int_t isemptym=2;
#endif
#else /* RSB_ALLOW_EMPTY_MATRICES */
	rsb_blas_int_t isemptym=1;
#endif /* RSB_ALLOW_EMPTY_MATRICES */
	const rsb_char_t*btps=RSB_EMPTY_STRING;
	rsb_int_t iat=1;
#if RSB_ALLOW_INTERNAL_GETENVS
	rsb_blas_int_t maxtc = 0; /* max tests count, current tests count  */
#endif /* RSB_ALLOW_INTERNAL_GETENVS */

	if( (sizeof(typecodea)==0)
#if (RSB_IMPLEMENTED_SOME_BLAS_TYPES==0)
	|| 1		
#endif /* RSB_IMPLEMENTED_SOME_BLAS_TYPES */
	)
	{
		// FIXME: new
		RSB_INFO("Did not configure any BLAS-standard type: thus skipping BLAS-based testing.\n");
		goto ret;
	}
#if RSB_TESTER_ALLOW_TIMEOUT
	if(!top)
		errval = rsb_blas_tester_options_init(&to);
	else
		rsb_memcpy(&to,top,sizeof(to));
#endif /* RSB_TESTER_ALLOW_TIMEOUT */
	if(RSB_SOME_ERROR(errval))
	{
		RSB_ERROR("!\n");
		goto ret;
	}
	errval = rsb_basic_primitives_tester();
	if(RSB_SOME_ERROR(errval))
	{
		RSB_ERROR("!\n");
		goto ret;
	}

	#if RSB_WANT_COO_BEGIN 
	/* FIXME: this mini-test is insufficient! */
	if(RSB_SOME_ERROR(errval=rsb_mtx_alloc_from_coo_test()))
	{ RSB_ERROR("!\n"); goto ret; }
	#endif /* RSB_WANT_COO_BEGIN  */


#if RSB_HAVE_ISATTY
#if RSB_HAVE_STREAMS
	if( rsb_global_session_handle.out_stream )
		iat=( isatty(rsb__fileno(rsb_global_session_handle.out_stream)) );
	else
		iat=0;
#endif /* RSB_HAVE_STREAMS */
#endif /* RSB_HAVE_ISATTY */

	if(to.wcs==RSB_BOOL_TRUE)
		btps=RSB_CLEARTERM_STRING; 
	if((to.wqc==RSB_BOOL_TRUE) && (!iat))
		to.wqt=RSB_BOOL_TRUE;
	RSB_INFO("ADVANCED SPARSE BLAS TEST: BEGIN\n");
#if 1
	for(cl=0;cl<dima_pcl;++cl)
	{
		// 1,2,.. 
		dima[dims++]=1<<cl;
	}
#else
//	if(dims<dimas)dima[dims++]=39;
//	if(dims<dimas)dima[dims++]=724;
// 	if(dims<dimas)dima[dims++]=362;
//	if(dims<dimas)dima[dims++]=1;
	if(dims<dimas)dima[dims++]=2;
//	if(dims<dimas)dima[dims++]=3;
#endif
#if 1
	for(cl=1;cl<=cln && dims<dimas;++cl)
	{
		// around cache size
		long cs = rsb__get_lnc_size(cl);
		rsb_blas_int_t i;
		for(i=1;i<=dima_tcl;++i)
			dima[dims++]=((1<<i)*2*sqrt(cs))/(4*sizeof(rsb_coo_idx_t));
	}
	if((cl=cln)>0)
	{
		// more than outermost cache size
		rsb_blas_int_t i;
		long cs = rsb__get_lnc_size(cl);
		for(i=1;i<=dima_lcl && dims<dimas;++i)
		{
			long ndim=(((i)*(1<<dima_tcl)*2*sqrt(cs))/(4*sizeof(rsb_coo_idx_t)));
			if(ndim > dima[dims]) // to avoid duplicates
				dima[dims++]=ndim;
		}
	}
#endif

#if RSB_ALLOW_INTERNAL_GETENVS
	rsb__txt_ar(getenv("RSB_BMT_ALPHA"),  &   alphaa[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_INCXA"),  &    incXa[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_INCBA"),  &    incBa[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_SYMMA"),  &   stypea[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_STYPA"),  &typecodea[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_DIAGA"),  &    diaga[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_TRANSA"), &  transTa[0], NULL);
	rsb__txt_ar(getenv("RSB_BMT_DIMA"),   &     dima[0],&dims);
#if RSB_ALLOW_EMPTY_MATRICES
	if(getenv("RSB_BMT_ISEMPTYM")) isemptym = rsb__util_atoi(getenv("RSB_BMT_ISEMPTYM"));
#endif /* RSB_ALLOW_EMPTY_MATRICES */
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
	
#if 1
	if(
		(rsb_do_psblas_trans_to_rsb_trans(RSB_PSBLAS_TRANS_N) != RSB_TRANSPOSITION_N) ||
		(rsb_do_psblas_trans_to_rsb_trans(RSB_PSBLAS_TRANS_T) != RSB_TRANSPOSITION_T) ||
		(rsb_do_psblas_trans_to_rsb_trans(RSB_PSBLAS_TRANS_C) != RSB_TRANSPOSITION_C)
		)
	{RSB_ERROR("!\n"); goto err;}
#endif

	//dims=0;
	//dima[dims++]=45;
	//dima[dims++]=362;
	//dima[dims++]=500;
	//dima[dims++]=499;
	//dima[dims++]=724;
	//dima[dims++]=1448;
//	typecodei=3;
	for(dimi=0;dimi<dims;++dimi)
	for(stypei=0;stypei<sizeof(stypea)/sizeof(enum blas_symmetry_type);++stypei)
	for(typecodei=0;typecodei<sizeof(typecodea)/sizeof(rsb_type_t);++typecodei)
	for(incXi=0;incXi<sizeof(incXa)/sizeof(rsb_blas_int_t);++incXi)
	for(incBi=0;incBi<sizeof(incBa)/sizeof(rsb_blas_int_t);++incBi)
	for(alphai=0;alphai<sizeof(alphaa)/sizeof(rsb_blas_int_t);++alphai)
	for(betai=0;betai<sizeof(betaa)/sizeof(rsb_blas_int_t);++betai)
	for(diagi=0;diagi<sizeof(diaga)/sizeof(enum blas_diag_type);++diagi)
	for(transTi=0;transTi<sizeof(transTa)/sizeof(enum blas_trans_type);++transTi)
#if RSB_ALLOW_EMPTY_MATRICES
	for(isempty=0;isempty<isemptym;++isempty)
#endif /* RSB_ALLOW_EMPTY_MATRICES */
	{
		const rsb_blas_int_t is_really_empty=isempty && (diaga[diagi]!=blas_unit_diag);
		blas_sparse_matrix T = RSB_BLAS_INVALID_MATRIX;
		void *B=NULL,*X=NULL,*D=NULL;
		rsb_blas_int_t dim=dima[dimi];
	       	rsb_coo_idx_t *IA=NULL,*JA=NULL;
		void * VA=NULL;
	       	rsb_blas_int_t nnz = RSB_BLAS_INT_MAX;
		rsb_type_t typecode=typecodea[typecodei];
		enum blas_trans_type transT=transTa[transTi];
		rsb_blas_int_t incX=incXa[incXi],incB=incBa[incBi],incD=1;
		size_t el_size = RSB_NUMERICAL_TYPE_SIZE(typecode);
		enum blas_symmetry_type stype=stypea[stypei];
		rsb_aligned_t alpha_inv[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t inrm[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t inrm_[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t alpha[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t beta[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		struct rsb_mtx_t * mtxAp=NULL;
		struct rsb_mtx_t *cmatrix=NULL;
		struct rsb_mtx_t *kmatrix=NULL;
		size_t extra_vels=0;
		rsb_nnz_idx_t rnnz=0,ndnnz,rnz=0;
		rsb_submatrix_idx_t submatrices=0;
		rsb_trans_t trans = rsb_blas_trans_to_rsb_trans(transT);
		rsb_char_t tc = RSB_TRANSPOSITION_AS_CHAR(trans);
	       	rsb_blas_int_t mmi,msmd=100;
		rsb_coo_idx_t coov;
		rsb_nnz_idx_t nnzv;
		rsb_aligned_t zero[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t one[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t two[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t three[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_int do_tune_test = 0; 
#if RSB_ALLOW_ZERO_DIM
		if(isempty>=2 && dim > 1) continue;
		if(isempty>=2) dim=0;
#endif
		rsb__util_set_area_to_converted_integer(one,typecode,1);
		rsb__util_set_area_to_converted_integer(two,typecode,2);
		rsb__util_set_area_to_converted_integer(three,typecode,3);

		rsb__util_set_area_to_fraction_of_integer(alpha_inv,alphaa[alphai],typecode);

		// ... need asserts ...
		rsb__util_set_area_to_converted_integer(alpha,typecode,alphaa[alphai]);
		rsb__util_set_area_to_converted_integer(beta,typecode,betaa[betai]);
		rsb__util_set_area_to_converted_integer(zero,typecode,0);
#if RSB_ALLOW_ZERO_DIM
		if(isempty>=2) extra_vels=1;
#endif
		X = rsb__calloc(el_size*(dim*incX+extra_vels));
		B = rsb__calloc(el_size*(dim*incB+extra_vels));
		D = rsb__calloc(el_size*(dim*incD+extra_vels));
		if(!X || !B || !D)
		{
			RSB_ERROR("failed allocating a vector!\n"); goto err;
		}

		/* generate a triangular matrix */
		/* FIXME: should make sure that the matrix is recursive, somehow. */
		errval = rsb__generate_dense_lower_triangular_coo(dim,1,&IA,&JA,&VA,&nnz,typecode);
		if(RSB_SOME_ERROR(errval))
		{
			RSB_ERROR("!\n"); goto err;
		}
#if RSB_ALLOW_EMPTY_MATRICES
		if(isempty)
		{
			RSB_DO_ERROR_CUMULATE(errval,rsb__cblas_Xscal(typecode,nnz,&zero,VA,1));
			if(RSB_SOME_ERROR(errval))
				{RSB_ERROR("!\n"); goto err;}
		}
#endif /* RSB_ALLOW_EMPTY_MATRICES */
		isinvertible=(diaga[diagi]==blas_unit_diag||!isempty);
		isinvertible&=(stype != blas_general);
	/*	isinvertible&=(stype != blas_lower_symmetric);
		isinvertible&=(stype != blas_upper_symmetric);
	       	*/
		isinvertible&=(stype != blas_upper_hermitian);
		isinvertible&=(stype != blas_lower_hermitian);
		ndnnz=nnz-(diaga[diagi]==blas_unit_diag?dim:0);
		if(ndnnz>nnz)
		{
			RSB_ERROR("!\n"); goto err;
		}
		if(nnz > 0 && (!VA || !IA || !JA))
		{
			RSB_ERROR("!\n");
			goto err;
		}

		if(stype==blas_upper_triangular)
			RSB_SWAP(rsb_coo_idx_t*,IA,JA);

#if 1 /* 20110425 */
		if(incX==1)
		if(incB==1)/* FIXME: shall propagate incX to the test routine, someday */
		if(nnz>0)/* empty matrices are not supported for now */
		if(!(diaga[diagi]==blas_unit_diag))/* FIXME: the accuracy test needs cleaned up input (i.e.: won't remove the diagonal) */

		if(!RSB_IS_MATRIX_TYPE_COMPLEX(typecode))/* FIXME: shall fix many vector-operating routines, first */
		{
			/* FIXME: to be complete, shall implement symmetry/lower/upper/diagonal flags */
			rsb_flags_t aflags = RSB_FLAG_NOFLAGS;
			struct rsb_coo_matrix_t coo;
			if(stype==blas_lower_symmetric) RSB_DO_FLAG_ADD(aflags,RSB_FLAG_SYMMETRIC);
			if(stype==blas_upper_symmetric) RSB_DO_FLAG_ADD(aflags,RSB_FLAG_SYMMETRIC);
			if(stype==blas_upper_hermitian) RSB_DO_FLAG_ADD(aflags,RSB_FLAG_UPPER_HERMITIAN);
			if(stype==blas_lower_hermitian) RSB_DO_FLAG_ADD(aflags,RSB_FLAG_LOWER_HERMITIAN);
			if(diaga[diagi]==blas_unit_diag)RSB_DO_FLAG_ADD(aflags,RSB_FLAG_UNIT_DIAG_IMPLICIT);
			rsb__fill_coo_struct(&coo,VA,IA,JA,dim,dim,nnz,typecode);
			RSB_DO_ERROR_CUMULATE(errval,rsb__do_spmv_accuracy_test(&coo,NULL,0,aflags));
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("!\n");
				RSB_DO_ERROR_CUMULATE(errval,rsb__do_spmv_accuracy_test(&coo,NULL,0,aflags));
				goto err;
			}
		}
#endif
		T = rsb__BLAS_Xuscr_begin(dim,dim,typecode);
		if( T == RSB_BLAS_INVALID_MATRIX )
			{errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while calling uscr_begin\n"); goto err;}
		if( BLAS_ussp(T,stype) != RSB_BLAS_NO_ERROR )
			{errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while calling ussp(%d)\n",stype); goto err;}
		if( BLAS_ussp(T,diaga[diagi]) != RSB_BLAS_NO_ERROR )
			{errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while calling ussp(%d)\n",diaga[diagi]); goto err;}
		if( rsb__BLAS_Xuscr_insert_entries(T,nnz,VA,IA,JA) != RSB_BLAS_NO_ERROR)
			{errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while calling cr_insert_entries\n"); goto err;}
		if( rsb__BLAS_Xuscr_end(T) != RSB_BLAS_NO_ERROR )
			{errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error finalizing matrix!\n"); goto err;}
		mtxAp = rsb__BLAS_inner_matrix_retrieve(T);
		if(!mtxAp)
		{
			RSB_ERROR(RSB_ERRM_NL);
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
		}

		if( mtxAp->nnz>0 && rsb__get_index_storage_amount(mtxAp)==0 )
		{
			RSB_ERROR(RSB_ERRM_NL);
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
		}

		if(diaga[diagi]==blas_non_unit_diag) /* FIXME */
		if(nnz > 0)
		{
			if( rsb__BLAS_Xusset_elements(T, IA, JA, VA, mtxAp->nnz) != RSB_BLAS_NO_ERROR )
			{
				RSB_ERROR(RSB_ERRM_NL);
			       	errval = RSB_ERR_INTERNAL_ERROR;
			       	goto err;
			}
		}

		rnz=mtxAp->nnz;
		if(!rsb__mtx_chk(mtxAp))
		{
			RSB_ERROR(RSB_ERRM_NL);
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
		}
		
		coov=0;
		rsb__do_mtx_get_info(mtxAp,             RSB_MIF_MATRIX_COLS__TO__RSB_COO_INDEX_T ,&coov);
		if(coov!=dim){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL);goto err;}
		coov=0;
		rsb__do_get_matrix_info_from_string(mtxAp,"RSB_MIF_MATRIX_COLS__TO__RSB_COO_INDEX_T",&coov,0);
		if(coov!=dim){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL);goto err;}

		coov=0;
		rsb__do_mtx_get_info(mtxAp,             RSB_MIF_MATRIX_ROWS__TO__RSB_COO_INDEX_T ,&coov);
		if(coov!=dim){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL);goto err;}
		coov=0;
		rsb__do_get_matrix_info_from_string(mtxAp,"RSB_MIF_MATRIX_ROWS__TO__RSB_COO_INDEX_T",&coov,0);
		if(coov!=dim){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL);goto err;}

		nnzv=0;
		rsb__do_mtx_get_info            (mtxAp, RSB_MIF_MATRIX_NNZ__TO__RSB_NNZ_INDEX_T ,&nnzv);
		if(nnzv!=mtxAp->nnz){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL);goto err;}
		nnzv=0;
		rsb__do_get_matrix_info_from_string(mtxAp,"RSB_MIF_MATRIX_NNZ__TO__RSB_NNZ_INDEX_T",&nnzv,0);
		if(nnzv!=mtxAp->nnz){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL);goto err;}
		
		{
			/* TODO: need a systematic tester */
			const int errstrlen=128;
			char errstr[errstrlen];
			strcpy(errstr,"");
			rsb__do_strerror_r(RSB_ERR_BADARGS,errstr,errstrlen);  
			if(strlen(errstr)<1)
			RSB_LSTERR(RSB_ERRM_ES);
			
		}

		/* TODO: extract && test other values as well... */

//		submatrices = rsb__submatrices(mtxAp);
		submatrices = rsb__terminal_recursive_matrix_count(mtxAp);
		instantiated_some_recursive+=(rsb__submatrices(mtxAp)>1?1:0);
		if(rsb__get_sizeof(mtxAp)<(
		(mtxAp->el_size*mtxAp->nnz)+ (sizeof(rsb_half_idx_t)*mtxAp->nnz)+0))
		{
			errval = RSB_ERR_INTERNAL_ERROR;
			RSB_ERROR("!\n");
			goto err;
		}

#if 1
#if RSB_ALLOW_INTERNAL_GETENVS
		if(! ( getenv("RSB_BMT_SPMV") && rsb__util_atoi(getenv("RSB_BMT_SPMV")) == 0 ) )
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
		if(RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_SYMMETRIC) || RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_HERMITIAN))
		{
			/* FIXME: this gets NOT covered, it seems  */
		for(mmi=0;mmi< (dim<msmd?3:2) ;++mmi)
		if(! (mmi==1 && ((incX!= 1) || (incB!=1) )  ))
		{
			const int nrhs=1;/* TODO: need more ... */
			/* TODO : should fill X and B with sentinel values ! */
			if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,X,incX)) || RSB_SOME_ERROR(rsb__fill_with_ones (B,typecode,dim,incB)))
			{ errval = RSB_ERR_INTERNAL_ERROR; goto err; }


			if(mmi==0)
			if( rsb__BLAS_Xusmv(transT,alpha,T,B,incB,beta,X,incX) != RSB_BLAS_NO_ERROR )
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("Symmetric USMV failed!\n"); goto err;
			}

			if(mmi==1)
			if(RSB_SOME_ERROR( rsb__do_spmm(trans,alpha,mtxAp,nrhs,RSB_FLAG_WANT_COLUMN_MAJOR_ORDER,B,dim,beta,X,dim,RSB_OP_FLAG_DEFAULT)) )
			{
			       	errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("Symmetric USMV failed!\n"); goto err;
		       	}

			if(mmi==2)
			if(RSB_SOME_ERROR( rsb_do_spmv_general(trans,alpha,mtxAp,B,incB,beta,X,incX,RSB_OP_FLAG_WANT_SERIAL RSB_DEFAULT_OUTER_NRHS_SPMV_ARGS)))
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("Symmetric USMV failed!\n"); goto err;
			}

			/* if(!isinvertible) */
			if(is_really_empty)
				rsb__util_set_array_to_converted_integer(B,typecode,dim,incB,0                 );
			else
			{
				if(isempty)
					rsb__util_set_array_to_converted_integer(B,typecode,dim,incB,    alphaa[alphai]);
				else
					rsb__util_set_array_to_converted_integer(B,typecode,dim,incB,dim*alphaa[alphai]);
			}
			if( RSB_SOME_ERROR(rsb__do_are_same(B,X,dim,typecode,incB,incX)) )
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("Symmetric USMV computed wrong results!\n"); goto err;
			}
			goto err; /* ok. skip the remaining tests FIXME */
		}
		}
#endif
#if 1
#if RSB_ALLOW_INTERNAL_GETENVS
		if(! ( getenv("RSB_BMT_SPGEMM") && rsb__util_atoi(getenv("RSB_BMT_SPGEMM")) == 0 ) )
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
		// FIXME: this is quite slow : SPGEMM is O(dim^3, and so shall be limited down to a certain threshold)
		if(dimi <= RSB_WANT_SPGEMM_TESTING_FOR_ONLY_FIRST_DIMI)// FIXME: spgemm cost increases quadratically..
		if(incX==1 && incB==1)
		if(stype==blas_lower_triangular || stype==blas_upper_triangular)
		if(!RSB_DO_TOOFEWNNZFORCSR(nnz,dim) /*&& typecode == RSB_NUMERICAL_TYPE_DOUBLE*/ )
		if(diaga[diagi]==blas_non_unit_diag)
		if( trans == RSB_TRANSPOSITION_N ) /* FIXME: this is just a workaround, 20140324 */
		{
			rsb_nnz_idx_t ldd=2*dim;
			rsb_bool_t rowmajor=(stype==blas_lower_triangular)?RSB_BOOL_TRUE:RSB_BOOL_FALSE;
			//rsb_nnz_idx_t ldd=1*dim;
			void *dVA = rsb__calloc(el_size*dim*ldd);
			rsb_aligned_t sum1[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
			rsb_aligned_t sum2[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
			rsb_aligned_t two[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];

			if(dVA)
			{
				/* rsb_trans_t ttrans = rsb__do_transpose_transposition(trans); */
				rsb_trans_t ttrans = (trans); /* FIXME: this is just a workaround, 20140324 */

				RSB_DO_ERROR_CUMULATE(errval,rsb__do_spgemm_to_dense(typecode,trans,alpha,mtxAp,ttrans,beta,mtxAp,ldd,dim,dim,rowmajor,dVA,NULL,NULL));
				if(RSB_SOME_ERROR(errval)) { RSB_ERROR("!\n"); goto err; }
				RSB_DO_ERROR_CUMULATE(errval,rsb__util_vector_sum(sum1,dVA,typecode,ldd*dim));
				if(RSB_SOME_ERROR(errval)) { RSB_ERROR("!\n"); goto err; }
				// now: c <- alpha a ^ trans * beta a ^ trans
				RSB_DO_ERROR_CUMULATE(errval,rsb__do_spgemm_to_dense(typecode,trans,alpha,mtxAp,ttrans,beta,mtxAp,ldd,dim,dim,rowmajor,dVA,NULL,NULL));
				if(RSB_SOME_ERROR(errval)) { RSB_ERROR("!\n"); goto err; }
				RSB_DO_ERROR_CUMULATE(errval,rsb__util_vector_sum(sum2,dVA,typecode,ldd*dim));
				if(RSB_SOME_ERROR(errval)) { RSB_ERROR("!\n"); goto err; }
				// now: c <- 2 ( alpha a ^ trans * beta a ^ trans )
				if(RSB_SOME_ERROR(errval))
				{
					RSB_CONDITIONAL_FREE(dVA);
					RSB_ERROR(RSB_ERRM_FMMTDT);
		       			errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}
				rsb__util_set_area_to_converted_integer(two,typecode,2);
				rsb__util_vector_div(sum2,two,typecode,1);
				// TODO: there is risk of overflow, though..
				if( RSB_SOME_ERROR(rsb__do_are_same(sum1,sum2,1,typecode,1,1) ))
				{
					RSB_CONDITIONAL_FREE(dVA);
					RSB_ERROR(RSB_ERRM_FMMTDT);
		       			errval = RSB_ERR_INTERNAL_ERROR; goto err;
			       	}
				// since a is lower triangular full with ones,
				// c is full, with values (given s=2(alpha+beta))
				//   s   s   s   s ...
				//   s  2s  2s  2s ...
				//   s  2s  3s  3s ...
				//   s  2s  3s  4s ...
				//   ...
				// (transposed, in the case trans is)
				// now: c <- 2 ( alpha a ^ trans * beta ^ trans ) + alpha a ^ trans

				RSB_DO_ERROR_CUMULATE(errval,rsb__cblas_Xscal(typecode,ldd*dim,&zero,dVA,1));
				RSB_DO_ERROR_CUMULATE(errval,rsb__do_matrix_add_to_dense(alpha,mtxAp,ldd,dim,dim,rowmajor,dVA));
				RSB_DO_ERROR_CUMULATE(errval,rsb__util_vector_sum(sum1,dVA,typecode,ldd*dim));
				RSB_DO_ERROR_CUMULATE(errval,rsb__do_matrix_add_to_dense(alpha,mtxAp,ldd,dim,dim,rowmajor,dVA));
				RSB_DO_ERROR_CUMULATE(errval,rsb__util_vector_sum(sum2,dVA,typecode,ldd*dim));
				rsb__util_vector_div(sum2,two,typecode,1);
				// TODO: there is risk of overflow, though..
				if( RSB_SOME_ERROR(rsb__do_are_same(sum1,sum2,1,typecode,1,1) ))
				{
					RSB_CONDITIONAL_FREE(dVA);
					RSB_ERROR(RSB_ERRM_FMATDBC);
		       			errval = RSB_ERR_INTERNAL_ERROR; goto err;
			       	}
				RSB_CONDITIONAL_FREE(dVA);
				if(RSB_SOME_ERROR(errval))
				{
					RSB_ERROR(RSB_ERRM_FMATD);
		       			errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}
			}
			//if((cmatrix = rsb__do_matrix_mul(RSB_TRANSPOSITION_N,NULL,mtxAp,trans,NULL,mtxAp,&errval))!=RSB_ERR_NO_ERROR)
			if((cmatrix = rsb__do_matrix_mul(typecode,RSB_TRANSPOSITION_N,NULL,mtxAp,trans,NULL,mtxAp,&errval))!=NULL)
			{
				// FIXME: ignoring scaling values!
				RSB_MTX_FREE(cmatrix);
			}
			else
			{
				RSB_ERROR(RSB_ERRM_FMM);
		       		errval = RSB_ERR_INTERNAL_ERROR; goto err;
			}
		}
#endif
#if 1
#if RSB_ALLOW_INTERNAL_GETENVS
		if(! ( getenv("RSB_BMT_SUM") && rsb__util_atoi(getenv("RSB_BMT_SUM")) == 0 ) )
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
	{
		if(diaga[diagi]!=blas_unit_diag && nnz>0)
		{
			cmatrix = NULL;
			errval = rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_N,NULL,mtxAp,RSB_FLAG_IDENTICAL_FLAGS);
			// cmatrix = rsb__clone_simple(mtxAp);
		if( (cmatrix == NULL) || (!rsb__mtx_chk(cmatrix) ) )
		{
			if(cmatrix==NULL)
			{
				RSB_ERROR(RSB_ERRM_FMC);
		       		errval = RSB_ERR_INTERNAL_ERROR; goto err;
			}
			else
			{
				RSB_ERROR(RSB_ERRM_CMINBC);
		       		errval = RSB_ERR_INTERNAL_ERROR; goto err;
			}
		}
		else
		{
			/* very, very slow sparse matrices sum testing */
			struct rsb_mtx_t *smatrix=NULL;
			struct rsb_coo_matrix_t coo,coc;
			// s = m^trans + 2 * m^trans = 3 m^trans
			//smatrix = rsb__do_matrix_sum(typecode,mtxAp,one,trans,cmatrix,two,trans,&errval);
			RSB_BZERO_P(&coo);
		       	RSB_BZERO_P(&coc);

			smatrix = rsb__do_matrix_sum(typecode,RSB_TRANSPOSITION_N,one,mtxAp,RSB_TRANSPOSITION_N,two,cmatrix,&errval);
			if(!smatrix ) {RSB_ERROR(RSB_ERRM_FCMS);errval = RSB_ERR_INTERNAL_ERROR;}
			if( RSB_SOME_ERROR(errval)) { RSB_ERROR(RSB_ERRM_FCMS); goto smerr; }
#if 0
			if( smatrix->nnz > 1)
			{
				rsb_nnz_idx_t i;
				for(i=0;i<cmatrix->nnz;++i)RSB_STDOUT("%d \n",((rsb_half_idx_t*)(mtxAp->bindx))[i]);
				RSB_INFO("cmatrix:\n");
			       	rsb_print_matrix_t(cmatrix);
				RSB_INFO(" mtxAp:\n");
			       	rsb_print_matrix_t( mtxAp);
				RSB_STDOUT_MATRIX_SUMMARY(mtxAp), RSB_INFO("\n");
				RSB_INFO("smatrix:\n");
			       	rsb_print_matrix_t(smatrix);
				RSB_INFO("\n");
			}
#endif
			if( smatrix->nnz != mtxAp->nnz)
#if RSB_ALLOW_EMPTY_MATRICES
			if( !isempty )
#endif /* RSB_ALLOW_EMPTY_MATRICES */
			{
			       	RSB_ERROR("seems like matrix sum failed (same pattern, no cancellation possible): %d + %d to %d nnz)\n",mtxAp->nnz,cmatrix->nnz,smatrix->nnz);
smatrix = rsb__do_matrix_sum(typecode,RSB_TRANSPOSITION_N,one,mtxAp,RSB_TRANSPOSITION_N,two,cmatrix,&errval);
				 errval = RSB_ERR_INTERNAL_ERROR;
				 goto smerr; 
			}
			//if(RSB_SOME_ERROR(errval = rsb_mtx_elemental_scale(cmatrix,&three)))
			if(RSB_SOME_ERROR(errval = rsb__do_upd_vals(cmatrix,RSB_ELOPF_MUL,&three)))
			{ RSB_ERROR(RSB_ERRM_ES); goto smerr; }
			coo.typecode=smatrix->typecode; coo.nnz=smatrix->nnz;
			RSB_DO_FLAG_ADD(smatrix->flags,RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS);
			errval = rsb__do_switch_rsb_mtx_to_coo(smatrix,&coo.VA,&coo.IA,&coo.JA,RSB_FLAG_SORTED_INPUT);
			if(RSB_SOME_ERROR(errval)){ RSB_ERROR(RSB_ERRM_ES); goto smerr; }
			smatrix=NULL;
			coc.typecode=cmatrix->typecode; coc.nnz=cmatrix->nnz;
			RSB_DO_FLAG_ADD(cmatrix->flags,RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS);
			errval = rsb__do_switch_rsb_mtx_to_coo(cmatrix,&coc.VA,&coc.IA,&coc.JA,RSB_FLAG_SORTED_INPUT);
			if(RSB_SOME_ERROR(errval)){ RSB_ERROR(RSB_ERRM_ES); goto smerr; }
			cmatrix=NULL;
			if(RSB_SOME_ERROR(errval)) { RSB_ERROR(RSB_ERRM_ES); goto smerr; }
			//if(trans == RSB_TRANSPOSITION_T)RSB_SWAP(rsb_coo_idx_t*,coo.IA,coo.JA);
			//if(trans == RSB_TRANSPOSITION_C)errval = rsb__util_do_conjugate(coo.VA,coo.typecode,coo.nnz);
			//coo.VA=coc.VA=NULL;
#if RSB_ALLOW_EMPTY_MATRICES
			if((!isempty) || (!rsb__are_coo_matrices_both_empty(&coo,RSB_FLAG_NOFLAGS,&coc,RSB_FLAG_NOFLAGS)))
#endif /* RSB_ALLOW_EMPTY_MATRICES */
			if(!rsb__are_coo_matrices_equal(&coo,&coc))
			{ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR("matrices do not match!\n"); goto smerr; }
			//
smerr:
			rsb__destroy_coo_matrix_t(&coo);
			rsb__destroy_coo_matrix_t(&coc);
			RSB_MTX_FREE(cmatrix);
			RSB_MTX_FREE(smatrix);
			if(RSB_SOME_ERROR(errval)) { RSB_ERROR("some error occurred while testing matrices sum functionality\n"); goto err; }
#if RSB_WANT_AUTOTUNING_TESTING
#if RSB_ALLOW_INTERNAL_GETENVS
		if( getenv("RSB_BMT_AUTOTUNE") )
		       	do_tune_test = rsb__util_atoi(getenv("RSB_BMT_AUTOTUNE"));
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
		if( do_tune_test > 0 )
		if( nnz > 0 && nnz < 100 ) /* FIXME: these limits here are only for time reasons */
		{
			struct rsb_mtx_t * mtxOp = NULL;
#if !RSB_AT_DESTROYS_MTX
			struct rsb_mtx_t * mtxQp = NULL;
#endif /* RSB_AT_DESTROYS_MTX */
			rsb_real_t *sfp = NULL;
		       	rsb_int_t *tnp = NULL;
		       	rsb_int_t oitmax = 0;
		       	rsb_time_t tmax=/*RSB_TIME_ZERO*/0.003;
		       	rsb_trans_t transA = trans;
		       	const void * alphap = NULL;
		       	// struct rsb_mtx_t * mtxAp = ;
		       	rsb_coo_idx_t nrhs = 1;
		       	rsb_flags_t order = RSB_FLAG_WANT_COLUMN_MAJOR_ORDER;
		       	const void * Bp = NULL;
		       	rsb_nnz_idx_t ldB = 0;
		       	const void * betap = NULL;
		       	void * Cp = NULL;
		       	rsb_nnz_idx_t ldC = 0;

		       	// mtxOp = rsb__clone_simple(mtxAp);
			errval = rsb__clone(&mtxOp,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_N,NULL,mtxAp,RSB_FLAG_IDENTICAL_FLAGS);

		       	ldB = rsb_do_get_rows_of(mtxOp,transA);
		       	ldC = rsb_do_get_columns_of(mtxOp,transA);
#if !RSB_AT_DESTROYS_MTX
			mtxQp = mtxOp;
#endif /* RSB_AT_DESTROYS_MTX */
			if( RSB_SOME_ERROR( errval = rsb__do_tune_spmm(&mtxOp, sfp, tnp, oitmax, tmax, transA, alphap, NULL, nrhs, order, Bp, ldB, betap, Cp, ldC)))
			{
				errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR("rsb_tune_spmm failed !\n"); goto sterr;
			}
#if !RSB_AT_DESTROYS_MTX
			if(mtxQp != mtxOp) RSB_MTX_FREE(mtxQp);
#endif /* RSB_AT_DESTROYS_MTX */
#if 0
			RSB_MTX_FREE(mtxOp);
			mtxOp = rsb__clone_simple(mtxAp);
#else
			errval = rsb__clone(&mtxOp,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_N,NULL,mtxAp,RSB_FLAG_IDENTICAL_FLAGS);
#endif
#if !RSB_AT_DESTROYS_MTX
			mtxQp = mtxOp;
#endif /* RSB_AT_DESTROYS_MTX */
#if RSB_ALLOW_EMPTY_MATRICES
			if(!isempty)
#endif
			if(stype==blas_upper_triangular || stype==blas_lower_triangular )
			if( RSB_SOME_ERROR( errval = rsb__do_tune_spsm(&mtxOp, sfp, tnp, oitmax, tmax, transA, alphap, NULL, nrhs, order, Bp, ldB, betap, Cp, ldC)))
			{
				errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR("rsb_tune_spsm failed !\n"); goto sterr;
			}
#if !RSB_AT_DESTROYS_MTX
			if(mtxQp != mtxOp) RSB_MTX_FREE(mtxQp);
#endif /* RSB_AT_DESTROYS_MTX */
			RSB_MTX_FREE(mtxOp);
sterr:
			if(RSB_SOME_ERROR(errval)) { RSB_ERROR("...\n"); goto err; }
		}
#endif	/* RSB_WANT_AUTOTUNING_TESTING */
		}
		}
	}
#endif
#if 1
		cmatrix = NULL;
		errval = rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_N,NULL,mtxAp,RSB_FLAG_IDENTICAL_FLAGS);
		// cmatrix = rsb__clone_simple(mtxAp);

		if( cmatrix == NULL )
		{
			RSB_ERROR("failed matrix cloning\n");
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
		}
		else
		{
			struct rsb_coo_matrix_t coo,csr;
			rsb_flags_t cflags = RSB_DO_FLAG_FILTEROUT(cmatrix->flags,RSB_FLAG_FORTRAN_INDICES_INTERFACE);

			RSB_BZERO_P(&coo);
		       	RSB_BZERO_P(&csr);

			RSB_DO_FLAG_ADD(cflags,RSB_FLAG_SORTED_INPUT); // NEW, TO SPEEDUP THIS CODE (WEAKENS THE TESTING EFFECTIVENESS, THOUGH)
			if(!rsb__mtx_chk(cmatrix))
			{
				RSB_ERROR("cloned matrix is not built correctly\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			if(!cmatrix->nnz)
				goto cmedone;
			RSB_INIT_CXX_FROM_MTX(&coo,cmatrix);
			csr=coo;
			if((rsb__allocate_coo_matrix_t(&coo)!=&coo) || (rsb__allocate_coo_matrix_t(&csr)!=&csr))
			{
				RSB_ERROR("allocaton problem\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			coo.nnz=csr.nnz=mtxAp->nnz;
getcsrcooagain:
			// RSB -> COO
			errval = rsb__do_get_coo_noalloc(mtxAp,coo.VA,coo.IA,coo.JA,NULL,cflags);
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("coo extraction problems\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// RSB -> CSR
			errval = rsb__do_get_csr(typecode,mtxAp,csr.VA,csr.IA,csr.JA,cflags);
			//errval = rsb__do_get_csr(typecode,mtxAp,csr.VA,csr.IA,csr.JA,cflags|RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS);
			//errval = rsb__do_get_csr(typecode,mtxAp,csr.VA,csr.IA,csr.JA,RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS);
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("csr extraction problems\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// CSR -> COO
			errval = rsb__util_uncompress_row_pointers_array(csr.IA,csr.nr,cflags,cflags,csr.IA);
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("coo->csr conversion problems\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// let's check if 'csr' was converted in coo 
			if(!rsb__are_coo_matrices_equal(&coo,&csr))
			{
				RSB_ERROR("no match in coo/csr extractors\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// COO -> CSR 
			errval = rsb__util_compress_to_row_pointers_array(NULL,csr.nnz,csr.nr,cflags,cflags,csr.IA);
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("csr->coo conversion failed!\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			else
			/* FIXME: checks are missing for the following ! */
			if(!RSB_DO_FLAG_HAS(cflags,RSB_FLAG_USE_HALFWORD_INDICES))
			if(RSB_SOME_ERROR(errval = rsb__csr_chk(csr.IA,csr.JA,csr.nr,csr.nc,csr.nnz,0)))
			{
				RSB_ERROR("csr->coo conversion produced corrupt results!\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// RSB -> COO
			// FIXME: 
			// errval = rsb__do_switch_recursive_in_place_matrix_to_in_place_coo_unsorteda.. 
			// if ..
			// rsb__destroy_coo_matrix_t(&icoo);
			RSB_MTX_FREE(cmatrix);
			// CSR -> RSB

			if(incX==1 && incB==1) if(alphai==0 && betai==0) /* agnostic to these parameters */
{
			kmatrix = rsb__do_mtx_alloc_from_csr_const(csr.VA,csr.IA,csr.JA,csr.nnz,csr.typecode,csr.nr,csr.nc,RSB_DEFAULT_ROW_BLOCKING,RSB_DEFAULT_COL_BLOCKING,cflags,&errval);
			if((RSB_SOME_ERROR(errval)) || (!kmatrix) || (!rsb__mtx_chk(kmatrix)))
			{ RSB_ERROR("csr->rsb construction problems\n"); goto err;}
			RSB_MTX_FREE(kmatrix);
}

			cmatrix = rsb__do_mtx_alloc_from_csr_inplace(csr.VA,csr.IA,csr.JA,csr.nnz,csr.typecode,csr.nr,csr.nc,RSB_DEFAULT_ROW_BLOCKING,RSB_DEFAULT_COL_BLOCKING,cflags,&errval);
			if((RSB_SOME_ERROR(errval)) || (!cmatrix) || (!rsb__mtx_chk(cmatrix)))
			{
				if(RSB_SOME_ERROR(errval))
					RSB_ERROR("csr->rsb construction problems\n");
				else
				if(!cmatrix)
				{
		       			errval = RSB_ERR_INTERNAL_ERROR;
					RSB_ERROR("csr->rsb construction problems: did not succeed\n");
				}
				else
				{
					RSB_ERROR("csr->rsb construction problems: built a corrupted matrix\n");
		       			errval = RSB_ERR_INTERNAL_ERROR;
				}
				goto err;
			}
			RSB_DO_FLAG_ADD(cmatrix->flags,RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS);
			// RSB -> COO 
#if 0
			errval = rsb__do_switch_rsb_mtx_to_coo(cmatrix,&csr.VA,&csr.IA,&csr.JA,cflags|RSB_FLAG_SORTED_INPUT);
#else
			// still broken!
			if(nnz<42) /* coverage testing purpose :P */
			{
				errval  = rsb__do_switch_rsb_mtx_to_coo(cmatrix,&csr.VA,&csr.IA,&csr.JA,RSB_DO_FLAG_FILTEROUT(cflags,RSB_FLAG_SORTED_INPUT));
				errval |= rsb_util_sort_row_major_inner(csr.VA,csr.IA,csr.JA,cmatrix->nnz,dim,dim,typecode,RSB_DO_FLAG_FILTEROUT(cflags,RSB_FLAG_SORTED_INPUT));
			}
			else
				errval = rsb__do_switch_rsb_mtx_to_coo(cmatrix,&csr.VA,&csr.IA,&csr.JA,cflags|RSB_FLAG_SORTED_INPUT);
#endif
			if((RSB_SOME_ERROR(errval)) || !rsb__are_coo_matrices_equal(&coo,&csr))

			{
				RSB_ERROR("rsb->coo conversion problems\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// COO -> RSB 
			cmatrix = rsb__do_mtx_alloc_from_coo_inplace(csr.VA,csr.IA,csr.JA,csr.nnz,csr.typecode,csr.nr,csr.nc,RSB_DEFAULT_ROW_BLOCKING,RSB_DEFAULT_COL_BLOCKING,cflags,&errval);
			if((RSB_SOME_ERROR(errval)) || (!cmatrix) || (!rsb__mtx_chk(cmatrix)))
			{
				RSB_ERROR("csr->coo conversion problems\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			// RSB -> CSR 
			RSB_DO_FLAG_ADD(cmatrix->flags,RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS);
			errval = rsb__do_switch_rsb_mtx_to_csr_sorted(cmatrix,&csr.VA,&csr.IA,&csr.JA,cflags);
			cmatrix=NULL;
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("coo->csr conversion problems\n");
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}
			errval = rsb__util_uncompress_row_pointers_array(csr.IA,csr.nr,cflags,cflags,csr.IA);
			if((RSB_SOME_ERROR(errval)) || !rsb__are_coo_matrices_equal(&coo,&csr))
			{
				RSB_ERROR("csr->coo conversion problems\n");
//				rsb__debug_print_index_vectors_diff(coo.IA,csr.IA,csr.nnz,RSB_VECTORS_DIFF_DISPLAY_N_SMALL);
//				rsb__debug_print_index_vectors_diff(coo.JA,csr.JA,csr.nnz,RSB_VECTORS_DIFF_DISPLAY_N_SMALL);
//				rsb__debug_print_vectors_diff(coo.VA,csr.VA,csr.nnz,typecode,1,1,RSB_VECTORS_DIFF_DISPLAY_N_SMALL);
		       		errval = RSB_ERR_INTERNAL_ERROR; goto converr;
			}

			if(!RSB_DO_FLAG_HAS(cflags,RSB_FLAG_FORTRAN_INDICES_INTERFACE))
			{
				RSB_DO_FLAG_ADD(cflags,RSB_FLAG_FORTRAN_INDICES_INTERFACE);
				goto getcsrcooagain;
			}
			else
				RSB_DO_FLAG_DEL(cflags,RSB_FLAG_FORTRAN_INDICES_INTERFACE);
converr:
			rsb__destroy_coo_matrix_t(&coo);
			rsb__destroy_coo_matrix_t(&csr);
cmedone:
			RSB_MTX_FREE(cmatrix);
		}
#endif
#if RSB_ALLOW_INTERNAL_GETENVS
		if(! ( getenv("RSB_BMT_SPSV") && rsb__util_atoi(getenv("RSB_BMT_SPSV")) == 0 ) )
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
		if(!RSB_DO_FLAG_HAS(mtxAp->flags,RSB_FLAG_SYMMETRIC))
		{
		for(mmi=0;mmi< (dim<msmd?3:2) ;++mmi)
		if(! (mmi==1 && ((incX!= 1) || (incB!=1) )  ))
		{
			const rsb_int nrhs=1;

			/* TODO : should fill X and B with sentinel values ! */
			if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,X,incX)) || RSB_SOME_ERROR(rsb__fill_with_ones (B,typecode,dim,incB)))
			{ errval = RSB_ERR_INTERNAL_ERROR; goto err; }

			if(mmi==0)
			if( rsb__BLAS_Xusmv(transT,alpha,T,B,incB,beta,X,incX) != RSB_BLAS_NO_ERROR )
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while performing Unsymmetric USMV\n"); goto err;
			}

			if(mmi==1)
			if(RSB_SOME_ERROR( rsb__do_spmm(trans,alpha,mtxAp,nrhs,RSB_FLAG_WANT_COLUMN_MAJOR_ORDER,B,dim,beta,X,dim,RSB_OP_FLAG_DEFAULT)) )
			{
			       	errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("Unsymmetric USMV failed!\n"); goto err;
		       	}

			if(mmi==2)
			if(RSB_SOME_ERROR( rsb_do_spmv_general(trans,alpha,mtxAp,B,incB,beta,X,incX,RSB_OP_FLAG_WANT_SERIAL RSB_DEFAULT_OUTER_NRHS_SPMV_ARGS)))
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("Unsymmetric USMV failed!\n"); goto err;
			}

		if(isinvertible)
		{
			if(mmi==0)
			if( rsb__BLAS_Xussv(transT,alpha_inv,T,X,incX) != RSB_BLAS_NO_ERROR )
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while performing USSV\n"); goto err;
			}

			if(mmi==1)
			if(RSB_SOME_ERROR( rsb_spsm(trans,alpha_inv,mtxAp,nrhs,RSB_FLAG_WANT_COLUMN_MAJOR_ORDER,alpha_inv,X,dim,X,dim)) )
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while performing USSV\n"); goto err;
			}

			if(mmi==2)
			if( rsb__do_spsv(trans,alpha_inv,mtxAp,X,incX,X,incX) != RSB_BLAS_NO_ERROR )
			{
				errval = RSB_ERR_INTERNAL_ERROR;RSB_ERROR("error while performing USSV\n"); goto err;
			}
		}
		if(!isinvertible)
			rsb__cblas_Xscal(typecode,dim,&zero,B,incB);
		if(stype != blas_general)
		if(stype != blas_lower_hermitian) /* FIXME: complete this case */
		if(stype != blas_upper_hermitian) /* FIXME: complete this case */
		if( RSB_SOME_ERROR(rsb__do_are_same(B,X,dim,typecode,incB,incX)) )
		{
			RSB_ERROR("failed post combined USMV-USSV check!\n");
			rsb__debug_print_vectors_diff(B,X,dim,typecode,incB,incX,RSB_VECTORS_DIFF_DISPLAY_N);
			errval = RSB_ERR_INTERNAL_ERROR;
			goto err;
		}
		}
		}

		if(betai > 0 || alphai > 0)
			goto err; /* only the previous tests were affected by alpha and beta */
		/*
		 TODO: complete the following ...
		 
		if( rsb__BLAS_Xusget_rows_sums(T,rs,transT) != RSB_BLAS_NO_ERROR )
		{
			RSB_ERROR("error getting rows sum!\n");
			errval = RSB_ERR_INTERNAL_ERROR;
			goto err;
		}
		else
		{
		*/

		/* TODO: need parameters scan here: */
		if( RSB_SOME_ERROR(errval = rsb__do_upd_vals(mtxAp,RSB_ELOPF_NEG,NULL))) { RSB_ERROR("Failed negating.\n"); goto smerr; }
		if( RSB_SOME_ERROR(errval = rsb__do_upd_vals(mtxAp,RSB_ELOPF_NEG,NULL))) { RSB_ERROR("Failed negating.\n"); goto smerr; }
		if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,1,&zero,inrm,1))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR("!\n"); goto err; }

		rsb__util_set_area_to_converted_integer(inrm,typecode,0);

		if( rsb__BLAS_Xusget_infinity_norm(T,inrm,transT) != RSB_BLAS_NO_ERROR )
		{
			RSB_ERROR("error getting infinity norm!\n");
			errval = RSB_ERR_INTERNAL_ERROR;
			goto err;
		}
		else
		{
			if(is_really_empty)
			{
				rsb__util_set_area_to_converted_integer(D,typecode,0);
			}
			else
			{
				if(isempty)
					rsb__util_set_area_to_converted_integer(D,typecode,1  );
				else
					rsb__util_set_area_to_converted_integer(D,typecode,dim);
			}
			if( RSB_SOME_ERROR(rsb__do_are_same(inrm,D,1,typecode,1,1)) )
			{
				RSB_ERROR("matrix norm is not what was expected!\n");
				rsb__debug_print_vectors_diff(inrm,D,1,typecode,1,1,RSB_VECTORS_DIFF_DISPLAY_N);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}

			mtxAp = rsb__BLAS_inner_matrix_retrieve(T);
			if(incX==1 && incB==1) if(alphai==0 && betai==0) /* agnostic to these parameters */
			if((!isempty) && !(mtxAp->nnz == 0 && diaga[diagi]==blas_unit_diag))
			{
				/* FIXME: Frobenius norm (RSB_EXTF_NORM_TWO) is untested! */
				rsb__util_set_area_to_converted_integer(D,typecode,1);
				rsb__do_upd_vals(mtxAp,RSB_ELOPF_MUL,D);
				rsb__util_set_area_to_converted_integer(inrm_,typecode,0);
				RSB_LSTPROBE(rsb__BLAS_Xusget_infinity_norm(T,inrm_,transT),"");
				RSB_LSTPROBE(rsb__do_are_same(inrm_,inrm,1,typecode,1,1),"");
				rsb__util_set_area_to_converted_integer(D,typecode,2);

				rsb__do_upd_vals(mtxAp,RSB_ELOPF_MUL,D);
				rsb__util_set_area_to_converted_integer(inrm_,typecode,0);
				RSB_LSTPROBE(rsb__BLAS_Xusget_infinity_norm(T,inrm_,transT),"");
				if(mtxAp->nnz && diaga[diagi]==blas_unit_diag)
					rsb__util_increase_by_one(inrm_,0,mtxAp->typecode);
				rsb__util_vector_div(inrm_,D,typecode,1);
				RSB_LSTPROBE(rsb__do_are_same(inrm_,inrm,1,typecode,1,1),"");

				rsb__do_upd_vals(mtxAp,RSB_ELOPF_DIV,D);
				rsb__util_set_area_to_converted_integer(inrm_,typecode,0);
				RSB_LSTPROBE(rsb__BLAS_Xusget_infinity_norm(T,inrm_,transT),"");
				RSB_LSTPROBE(rsb__do_are_same(inrm_,inrm,1,typecode,1,1),"");
				/* TODO: there are many more subcases for rsb__clone! */
				// if( ((cmatrix = rsb__clone_simple(mtxAp))!=NULL))
				cmatrix = NULL;
				errval = rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_N,NULL,mtxAp,RSB_FLAG_IDENTICAL_FLAGS);
				if( (cmatrix !=NULL))
			       	{
				RSB_LSTPROBE(rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_T,NULL,cmatrix,RSB_FLAG_IDENTICAL_FLAGS),"");
				RSB_LSTPROBE(rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_T,NULL,cmatrix,RSB_FLAG_IDENTICAL_FLAGS),"");
				if(RSB_IS_MATRIX_TYPE_COMPLEX(typecode))/* FIXME: shall fix many vector-operating routines, first */
				{
					/* TODO: more checking */
					RSB_LSTPROBE(rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_C,NULL,cmatrix,RSB_FLAG_IDENTICAL_FLAGS),"");
					RSB_LSTPROBE(rsb__clone(&cmatrix,RSB_NUMERICAL_TYPE_SAME_TYPE,RSB_TRANSPOSITION_C,NULL,cmatrix,RSB_FLAG_IDENTICAL_FLAGS),"");
				}
					RSB_MTX_FREE(cmatrix);
				}

				if(dim>0)
			       	{
					IA[0]=dim-1; JA[0]=dim-1;
					RSB_LSTPROBE(rsb__do_get_elements(mtxAp,VA,IA,JA,1,mtxAp->flags),"");
					if(!(diaga[diagi]==blas_unit_diag))
					RSB_LSTPROBE(rsb__do_set_elements(mtxAp,VA,IA,JA,1,mtxAp->flags),"");
				}
				if(dim>1)
			       	{
					const rsb_int mmudim=100;
					IA[0]=dim-1; JA[0]=0;
					/* TODO: shall check value! */
					if(stype==blas_upper_triangular)
					{
						RSB_LSTPROBE(rsb__do_get_elements(mtxAp,VA,JA,IA,1,mtxAp->flags),"");
						if(!(diaga[diagi]==blas_unit_diag))
						RSB_LSTPROBE(rsb__do_set_elements(mtxAp,VA,JA,IA,1,mtxAp->flags),"");
						RSB_LSTPROBI(rsb__do_get_elements/*rsb__do_get_elements*/(mtxAp,VA,IA,JA,1,mtxAp->flags),"");
						RSB_LSTPROBI(rsb__do_set_elements/*rsb__do_set_elements*/(mtxAp,VA,IA,JA,1,mtxAp->flags),"");
					}
					else
					{
						RSB_LSTPROBE(rsb__do_get_elements(mtxAp,VA,IA,JA,1,mtxAp->flags),"");
						if(!(diaga[diagi]==blas_unit_diag))
						RSB_LSTPROBE(rsb__do_set_elements(mtxAp,VA,IA,JA,1,mtxAp->flags),"");
						RSB_LSTPROBI(rsb__do_get_elements/*rsb__do_get_elements*/(mtxAp,VA,JA,IA,1,mtxAp->flags),"");
						RSB_LSTPROBI(rsb__do_set_elements/*rsb__do_set_elements*/(mtxAp,VA,JA,IA,1,mtxAp->flags),"");
					}

					if(dim>1 && mtxAp->nnz>0 )
					if(dim < mmudim )
					{
						struct rsb_mtx_t*LU[]={NULL,NULL};
						RSB_LSTPROBE(rsb__do_get_preconditioner(LU,mtxAp,RSB_PRECF_ILU0,NULL),"");
						RSB_MTX_FREE(LU[0]);
						RSB_MTX_FREE(LU[1]);
					}
#if 0
					if(dim < mmudim && ( cmatrix = rsb__clone_simple(mtxAp)) !=NULL)
					{
					if(stype==blas_upper_triangular)
					{
				RSB_LSTPROBE(rsb_mtx_set_values_pattern_changing(&cmatrix,VA,IA,JA,1,mtxAp->flags),"");
					}
					else
					{
				RSB_LSTPROBE(rsb_mtx_set_values_pattern_changing(&cmatrix,VA,JA,IA,1,mtxAp->flags),"");
					}
					RSB_MTX_FREE(cmatrix);
					}
#endif
				}

			}
		}

		RSB_LSTPROBE(rsb__do_elemental_binop(mtxAp, RSB_ELOPF_POW, &three),""); /* FIXME: shall test systematically all the others as well !*/

#if RSB_ALLOW_INTERNAL_GETENVS
		if(! ( getenv("RSB_BMT_GET") && rsb__util_atoi(getenv("RSB_BMT_GET")) == 0 ) )
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
		if(1)
		{
			rsb_coo_idx_t m=dim,k=dim;
			rsb_coo_idx_t rc=m/3,fr = rc,lr = RSB_MIN(m-1,2*rc),ri,
					cc=k/3,fc=cc,lc = RSB_MIN(k-1,2*cc),ci;
			rsb_nnz_idx_t bnnz=0,cnnz=0,off=0;
			const void*vp=NULL;
			bnnz = rsb__do_get_block_nnz(mtxAp,fr,lr,fc,lc,RSB_FLAG_C_INDICES_INTERFACE,&errval);
			// FIXME: TODO: should also test rsb__do_get_block_sparse()
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("sparse subblocks nnz count mechanisms seem broken.\n");
				rsb__do_perror(NULL,errval);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}
			for(ri=fr;ri<=lr;++ri)
				for(ci=fc;ci<=lc;++ci)
					cnnz+=(rsb__do_coo_element_inner_address(mtxAp,ri,ci)!=NULL);
			if(bnnz!=cnnz)
			{
				RSB_ERROR("sparse subblocks nnz count mechanisms seem broken (%d vs %d counted in (%d,%d)..(%d,%d)).\n",bnnz,cnnz,fr,fc,lr,lc);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}

			rsb__util_coo_array_set(IA,nnz,RSB_MARKER_COO_VALUE);
			rsb__util_coo_array_set(JA,nnz,RSB_MARKER_COO_VALUE);
			errval = rsb__do_get_block_sparse(mtxAp,VA,IA,JA,fr,lr,fc,lc,NULL,NULL,&cnnz,RSB_FLAG_C_INDICES_INTERFACE);
			if(RSB_SOME_ERROR(errval))
			{
				RSB_ERROR("sparse subblocks nnz get mechanisms seem broken.\n");
				rsb__do_perror(NULL,errval);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}

			if(bnnz!=cnnz)
			{
				RSB_ERROR("sparse subblocks nnz get mechanisms seem broken (%d vs %d counted in (%d,%d)..(%d,%d)).\n",bnnz,cnnz,fr,fc,lr,lc);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}

			for(off=0;off<bnnz;++off)
			if((vp = rsb__do_coo_element_inner_address(mtxAp,IA[off],JA[off]))!=NULL)
			{
				if(RSB_VA_MEMCMP(vp,0,VA,off,mtxAp->el_size))
				{
					RSB_ERROR("address of (%d,%d)@%d extracted from sparse seems not the right one\n",IA[off],JA[off],off);
					errval = RSB_ERR_INTERNAL_ERROR;
					goto err;
				}
			}
			else
			{
					RSB_ERROR("an element (%d,%d)@%d extracted from sparse seems not present\n",IA[off],JA[off],off);
					errval = RSB_ERR_INTERNAL_ERROR;
					goto err;
			}
		}

#if RSB_ALLOW_INTERNAL_GETENVS
		if(! ( getenv("RSB_BMT_SCALE") && rsb__util_atoi(getenv("RSB_BMT_SCALE")) == 0 ) )
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
	{
		if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,D,incD))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR("!\n"); goto err; }
	  	if( rsb__BLAS_Xusget_diag(T,D) != RSB_BLAS_NO_ERROR )
		{
			RSB_ERROR("!\n");
			errval = RSB_ERR_INTERNAL_ERROR;
			goto err;
		}
		else
		{
			if(is_really_empty)
			{if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,B,incB))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }}
			else
			{if(RSB_SOME_ERROR(rsb__fill_with_ones(B,typecode,dim,incB))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }}
			if(RSB_SOME_ERROR(rsb__do_are_same(D,B,dim,typecode,incD,incB)))
			{
				RSB_ERROR("diagonal vector not what expected!\n");
				rsb__debug_print_vectors_diff(D,B,dim,typecode,incD,incB,RSB_VECTORS_DIFF_DISPLAY_N);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}
		}

		incB=1; /* FIXME: now on, no stride */
		incD=1; /* FIXME: now on, no stride */
		if(RSB_SOME_ERROR(rsb__fill_with_increasing_values(B,typecode,dim))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }  /* rsb__fill_with_increasing_values..) -> ,every) */
	  	if(rsb__BLAS_Xusrows_scale(T,B,transT) != RSB_BLAS_NO_ERROR)
		{
			RSB_ERROR("!\n");
			errval = RSB_ERR_INTERNAL_ERROR;
			goto err;
		}

		if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,D,incD))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }
	  	if( rsb__BLAS_Xusget_diag(T,D) != RSB_BLAS_NO_ERROR )
		{
			RSB_ERROR("!\n");
			errval = RSB_ERR_INTERNAL_ERROR;
			goto err;
		}
		else
		if(diaga[diagi]==blas_non_unit_diag && !isempty) // diagonal implicit won't be scaled :)
		{
			rsb_nnz_idx_t n;
			if( RSB_SOME_ERROR(rsb__do_are_same(D,B,dim,typecode,incD,incB)) )
			{
				RSB_ERROR("!\n");
				rsb__debug_print_vectors_diff(D,B,dim,typecode,incD,incB,RSB_VECTORS_DIFF_DISPLAY_N);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}
			if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,B,incB))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }
			for(n=0;n<dim;++n) rsb__BLAS_Xusget_element(T,n,n,((rsb_char_t*)B)+el_size*n*incB);
			if( RSB_SOME_ERROR(rsb__do_are_same(D,B,dim,typecode,incD,incB)) )
			{
				RSB_ERROR("!\n");
				rsb__debug_print_vectors_diff(D,B,dim,typecode,incD,incB,RSB_VECTORS_DIFF_DISPLAY_N);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}
			if(RSB_SOME_ERROR(rsb__fill_with_increasing_values(B,typecode,dim))){errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }
			for(n=0;n<dim;++n) rsb__BLAS_Xusset_element(T,n,n,((rsb_char_t*)B)+el_size*n*incB);
			if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,dim,&zero,D,incD))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }
			for(n=0;n<dim;++n) rsb__BLAS_Xusget_element(T,n,n,((rsb_char_t*)D)+el_size*n*incB);
			if( RSB_SOME_ERROR(rsb__do_are_same(D,B,dim,typecode,incD,incB)) )
			{
				RSB_ERROR("!\n");
				rsb__debug_print_vectors_diff(D,B,dim,typecode,incD,incB,RSB_VECTORS_DIFF_DISPLAY_N);
				errval = RSB_ERR_INTERNAL_ERROR;
				goto err;
			}
		}

	}
		//if((rnnz = rsb__dodo_get_rows_nnz(mtxAp,0,dim-1,RSB_FLAG_C_INDICES_INTERFACE,NULL))!=ndnnz)
		rnnz=10;
		if(!isempty)/* FIXME */
		if((rsb__BLAS_Xusget_rows_nnz(T,0,dim-1,&rnnz)!=RSB_BLAS_NO_ERROR) || (rnnz!=ndnnz))
		{
			RSB_ERROR("Mismatch in the extracted rows nonzeroes counts vs non diagonal nonzero count: %d != %d\n",(int)rnnz,(int)ndnnz);
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
	       	}

		rnnz=0;
		if(!isempty)/* FIXME */
		if(rsb__BLAS_Xusget_matrix_nnz(T,&rnnz)!=RSB_BLAS_NO_ERROR || rnnz!=ndnnz)
		{
			RSB_ERROR("Mismatch in the wffective matrix counts vs input nonzero count: %d != %d\n",(int)rnnz,(int)ndnnz);
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
		}

		if(RSB_SOME_ERROR(rsb__cblas_Xscal(typecode,nnz,&zero,VA,1))){ errval = RSB_ERR_INTERNAL_ERROR; RSB_ERROR(RSB_ERRM_NL); goto err; }
		rsb__util_coo_array_set(IA,nnz,RSB_MARKER_COO_VALUE);
		rsb__util_coo_array_set(JA,nnz,RSB_MARKER_COO_VALUE);
		rnnz=0;
		//if(rsb__do_get_rows_sparse(RSB_TRANSPOSITION_N,NULL,mtxAp,VA,IA,JA,0,dim-1,&rnnz,RSB_FLAG_NOFLAGS|RSB_FLAG_SORT_INPUT))
		if(rsb__BLAS_Xusget_rows_sparse(T,VA,IA,JA,&rnnz,0,dim-1)!=RSB_BLAS_NO_ERROR)
		{
			RSB_ERROR(RSB_ERRM_NL);
		       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
		}
		else
		if(diaga[diagi]==blas_non_unit_diag)
		{
			rsb_nnz_idx_t n;
			for(n=0;n<nnz;++n)
			if(IA[n] != JA[n])
			if(( rsb__do_coo_element_inner_address(mtxAp,IA[n],JA[n] ) == NULL) ||
			 (RSB_SOME_ERROR(rsb__do_are_same(rsb__do_coo_element_inner_address(mtxAp,IA[n],JA[n]),
						((rsb_char_t*)VA)+el_size*n,1,typecode,1,1) ) ))
				{
					RSB_ERROR("@%d (%d,%d) : 0x%x\n",n,IA[n],JA[n],rsb__do_coo_element_inner_address(mtxAp,IA[n],JA[n] ));
				       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}
		}
		if(!RSB_DO_TOOFEWNNZFORCSR(nnz,dim))/* we don't want IA overwrite */
		{
			if(RSB_SOME_ERROR(rsb__do_get_csr(typecode,mtxAp,(void*)(VA),IA,JA,RSB_FLAG_DEFAULT_CSR_MATRIX_FLAGS)))
			{
				RSB_ERROR(RSB_ERRM_NL);
			       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
			}
			else
			if(diaga[diagi]==blas_non_unit_diag)
			{
				// TODO: is_csr_sorted ?
				rsb_nnz_idx_t n;
				rsb_coo_idx_t i;

				for(i=0;i<dim;++i)
				if(!rsb__util_is_nnz_array_sorted_up(JA+IA[i],IA[i+1]-IA[i]))
				{
					RSB_ERROR(RSB_ERRM_NL);
				       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}

				for(i=0;i<dim;++i)
				for(n=IA[i];n<IA[i+1];++n)
				if(JA[n]<0 || JA[n]>=dim)
				{
					RSB_ERROR(RSB_ERRM_NL);
				       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}

				for(i=0;i<dim;++i)
				for(n=IA[i];n<IA[i+1];++n)
				if(i != JA[n])
				if(( rsb__do_coo_element_inner_address(mtxAp,i,JA[n] ) == NULL) ||
				 (RSB_SOME_ERROR(rsb__do_are_same(rsb__do_coo_element_inner_address(mtxAp,i,JA[n]),
						((rsb_char_t*)VA)+el_size*n,1,typecode,1,1) ) ))
				{
					RSB_ERROR("@%d, %d %d : 0x%x\n",n,i,JA[n],rsb__do_coo_element_inner_address(mtxAp,i,JA[n] ));
				       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}
			}
		}
		if(!RSB_DO_TOOFEWNNZFORCSR(nnz,dim))/* we don't want JA overwrite */
		{
			if(RSB_SOME_ERROR(rsb__do_get_csc(mtxAp,(void*)(&VA),&JA,&IA)))
			{
				RSB_ERROR(RSB_ERRM_NL);
			       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
			}
			else
			if(diaga[diagi]==blas_non_unit_diag)
			{
				rsb_nnz_idx_t n;
				rsb_coo_idx_t j;

				if( RSB_SOME_ERROR(rsb__csc_chk(JA,IA,dim,dim,JA[dim],0) ) )
				{
					RSB_ERROR(RSB_ERRM_NL);
			       		errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}

				for(j=0;j<dim;++j)
				for(n=JA[j];n<JA[j+1];++n)
				if(j != IA[n])
				if(( rsb__do_coo_element_inner_address(mtxAp,IA[n],j) == NULL) ||
				 ( RSB_SOME_ERROR(rsb__do_are_same(rsb__do_coo_element_inner_address(mtxAp,IA[n],j),
						((rsb_char_t*)VA)+el_size*n,1,typecode,1,1))) )
				{
					RSB_ERROR("@%d, %d %d : 0x%x\n",n,IA[n],j,rsb__do_coo_element_inner_address(mtxAp,IA[n],j ));
				       	errval = RSB_ERR_INTERNAL_ERROR; goto err;
				}
			}

			if(incX==1 && incB==1) if(alphai==0 && betai==0) /* agnostic to these parameters */
{
			rsb_flags_t cflags = RSB_DO_FLAG_FILTEROUT(mtxAp->flags,RSB_FLAG_FORTRAN_INDICES_INTERFACE);
			kmatrix = rsb__do_mtx_alloc_from_csc_const(VA,IA,JA,/*nnz*/JA[dim],typecode,dim,dim,RSB_DEFAULT_ROW_BLOCKING,RSB_DEFAULT_COL_BLOCKING,cflags,&errval);
			if(RSB_SOME_ERROR(errval) || (!kmatrix) || (!rsb__mtx_chk(kmatrix)))
			{ RSB_ERROR("csc->rsb construction problems\n"); goto err;}
			RSB_MTX_FREE(kmatrix);
}


		}
err:
		if(errval == RSB_ERR_NO_ERROR)
		{
		}
		else
		if(mtxAp && X && B)
		{
			if(mtxAp->nnz<20)
			       	rsb__do_file_mtx_save(mtxAp,NULL),
				RSB_INFO("\n"),
				RSB_INFO("actual results vs correct results:\n"),
				rsb__debug_print_vectors(X,B,dim,incX,incB,typecode);
			else
			if( RSB_SOME_ERROR(rsb__do_are_same(B,X,dim,typecode,incB,incX) ))
			{
				RSB_INFO("actual results vs correct results:\n"),
				rsb__debug_print_vectors_diff(X,B,dim,typecode,incX,incB,RSB_VECTORS_DIFF_DISPLAY_N);
				errval = RSB_ERR_INTERNAL_ERROR;
			}
#if RSB_WANT_VERBOSE_FAILURES
			RSB_INFO("Matrix summary:\n");
		       	RSB_INFO_MATRIX_SUMMARY(mtxAp);
			RSB_INFO("\n");
 			rsb__do_print_matrix_stats(mtxAp,RSB_CONST_DUMP_RECURSION_BRIEF,NULL);
			RSB_INFO("\n");
#endif /* RSB_WANT_VERBOSE_FAILURES */
		}
		if( T != RSB_BLAS_INVALID_MATRIX && rsb__BLAS_Xusds(T) != RSB_BLAS_NO_ERROR )
			errval = RSB_ERR_INTERNAL_ERROR;
		T = RSB_BLAS_INVALID_MATRIX;
		RSB_MTX_FREE(cmatrix);

		RSB_CONDITIONAL_FREE(X);
		RSB_CONDITIONAL_FREE(B);
		RSB_CONDITIONAL_FREE(D);
		RSB_CONDITIONAL_FREE(IA);
		RSB_CONDITIONAL_FREE(JA);
		RSB_CONDITIONAL_FREE(VA);
		if(to.wqt!=RSB_BOOL_TRUE)
		RSB_INFO("%s%7d: type:%c sym:%s incX:%d incB:%d dim:%10d transT:%c alpha:%+2d beta:%+2d diag:%c subms:%5d nz:%d",btps,passed,(char)typecode,RSB_BLAS_MT_STR(stype),incX,incB,dim,tc,alphaa[alphai],betaa[betai],RSB_BLAS_DIAG_CHAR(diaga[diagi]),submatrices,rnz);
		errvalf|=errval;

		if(errval == RSB_ERR_NO_ERROR)
		{
			if(to.wqt!=RSB_BOOL_TRUE)
			RSB_INFO(" is ok\n");
			++passed;
		}
		else
		{
			if(to.wqt!=RSB_BOOL_TRUE)
			RSB_INFO(" is not ok\n");
			++failed;
			RSB_INFO("Terminating testing due to errors.\n");
			goto done;
		}

		if(RSB_SHALL_QUIT)
		{
			RSB_INFO("Terminating testing earlier due to interactive user request: test took %lf s, max allowed was %lf.\n",tt,to.mtt);
			goto done;
		}

#if RSB_TESTER_ALLOW_TIMEOUT
		if(to.mtt != RSB_TIME_ZERO && (tt = rsb_time()-tt0)>to.mtt)
		{
			RSB_INFO("Terminating testing earlier due to user timeout request: test took %lf s, max allowed was %lf.\n",tt,to.mtt);
			goto done;
		}
#endif /* RSB_TESTER_ALLOW_TIMEOUT */
#if RSB_ALLOW_INTERNAL_GETENVS
		if( maxtc != 0 && passed + failed >= maxtc )
		{
			RSB_INFO("Terminating testing earlier due to user limit request to %d tests.\n",maxtc); 
			goto done;
		}
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
		if(to.tur==RSB_BOOL_TRUE && instantiated_some_recursive==1 && failed==0)
		{
			RSB_INFO("ALL TESTS PASSED SO FAR, AND ALSO INSTANTIATED ONE \"RECURSIVE\" MATRIX... THIS IS ENOUGH\n");
			errval = RSB_ERR_NO_ERROR;
			goto done;
		}
	}
done:
	if(to.rrm==RSB_BOOL_TRUE && instantiated_some_recursive==0 && failed==0)
	{
		RSB_INFO("STRANGE: TESTS PASSED, BUT DID NOT INSTANTIATE ANY \"RECURSIVE\" MATRIX... RAISING AN ERROR FOR THIS\n");
		errvalf |= RSB_ERR_INTERNAL_ERROR;
		rsb__do_perror(NULL,RSB_ERR_INTERNAL_ERROR);
	}
	RSB_INFO("	PASSED:%d\n	FAILED:%d\n",passed,failed);
	RSB_INFO("ADVANCED SPARSE BLAS TEST: END (SUCCESS)\n");
//	return RSB_ERR_NO_ERROR;
	errval=errvalf;
ret:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb_blas_runtime_limits_tester(void)
{
	/**
	 \ingroup gr_internals
	 
	 TODO: INCOMPLETE
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_nnz_idx_t maxcoo = RSB_MAX_MATRIX_DIM;
	rsb_coo_idx_t * IA=NULL;
	size_t maxcoo_bytes=0;
	const size_t minmem=1;
	size_t free_mem=0;
	size_t tot_mem=0;
	rsb_nnz_idx_t i;
	rsb_nnz_idx_t fel;
	
	// FIXME: should fix the code revolving around the following:
	//	RSB_STDOUT("%u>=%u : %d\n", rsb__nearest_power_of_two(maxcoo), maxcoo , rsb__nearest_power_of_two(maxcoo)>= maxcoo );
	RSB_INFO("Beginning large binary search test.\n");
	maxcoo_bytes=((size_t)maxcoo)*sizeof(rsb_coo_idx_t);

	free_mem = rsb__sys_free_system_memory();
	tot_mem = rsb__sys_total_system_memory();

	RSB_INFO("Detected %zu bytes of memory, comprehensive of %zu of free memory.\n",tot_mem,free_mem);
	
	if(tot_mem<minmem || free_mem<minmem)
	{
		RSB_INFO("Too little memory detected: seems like your system is not well supported or not standards compliant.\n");
		tot_mem=free_mem=1024*1024*16;
		maxcoo=tot_mem/sizeof(rsb_coo_idx_t);
		RSB_INFO("Will try setting a reasonably small value: %zu for detected free memory.\n",free_mem);
		//goto skip_max_coo_test;
	}
	RSB_INFO("On this system, maximal array of coordinates can have %zu elements and occupy %zu bytes.\n",((size_t)maxcoo),maxcoo_bytes);

	if(tot_mem<maxcoo_bytes || RSB_MUL_OVERFLOW(maxcoo,sizeof(rsb_coo_idx_t),rsb_nnz_idx_t,size_t))
	{
		/* FIXME: overflow cases shall be handled better */
		maxcoo=(3*free_mem/4)/sizeof(rsb_coo_idx_t);
		maxcoo_bytes=sizeof(rsb_coo_idx_t)*maxcoo;
		RSB_INFO("Will perform the test using less memory (%zu MB) than on the maximal coordinate indices array (%zu) allows.\n",maxcoo_bytes/(1024*1024),maxcoo_bytes);
	}

	if(tot_mem<maxcoo_bytes)
	{
		RSB_INFO("Skipping test: too little memory.\n");
		goto skip_max_coo_test;
	}
		
	if(free_mem<maxcoo_bytes)
	{
		RSB_INFO("Detected %zd bytes of free memory, needed %zd\nlet's see if test succeed ..\n",free_mem,maxcoo_bytes);
		//RSB_STDOUT("detected %zd bytes of free memory, needed %zd.\n",free_mem,maxcoo_bytes);
		//RSB_STDOUT("detected %zd bytes of free memory, needed %zd.\n");
		//RSB_STDOUT("detected %zd bytes of free memory, needed %zd. skipping test.\n",free_mem,maxcoo_bytes);
		//goto skip_max_coo_test;
	}
	IA = rsb__calloc(sizeof(rsb_coo_idx_t)*maxcoo);
	if(!IA)
	{
		RSB_INFO("Failed (c)allocating of %zd nnz (%zd bytes)\n",(size_t)maxcoo,maxcoo_bytes);
		if(free_mem>maxcoo_bytes)
		{
			errval = RSB_ERR_ENOMEM;
			goto err;
		}
		else
			goto skip_max_coo_test;
	}
	else
	{
		RSB_INFO("(c)allocated %zd nnz (%zd bytes)\n",(size_t)maxcoo,maxcoo_bytes);
	}

	for(i=0;i<maxcoo;++i)IA[i]=i;

	fel = rsb__seek_coo_idx_t(IA,maxcoo-1,maxcoo);
	if(fel == RSB_MARKER_NNZ_VALUE || IA[fel]!=fel)
	{
		RSB_INFO("Failed retrieving array last element!\n");
		errval = RSB_ERR_INTERNAL_ERROR;
		goto err;
	}
	else
		RSB_INFO("Succeeded retrieving array last element.\n");

	RSB_INFO("Successfully performed large binary search test.\n");
	goto done;
skip_max_coo_test:
	RSB_INFO("Skipping large binary search test.\n");
done:
err:
	rsb__do_perror(NULL,errval);
	RSB_CONDITIONAL_FREE(IA);
	RSB_DO_ERR_RETURN(errval)
}

/* @endcond */
