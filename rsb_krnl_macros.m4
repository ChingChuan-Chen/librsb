dnl
dnl
dnl	@author: Michele Martone
dnl
dnl
include(`rsb_misc.m4')dnl
include(`do_unroll.m4')dnl
include(`rsb_krnl_vb_macros.m4')dnl
include(`rsb_krnl_bcss_macros.m4')dnl
include(`rsb_krnl_bcoo_macros.m4')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_ARGS(mop)
dnl	------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_ARGS',`dnl
pushdef(`mop',$1)dnl
dnl
`('RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(,mop,`function_args')`)'dnl
dnl (const struct rsb_mtx_t * mtxAp, const struct rsb_options_t *o, const void * rhs, void * out)dnl
dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_IDENTIFIER(mop,transposition)
dnl	------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_IDENTIFIER',`dnl
pushdef(`mop',$1)dnl
pushdef(`transposition',`')dnl
dnl
RSB_M4_PREFIX`'do_`'mop`'`'dnl
dnl
popdef(`transposition')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_NAME(mop)
dnl	------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_NAME',`dnl
pushdef(`mop',$1)dnl
pushdef(`transposition',`')dnl
dnl
rsb_err_t RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_IDENTIFIER(mop,transposition)dnl
dnl
popdef(`mop')dnl
popdef(`transposition')dnl
')dnl
dnl
dnl
dnl
dnl
dnl
dnl
dnl	RSB_M4_GET_NEXT_BLOCK_POINTER_MACRO(matrix_storage)
dnl	---------------------------------------------------
dnl
define(`RSB_M4_GET_NEXT_BLOCK_POINTER_MACRO',`dnl
pushdef(`matrix_storage',$1)dnl
dnl
ifelse(matrix_storage,`VBR',`RSB_GET_NEXT_BLOCK_POINTER')`'dnl
ifelse(matrix_storage,`BCSR',`RSB_BCSR_GET_NEXT_BLOCK_POINTER')`'dnl
dnl	else  ? should give error! fixme :)
dnl
popdef(`matrix_storage')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_GET_FIRST_BLOCK_POINTER_MACRO(matrix_storage)
dnl	----------------------------------------------------
dnl
define(`RSB_M4_GET_FIRST_BLOCK_POINTER_MACRO',`dnl
pushdef(`matrix_storage',$1)dnl
dnl
ifelse(matrix_storage,`VBR',`RSB_GET_FIRST_BLOCK_POINTER')`'dnl
ifelse(matrix_storage,`BCSR',`RSB_BCSR_GET_FIRST_BLOCK_POINTER')`'dnl
dnl	else  ? should give error! fixme :)
dnl
popdef(`matrix_storage')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_GOT_LAST_BLOCK_POINTER_MACRO(matrix_storage)
dnl	---------------------------------------------------
dnl
define(`RSB_M4_GOT_LAST_BLOCK_POINTER_MACRO',`dnl
pushdef(`matrix_storage',$1)dnl
dnl
ifelse(matrix_storage,`VBR',`RSB_GOT_LAST_BLOCK_POINTER')`'dnl
ifelse(matrix_storage,`BCSR',`RSB_BCSR_GOT_LAST_BLOCK_POINTER')`'dnl
dnl	else  ? should give error! fixme :)
dnl
popdef(`matrix_storage')dnl
')dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION(types,mop)
dnl	-------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION',`dnl
pushdef(`types',$1)dnl
pushdef(`mop',$2)dnl
pushdef(`unrolling',`u')dnl
dnl pushdef(`transposition',`')dnl
dnl pushdef(`transposition',$3)dnl
dnl
dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_NAME(mop,`')dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_ARGS(mop)dnl
ifdef(`ONLY_WANT_HEADERS',`;
',`
RSB_M4_DEBUGINFO(``$0'')dnl
ifelse(RSB_M4_IS_IMPLEMENTED_MOP(mop),0,`dnl
{
	return RSB_ERR_UNSUPPORTED_OPERATION;
}
',`dnl
{
	/*!
	 * \ingroup rsb_doc_kernels
	 * A run-time kernel dispatching function.
	 * 
	 * Will use the right "mop" kernel for each matrix block.
	 * 
	 * However, there could be some overhead in the process of dispatching
	 * the right function kernel for each block, especially for matrices
	 * partitioned in same-size blocks.
	 * 
	 * In that case, it is better to use some specialized function.
	 *
	 * \return \rsb_errval_inp_param_msg
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
dnl	register rsb_coo_idx_t baserow,basecolumn,rows,columns;
dnl	register rsb_coo_idx_t blockrow,blockcolumn;
dnl	register char *bp=NULL;
	rsb_flags_t symmetry,diagonal;
`#ifdef' RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(`rsb_half_idx_t')
	rsb_int_t half_storage = rsb__do_is_candidate_size_for_halfword(mtxAp->Mdim,mtxAp->mdim,/*nnz*/0,mtxAp->flags)?`'dnl
RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(`rsb_half_idx_t'):`'dnl
RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(`rsb_coo_idx_t');
#else /* RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(`rsb_half_idx_t') */
	rsb_int_t half_storage=`'dnl
RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(`rsb_coo_idx_t');
#endif /* RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(`rsb_half_idx_t') */

	if(!mtxAp /*|| !mtxAp->options */)
		return RSB_ERR_BADARGS;

	symmetry = rsb__get_symmetry_type_flag(mtxAp);
	diagonal = rsb__get_diagonal_type_flag(mtxAp);

	if(RSB_MATRIX_UNSUPPORTED_TYPE(mtxAp->`typecode'))
		return RSB_ERR_BADARGS;

dnl ifelse(mop,`spmv_uxux',`dnl
dnl	if(RSB_IS_ELEMENT_ZERO(betap,mtxAp->`typecode') && RSB_IS_ELEMENT_ONE(alphap,mtxAp->`typecode') )
dnl		return rsb_spmv(...);
dnl')dnl
	switch(diagonal)
	{
foreach(`k_diagonal',RSB_M4_MATRIX_DIAGONAL_TYPES,`dnl
	case(RSB_M4_MATRIX_DIAGONAL_PREPROCESSOR_SYMBOL(k_diagonal)):
	switch(half_storage)
	{
foreach(`citype',RSB_M4_MATRIX_COORDINATE_TYPES,`dnl
	case(RSB_M4_MATRIX_INDEX_COORDINATE_TYPE_PREPROCESSOR_SYMBOL(citype)):
	switch(transA)
	{
foreach(`transposition',RSB_M4_MATRIX_TRANSPOSITIONS,`dnl
	case(RSB_M4_MATRIX_TRANSPOSITION_PREPROCESSOR_SYMBOL(transposition)):
dnl //	switch(mtxAp->`flags' | RSB_M4_MATRIX_SYMMETRY_PREPROCESSOR_SYMBOL(RSB_M4_SYMBOL_SYMMETRIC))
	switch(symmetry)
	{
foreach(`k_symmetry',RSB_M4_MATRIX_SYMMETRY,`dnl
	case(RSB_M4_MATRIX_SYMMETRY_PREPROCESSOR_SYMBOL(k_symmetry)):
	switch(mtxAp->`matrix_storage')
	{
foreach(`matrix_storage',RSB_M4_MATRIX_STORAGE,`dnl
	case(RSB_M4_MATRIX_STORAGE_PREPROCESSOR_SYMBOL(matrix_storage)):
dnl		/* return RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(matrix_storage,unrolling,mop,identifier)(...); */
	switch(mtxAp->`typecode')
	{
foreach(`mtype',types,`dnl
	case(RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)):
dnl
ifelse(RSB_M4_IS_SPSX_KERNEL_MOP(mop),1,`dnl
	if(rsb__is_lower_triangle(mtxAp->flags))
		errval = RSB_M4_KERNEL_SIZE_DISPATCH_FUNCTION_NAME(mtype,matrix_storage,transposition,k_symmetry,unrolling,mop,citype,k_diagonal,`l')`'dnl
(RSB_M4_ACTUAL_ARGS_APPLY_MEMBERSHIP(RSB_M4_ARGS_TO_ACTUAL_ARGS(RSB_M4_BCXX_KERNEL_SIZE_DISPATCH_FUNCTION(`ARGS',mtype,matrix_storage,transposition,k_symmetry,unrolling,,,mop,citype,k_diagonal,`l'))));
	else
		errval = RSB_M4_KERNEL_SIZE_DISPATCH_FUNCTION_NAME(mtype,matrix_storage,transposition,k_symmetry,unrolling,mop,citype,k_diagonal,`u')`'dnl
(RSB_M4_ACTUAL_ARGS_APPLY_MEMBERSHIP(RSB_M4_ARGS_TO_ACTUAL_ARGS(RSB_M4_BCXX_KERNEL_SIZE_DISPATCH_FUNCTION(`ARGS',mtype,matrix_storage,transposition,k_symmetry,unrolling,,,mop,citype,k_diagonal,`u'))));
dnl
',`dnl
dnl
dnl		/* FIXME: the following line could cause severe compiler warnings (e.g.: 1506-280 (W) Function argument assignment between types "const unsigned short* restrict" and "int*" is not allowed) */
dnl
		errval = RSB_M4_KERNEL_SIZE_DISPATCH_FUNCTION_NAME(mtype,matrix_storage,transposition,k_symmetry,unrolling,mop,citype,k_diagonal,`g')`'dnl
(RSB_M4_ACTUAL_ARGS_APPLY_MEMBERSHIP(RSB_M4_ARGS_TO_ACTUAL_ARGS(RSB_M4_BCXX_KERNEL_SIZE_DISPATCH_FUNCTION(`ARGS',mtype,matrix_storage,transposition,k_symmetry,unrolling,,,mop,citype,k_diagonal,`g'))));
')dnl
dnl
popdef(`specific_args')dnl
	break;
	')dnl
		default:
		RSB_ERROR("Sorry, data type \"%c\" currently not supported.\n",mtxAp->typecode);
		errval = RSB_ERR_UNSUPPORTED_TYPE	;
		}
	break;
	')dnl
		default:
		{
		RSB_ERROR("Sorry, matrix storage \"%c\" currently not supported.\n",mtxAp->`matrix_storage');
dnl		FIXME : SOMEWHERE SOMEONE FORGETS TO POPDEF(`matrix_storage') ...
		errval = RSB_ERR_UNSUPPORTED_FORMAT;
		}
	}
	break;
	')dnl
		default:
		{
			RSB_ERROR("Sorry, this symmetry case (0x%xd) is not supported.\n",(rsb_int)symmetry);
			errval = RSB_ERR_UNSUPPORTED_TYPE	;
		}
	}
	break;
	')dnl
		default:
		{
			RSB_ERROR("Sorry, this transposition case (0x%xd) is not supported.\n",(rsb_int)transA);
			errval = RSB_ERR_UNSUPPORTED_TYPE	;
		}
	}
	break;
	')dnl
		default:
		{
			RSB_ERROR("Sorry, this coordinate index (0x%xd) is not supported.\n",(rsb_int)half_storage);
			errval = RSB_ERR_UNSUPPORTED_FEATURE;
		}
	}
	break;
	')dnl
		default:
		{
			RSB_ERROR("Sorry, this diagonal type (0x%xd) is not supported.\n",(rsb_int)diagonal);
			errval = RSB_ERR_UNSUPPORTED_FEATURE;
		}
	}
	return errval;
dnl	return RSB_ERR_INTERNAL_ERROR;	
}
')dnl
')dnl
dnl
popdef(`unrolling')dnl
popdef(`types')dnl
popdef(`mop')dnl
dnl popdef(`transposition')dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ACTUAL_ARGS(mop)
dnl	--------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ACTUAL_ARGS',`dnl
pushdef(`mop',$1)dnl
dnl
RSB_M4_ARGS_TO_ACTUAL_ARGS((RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(,mop,`function_args')))dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ARGS(mop)
dnl	-------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ARGS',`dnl
pushdef(`mop',$1)dnl
dnl
`('double * elapsed_time, RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(,mop,`function_args')`)'dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_IDENTIFIER(mop)
dnl	-------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_IDENTIFIER',`dnl
pushdef(`mop',$1)dnl
dnl
rsb_do_time_`'mop`'dnl
dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_NAME(mop)
dnl	-------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_NAME',`dnl
pushdef(`mop',$1)dnl
dnl
rsb_err_t RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_IDENTIFIER($1)`'dnl
dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION(types,mop)
dnl	--------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION',`dnl
pushdef(`types',$1)dnl
pushdef(`mop',$2)dnl
pushdef(`transposition',RSB_M4_TRANS_N)dnl FIXNE
dnl
dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_NAME(mop)dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ARGS(mop)dnl
ifdef(`ONLY_WANT_HEADERS',`;
',`
RSB_M4_DEBUGINFO(``$0'')dnl
{
	/*!
	 * \ingroup gr_bench
	 * This wrapper function will perform the "mop" operation, 
	 * measuring the time elapsed in seconds and writing it in a
	 * user set variable.
         * 
	 * Note that this dispatch function is matrix type indipendent.
         *
	 * \return \rsb_errval_inp_param_msg
	 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	if( ! elapsed_time ) return RSB_ERR_BADARGS;

	*elapsed_time = - rsb_time();
	errval = RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_IDENTIFIER(mop,transposition)dnl
	(RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ACTUAL_ARGS(mop));
	
	*elapsed_time += rsb_time(); dnl 	FIXME!

	return errval;
}
')dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ACTUAL_ARGS(mop)
dnl	-----------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ACTUAL_ARGS',`dnl
pushdef(`mop',$1)dnl
dnl
RSB_M4_ARGS_TO_ACTUAL_ARGS((RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(,mop,`function_args')))dnl
dnl
popdef(`mop')dnl
popdef(`transposition')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ARGS(mop,mtype)
dnl	----------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ARGS',`dnl
pushdef(`mop',$1)dnl
pushdef(`mtype',$2)dnl
dnl
double * total_elapsed_time, double * m_flops, RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(,mop,`function_args')`'dnl
dnl
popdef(`mtype')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_IDENTIFIER(mop,mtype)
dnl	----------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_IDENTIFIER',`dnl
pushdef(`mop',$1)dnl
pushdef(`mtype',$2)dnl
dnl
RSB_M4_PREFIX`'do_benchmark_`'RSB_M4_CHOPSPACES(mtype)`_'mop`'dnl
dnl
dnl
popdef(`mtype')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_NAME(mop,mtype)
dnl	----------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_NAME',`dnl
pushdef(`mop',$1)dnl
pushdef(`mtype',$2)dnl
dnl
rsb_err_t RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_IDENTIFIER(mop,mtype)`'dnl
dnl
dnl
popdef(`mtype')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION(types,mop)
dnl	-----------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION',`dnl
pushdef(`types',$1)dnl
pushdef(`mop',$2)dnl
dnl
dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_NAME(mop,mtype)dnl
(RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ARGS(mop,mtype))dnl
ifdef(`ONLY_WANT_HEADERS',`;
',`
{
	/*!
	 * \ingroup gr_bench
	 * This wrapper function will benchmark the "mop" operation
	 * a number of times, measuring the elapsed time in seconds
	 * and writing it in a user set location for a specified matrix.
	 *
	 * It will also add  the performed millions of floating point
	 * operation count in another user specified location.
	 *
	 * \param total_elapsed_time if > 0 on input, will benchmark at least total_elapsed_time seconds
	 * \param m_flops if m_flops > 0 on input, will benchmark at least m_flops times
	 *
	 * If neither of the two input arguments will be set on input,
	 * the benchmark will cease after RSB_BENCHMARK_MIN_RUNS runs or RSB_BENCHMARK_MIN_SECONDS seconds.
	 *
	 * Assuming time_limit = *total_elapsed_time :
	 *
	 * if(time_limit <= 0) will benchmark at least min_runs times
	 * if(time_limit >  0) will benchmark at least min_runs times and for time_limit seconds
	 *
	 * \return \rsb_errval_inp_param_msg
         *
	 */

	double time_limit;
	double elapsed_time;
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	int runs=0,min_runs=0;

        if( ! total_elapsed_time || ! m_flops)
		return RSB_ERR_BADARGS;

	time_limit = *total_elapsed_time;	/* we read input (FIXME) */
	min_runs   = (int)*m_flops;			/* we read input (FIXME) */

	*total_elapsed_time = RSB_TIME_ZERO;
	*m_flops = RSB_TIME_ZERO;

	if(time_limit <= 0 )
	{
		time_limit = RSB_BENCHMARK_MIN_SECONDS;
	}

	if(min_runs   <= 0 )
	{
		min_runs = RSB_BENCHMARK_MIN_RUNS ;	/* NOTE : this is a completely arbitrary number (FIXME) */
	}

	//RSB_INFO("will perform min  %d runs, for %lg seconds\n",min_runs, time_limit);

	// FIXME : seems like this affects performance ...
	// *total_elapsed_time = - rsb_time();
	*total_elapsed_time =0;

	while( ( time_limit? ( *total_elapsed_time < time_limit):0 ) || ( min_runs ? ( runs < min_runs ) : 0 ) )
	{
		//elapsed_time = RSB_TIME_ZERO;
		//errval = dnl
		/* FIXME : use an even more general function here (the following is vbr-only!) */
RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_IDENTIFIER(mop)dnl
`'(&elapsed_time,RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ACTUAL_ARGS(mop));
dnl		errval = RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_IDENTIFIER(mop)dnl
dnl (RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ACTUAL_ARGS(mop));

		//*total_elapsed_time += rsb_time();
/*		RSB_INFO("tl : %lg\n",time_limit );*/
/*		RSB_INFO("ss : %lg\n",*total_elapsed_time );*/
/*		RSB_INFO("sse : %lg\n",elapsed_time );*/

		*total_elapsed_time  +=  elapsed_time;
		*m_flops += RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_IDENTIFIER(mop)(mtxAp);
		if(RSB_SOME_ERROR(errval)) return errval;
		++runs;
	}
	/* FIXME : get rid of this line */
	{rsb_char_t buf[RSB_MAX_LINE_LENGTH];
	RSB_STDERR("%s : ",rsb__sprint_matrix_implementation_code(mtxAp,"mop",RSB_FLAG_NOFLAGS,buf));}
	RSB_STDERR("performed %d runs, %lg/%lg seconds (mop,mtype) \n",runs, *total_elapsed_time,time_limit);

	/*
         * FIXME : this is a candidate location for a conditional performance data printout
         */

	return RSB_ERR_NO_ERROR;
}
')dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_FULLRANGEBENCHMARK_FUNCTION_ACTUAL_ARGS(mop,mtype)
dnl	--------------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_FULLRANGEBENCHMARK_FUNCTION_ACTUAL_ARGS',`dnl
pushdef(`mop',$1)dnl
pushdef(`mtype',$2)dnl
dnl
dnl RSB_M4_ARGS_TO_ACTUAL_ARGS((RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_ARGS(mop)))`'dnl
dnl
popdef(`mtype')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_FULLRANGEBENCHMARK_FUNCTION(mop,mtype)
dnl	--------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_FULLRANGEBENCHMARK_FUNCTION',`dnl
dnl
pushdef(`mop',$1)dnl
pushdef(`mtype',$2)dnl
pushdef(`want_what',$3)dnl
pushdef(`args',`$1,$2')dnl
dnl
ifelse(want_what,`function_identifier',`dnl
RSB_M4_PREFIX`do_fullrangebenchmark_'RSB_M4_CHOPSPACES(mtype)`_'mop`'dnl
')dnl
ifelse(want_what,`function_declaration',`dnl
rsb_err_t $0(args,`function_identifier')dnl
($0(args,`function_args'));
')dnl
ifelse(want_what,`function_args',`dnl
void *VA, rsb_coo_idx_t * IA, rsb_coo_idx_t * JA, rsb_nnz_idx_t nnz, rsb_coo_idx_t rows, rsb_coo_idx_t cols, struct rsb_mop_performance_info_t * mpi, rsb_flags_t flags`'dnl
')dnl
ifelse(want_what,`function_definition',`dnl
rsb_err_t $0(args,`function_identifier')dnl
($0(args,`function_args'))
RSB_M4_DEBUGINFO(``$0'')dnl
{
	/*!
	 * \ingroup gr_bench
	 * Will benchmark the "mtype" type implementation of operation "mop" 
	 * for a single matrix, but for the whole range of different block sizes
	 * partitionings.
         * 
         * Therefore, the VBR features of this library will be NOT used here.
	 *
	 * The performance information will be written in a user supplied structure.
         *
	 * \return \rsb_errval_inp_param_msg
	 */
	rsb_flags_t typecode = RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype);
	int ri=0,ci=0;
	rsb_blk_idx_t br=0,bc=0;
	//rsb_blk_idx_t M_b,K_b;
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	struct rsb_mtx_t * mtxAp = NULL;
	rsb_trans_t transA = RSB_DEFAULT_TRANSPOSITION;
ifelse(RSB_M4_IS_SPXX_TWO_VECTORS_OPERATING_KERNEL_MOP(mop),1,`dnl
	mtype *out=NULL,*rhs=NULL;
')dnl
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
	mtype * row_sums=NULL;
')dnl
	rsb_blk_idx_t rua[]=RSB_ROWS_UNROLL_ARRAY;
	rsb_blk_idx_t cua[]=RSB_COLUMNS_UNROLL_ARRAY;
ifelse(RSB_M4_IS_STRIDED_KERNEL_MOP(mop),1,`dnl
	rsb_coo_idx_t incx=1,incy=1;
',`dnl
dnl
dnl	incx is sometimes needed for scaling a vector, even if the op itself is strided 1
dnl
	rsb_coo_idx_t incx=1,incy=1;
	incx=1,incy=1;	/* just to avoid "unused variable"-like  just to avoid "unused variable"-like warnings warnings */
')dnl

	if(!VA || !IA || !JA || !mpi)
		return RSB_ERR_BADARGS;

	RSB_BZERO_P(mpi);
	mpi->rows = rows;
	mpi->cols=cols;
	mpi->nnz=nnz;

	for(ri=0;ri<RSB_ROWS_UNROLL_ARRAY_LENGTH;++ri)
	{
		for(ci=0;ci<RSB_COLUMNS_UNROLL_ARRAY_LENGTH;++ci)
		{
ifelse(RSB_M4_IS_SPXX_TWO_VECTORS_OPERATING_KERNEL_MOP(mop),1,`dnl
			rsb_coo_idx_t bstride = 0;
			rsb_coo_idx_t cstride = 0;
')dnl
dnl
ifelse(RSB_M4_IS_SPXX_KERNEL_MOP(mop),`1',`dnl
ifelse(RSB_M4_IS_SPXM_KERNEL_MOP(mop),`1',`dnl
			rsb_coo_idx_t nrhs=4;
',`dnl
			rsb_coo_idx_t nrhs=1;
')dnl
')dnl
dnl
ifelse(RSB_M4_IS_SPXX_OP_SCALING_KERNEL_MOP(mop),1,`dnl
			double alpha=1.0;/* FIXME */
			double * alphap = &alpha;
')dnl
dnl
ifelse(RSB_M4_IS_SPXX_SCALING_KERNEL_MOP(mop),1,`dnl
			double beta=1.0;/* FIXME */
			double * betap=&beta ;
')dnl
dnl
ifelse(mop,`scale',`dnl
			mtype * scale_factors = NULL;
')dnl
dnl
			br = rua[ri];
			bc = cua[ci];
dnl			mtxAp = rsb_allocate_bcsr_sparse_matrix(VA, IA, JA, nnz, typecode, rows, cols, br,bc, flags,&errval);
			mtxAp = rsb__do_mtx_alloc_from_coo_const(VA,IA,JA,nnz,typecode,rows,cols,br,bc,flags,&errval);
			if(!mtxAp||RSB_SOME_ERROR(errval)) {goto erri;}

			if( ( flags & RSB_FLAG_AUTO_BLOCKING ) != 0)
			{

				/* no need for further benchmarks (FIXME : a temporary, horrible hack! ) */
				ri=ci=-1;
				for(ri=0;ri<RSB_ROWS_UNROLL_ARRAY_LENGTH;++ri)
					for(ci=0;ci<RSB_COLUMNS_UNROLL_ARRAY_LENGTH;++ci)
						if( rua[ri] == mtxAp->rpntr[1] - mtxAp->rpntr[0] )
							if( cua[ci] == mtxAp->cpntr[1] - mtxAp->cpntr[0] )
								goto ok; /* lol */
				errval = RSB_ERR_INTERNAL_ERROR;
				goto erri;
			}

			ok:
				br = rua[ri];
				bc = cua[ci];
				/* autoblocking found a blocking among the supported ones.
				 * we fill in performance info and quit.
				 */

ifelse(RSB_M4_IS_SPXX_TWO_VECTORS_OPERATING_KERNEL_MOP(mop),1,`dnl
			bstride=cols+bc;
			cstride = rows+br;
			rhs = rsb__malloc(mtxAp->el_size*(bstride)*nrhs);
			out = rsb__malloc(mtxAp->el_size*(cstride)*nrhs);
			if(!out || rsb__fill_with_ones(out,mtxAp->typecode,cstride*nrhs,incy)){errval = RSB_ERR_ENOMEM;goto erri;}
			if(!rhs || rsb__fill_with_ones(rhs,mtxAp->typecode,bstride*nrhs,incx)){errval = RSB_ERR_ENOMEM;goto erri;}
			if(!out || !rhs) {errval = RSB_ERR_ENOMEM;goto erri;}
			if(rsb__fill_with_ones(rhs,mtxAp->typecode,(cols)*nrhs,cols))     {errval = RSB_ERR_ENOMEM;goto erri;}
			/* FIXME : are we sure this is correct ?*/
			if(rsb__cblas_Xscal(mtxAp->typecode,(rows+br)*nrhs,NULL,out,incy)) {errval = RSB_ERR_ENOMEM;goto erri;}
')dnl
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
			row_sums = rsb__malloc(mtxAp->el_size*(rows+br));
			if(!row_sums) {errval = RSB_ERR_ENOMEM;goto erri;}
			if(rsb__fill_with_ones(row_sums,mtxAp->typecode,cols,1))     {errval = RSB_ERR_ENOMEM;goto erri;}
')dnl
ifelse(mop,`scale',`dnl
			scale_factors = rsb__malloc(mtxAp->el_size*(rows+br));
			if(!scale_factors) {errval = RSB_ERR_ENOMEM;goto erri;}
			if(rsb__fill_with_ones(scale_factors,mtxAp->typecode,rows,1))     {errval = RSB_ERR_ENOMEM;goto erri;}
')dnl
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
')dnl
ifelse(mop,`negation',`dnl
			int please_fix_RSB_M4_ARGS_TO_ACTUAL_ARGS=-1;
')dnl
			
			mpi->seconds[ri][ci] = RSB_BENCHMARK_MIN_SECONDS; /* min seconds */
			mpi->m_flops[ri][ci] = (double)RSB_BENCHMARK_MIN_RUNS; /* min runs */

dnl			struct rsb_options_t * o = mtxAp->options;
			RSB_M4_DIRECT_KERNEL_DISPATCH_BENCHMARK_FUNCTION_IDENTIFIER(mop,mtype)dnl
( &(mpi->seconds[ri][ci]), &(mpi->m_flops[ri][ci]), RSB_M4_DIRECT_KERNEL_DISPATCH_TIMING_FUNCTION_ACTUAL_ARGS(mop,mtype));
			mpi->fillin[ri][ci] = rsb__do_get_matrix_fillin(mtxAp);
			mpi->e_mflops[ri][ci] =	mpi->m_flops[ri][ci] / mpi->fillin[ri][ci] ;/* new */
			erri:
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
			RSB_CONDITIONAL_FREE(row_sums);
')dnl
ifelse(RSB_M4_IS_SPXX_TWO_VECTORS_OPERATING_KERNEL_MOP(mop),1,`dnl
			RSB_CONDITIONAL_FREE(out);
			RSB_CONDITIONAL_FREE(rhs);
')dnl
ifelse(mop,`scale',`dnl
			RSB_CONDITIONAL_FREE(scale_factors);
')dnl
			RSB_MTX_FREE(mtxAp);
			if(RSB_SOME_ERROR(errval)){rsb__do_perror(NULL,errval);return errval;}

			if( ( flags & RSB_FLAG_AUTO_BLOCKING ) != 0)
				return errval;/* no need for further benchmarks (FIXME : a temporary hack! ) */
		}
	}
	return errval;
}
')dnl
dnl
popdef(`args')dnl
popdef(`want_what')dnl
popdef(`mtype')dnl
popdef(`mop')dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS(mop,mtype)
dnl	-----------------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS',`dnl
pushdef(`mop',$1)dnl
pushdef(`mtype',$2)dnl
dnl
dnl	FIXME
dnl
popdef(`mtype')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ARGS(mtype)
dnl	------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ARGS',`dnl
pushdef(`mtype',$1)dnl
dnl
(const char * filename, struct rsb_mops_performance_info_t * mspi)dnl
dnl
popdef(`mtype')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_IDENTIFIER(mtype)
dnl	------------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_IDENTIFIER',`dnl
pushdef(`mtype',$1)dnl
dnl
`rsb_do_completetypebenchmark_'RSB_M4_CHOPSPACES(mtype)`'dnl
dnl
popdef(`mtype')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_NAME(mtype)
dnl	------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_NAME',`dnl
pushdef(`mtype',$1)dnl
dnl
rsb_err_t RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_IDENTIFIER(mtype)`'dnl
dnl
popdef(`mtype')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION(mtype)
dnl	-------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION',`dnl
dnl
pushdef(`mtype',$1)dnl
dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`dnl
',`
static RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_NAME(mtype)dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ARGS(mtype)dnl
RSB_M4_DEBUGINFO(``$0'')dnl
{
        /*!
	 * \ingroup gr_bench
	 * Will benchmark all supported matrix operations over the "mtype" type.
	 * over all supported matrix partitionings for a fixed block size.
         *
	 * \return \rsb_errval_inp_param_msg
	 */

	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_coo_idx_t * IA=NULL,*JA=NULL;
	rsb_coo_idx_t rows=0,cols=0;
	rsb_nnz_idx_t nnz=0;
	void *VA=NULL;

	struct rsb_mop_performance_info_t * mpi = &(mspi->pipmo[0]);
	rsb_flags_t typecode = RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype),flags=0;

	RSB_BZERO(mspi,sizeof(mspi)); /* FIXME: may double check this */

	if((rsb__util_mm_load_matrix_f(filename,&IA,&JA,&VA,&rows,&cols,&nnz,typecode,flags,NULL,NULL))!=0)
	{
		RSB_STDERR(RSB_ERRMSG_NOTMTXMKT" : %s ..\n",filename);
		goto err;
	}
	
pushdef(`mopcode',0)dnl
foreach(`mop',RSB_M4_MATRIX_OPS,`dnl
pushdef(`mopcode',incr(mopcode))dnl

	/* we benchmark our mtype library implementation for operation mop */
	errval = dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_FULLRANGEBENCHMARK_FUNCTION(mop,mtype,`function_identifier')dnl
(RSB_M4_ARGS_TO_ACTUAL_ARGS((RSB_M4_DIRECT_KERNEL_DISPATCH_FULLRANGEBENCHMARK_FUNCTION(mop,mtype,`function_args'))));
	++mpi;
	if(RSB_SOME_ERROR(errval))goto err;
')dnl
	mpi-=mopcode;
foreach(`mop',RSB_M4_MATRIX_OPS,`dnl
popdef(`mopcode')dnl
')dnl
popdef(`mopcode')dnl
dnl
	
dnl		performance info dumpout

pushdef(`mopcode',0)dnl
foreach(`mop',RSB_M4_MATRIX_OPS,`dnl
pushdef(`mopcode',incr(mopcode))dnl
	/* FIXME : WE SHOULD DUMP OUT PERFORMANCE INFORMATION HERE ! */
	errval = rsb__dump_performance_info(mpi,"RSB_M4_DUMP_PERFOMANCE_INFO_RECORD_IDENTIFIER(mtype,mop)");
	if(RSB_SOME_ERROR(errval))goto err;
	++mpi;
')dnl
	mpi-=mopcode;
foreach(`mop',RSB_M4_MATRIX_OPS,`dnl
popdef(`mopcode')dnl
')dnl
popdef(`mopcode')dnl
dnl

	err:
	RSB_CONDITIONAL_FREE(IA);
	RSB_CONDITIONAL_FREE(JA);
	RSB_CONDITIONAL_FREE(VA);
	return errval;
}
popdef(`mtype')dnl
')dnl
')dnl
dnl
dnl
dnl
define(`RSB_M4_DUMP_PERFORMANCE_INFO_ARRAY_FUNCTION_IDENTIFIER',`dnl
dnl
RSB_M4_PREFIX`dump_performance_array'dnl
dnl
')dnl
dnl
define(`RSB_M4_DUMP_PERFORMANCE_INFO_ARRAY_FUNCTION_ARGS',`dnl
dnl
`(const char * an, const double*array)'dnl
dnl
')dnl
dnl
dnl
dnl	RSB_M4_DUMP_PERFORMANCE_INFO_ARRAY_FUNCTION
dnl	-------------------------------------------
dnl	FIXME : this should go in some other file
dnl
define(`RSB_M4_DUMP_PERFORMANCE_INFO_ARRAY_FUNCTION',`dnl
dnl
rsb_err_t` 'dnl
RSB_M4_DUMP_PERFORMANCE_INFO_ARRAY_FUNCTION_IDENTIFIER()dnl
RSB_M4_DUMP_PERFORMANCE_INFO_ARRAY_FUNCTION_ARGS()dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`;
',`
RSB_M4_DEBUGINFO(``$0'')dnl
{
	/*!
	 * \ingroup gr_bench
	 * A benchmark info dumping function.
         *
	 * \return \rsb_errval_inp_param_msg
         *
	 * FIXME : UNFINISHED
	 */
#if RSB_ALLOW_STDOUT
	int ri,ci;
	rsb_blk_idx_t rua[]=RSB_ROWS_UNROLL_ARRAY;
	rsb_blk_idx_t cua[]=RSB_COLUMNS_UNROLL_ARRAY;
	if(!array || !an)
		return RSB_ERR_BADARGS;

/*	RSB_STDOUT("const double %s [RSB_ROWS_UNROLL_ARRAY_LENGTH][RSB_COLUMNS_UNROLL_ARRAY_LENGTH] = \n",an);*/
	RSB_STDOUT(".%s = \n",an);
	RSB_STDOUT("{");
	RSB_STDOUT("\t/*");
	for(ci=0;ci<RSB_COLUMNS_UNROLL_ARRAY_LENGTH;++ci) RSB_STDOUT("%d, ",cua[ci]);
	RSB_STDOUT("columns per block */\n");
		
	for(ri=0;ri<RSB_ROWS_UNROLL_ARRAY_LENGTH;++ri)
	{
		RSB_STDOUT("\t{");
		for(ci=0;ci<RSB_COLUMNS_UNROLL_ARRAY_LENGTH;++ci)
		{
			if(ci)RSB_STDOUT(",");
			RSB_STDOUT(" %lg",array[ri*RSB_ROWS_UNROLL_ARRAY_LENGTH+ci]);
		}
		RSB_STDOUT(" }, /* %d rows per block */\n",rua[ri]);
	}
	RSB_STDOUT("},\n");
	return RSB_ERR_NO_ERROR;
#else /* RSB_ALLOW_STDOUT */
	return RSB_ERR_UNSUPPORTED_FEATURE;
#endif /* RSB_ALLOW_STDOUT */
}
dnl
')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DUMP_PERFOMANCE_INFO_RECORD_IDENTIFIER(MTYPE,MOP)
dnl	--------------------------------------------------------
dnl
dnl
define(`RSB_M4_DUMP_PERFOMANCE_INFO_RECORD_IDENTIFIER',`dnl
pushdef(`mtype',$1)dnl
pushdef(`mop',$2)dnl
dnl
`pi_'RSB_M4_CHOPSPACES(mtype)`_'mop`'dnl
dnl
popdef(`mop')dnl
popdef(`mtype')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS(mtype)
dnl	-------------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS',`dnl
pushdef(`mtype',$1)dnl
dnl
RSB_M4_ARGS_TO_ACTUAL_ARGS(RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ARGS(mtype))dnl
dnl
popdef(`mtype')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_ARGS()
dnl	---------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_ARGS',`dnl
dnl
(const int argc, char *const argv[])dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_IDENTIFIER(mop)
dnl	-------------------------------------------------------
dnl
define(`RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_IDENTIFIER',`dnl
pushdef(`mop',$1)dnl
dnl
RSB_M4_PREFIX`estimate_mflops_per_op_'mop`'dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl	RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_ARGS(mop)
dnl	-------------------------------------------------
dnl
define(`RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_ARGS',`dnl
pushdef(`mop',$1)dnl
dnl
`(const struct rsb_mtx_t * mtxAp)'dnl
dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION(mop)
dnl	--------------------------------------------
dnl
define(`RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION',`dnl
pushdef(`mop',$1)dnl
dnl
`double 'dnl
RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_IDENTIFIER(mop)dnl
RSB_M4_ESTIMATE_MFLOPS_PER_MOP_FUNCTION_ARGS(mop)dnl
ifdef(`ONLY_WANT_HEADERS',`;
',`
RSB_M4_DEBUGINFO(``$0'')dnl
{
	/*!
	 * \ingroup gr_internals
	 * A function which returns the approximate count of floating point operations
	 * needed to perform the "mop" matrix operation.
	 * In the case of symmetric/hermitian, the number of operations is multiplied by two.
	 * In the case of a complex type, the number of operations is multiplied by six:
	 * (a +bi)*(c+di) = (ac-bd)+(ad+bc)i
	 * accounts for four real multiplications and two real additions.
	 * FIXME: complexity is NOT taken in consideration for non-SPMV/SPSV operations
	 */

	const double M_  = 1000000.0;
	const double Ec = ((double)mtxAp->element_count); double Me = Ec;
ifelse(RSB_M4_IS_SPXX_KERNEL_MOP(mop),`1',`dnl
	if(RSB_IS_MATRIX_TYPE_COMPLEX(mtxAp->typecode)) { Me=8*Ec; } else { Me=2*Ec; }
')dnl
ifelse(RSB_M4_IS_SPMV_KERNEL_MOP(mop),`1',`dnl
	if(rsb__is_not_unsymmetric(mtxAp)){ Me*=2; }/* slightly optimistic : should subtract the diagonal elements count */
')dnl
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
')dnl
ifelse(mop,`negation',`dnl
')dnl
ifelse(mop,`scale',`dnl
')dnl
	Me /= M_;
	return Me;
}
dnl
')dnl
popdef(`mop')dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_IDENTIFIER()
dnl	---------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_IDENTIFIER',`dnl
dnl
`rsb_do_completebenchmark'dnl
dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_NAME()
dnl	---------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_NAME',`dnl
dnl
rsb_err_t`' RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_IDENTIFIER`'dnl
dnl
dnl
')dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION()
dnl	----------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION',`dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`dnl
',`
RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_NAME`'dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETEBENCHMARK_FUNCTION_ARGS`'dnl
RSB_M4_DEBUGINFO(``$0'')dnl
{
	/*!
	 * \ingroup gr_bench
	 * A complete benchmark program.
	 * Will benchmark all supported matrix operations over all supported types
	 * over all supported matrix partitionings for a fixed block size.
         *
	 * \return \rsb_errval_inp_param_msg
         *
	 * FIXME : UNFINISHED: should process and dump this info in a header file.
	 */
	struct rsb_global_performance_info_t mspis;
	struct rsb_mops_performance_info_t * mspi = &(mspis.gpi[0]);

	rsb_option options[] = {
	    {"matrix-filename",	required_argument, NULL, 0x66},  /* f */
	    {0,0,0,0}
	};
	const char * filename=NULL;
	int c=0;
	int opt_index=0;
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	if(rsb_lib_init(RSB_NULL_INIT_OPTIONS))goto err;

	for (;;)
	{
		c = rsb_getopt_long(argc, argv, "f:" , options, &opt_index);/* Flawfinder: ignore */
		if (c == -1)break;
		switch (c)
		{
			case 0x66:/* f */
			filename = optarg;
			break;
	    	}
	}

foreach(`mtype',types,`dnl

	errval=dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_IDENTIFIER(mtype)dnl
(RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS(mtype));
	if(RSB_SOME_ERROR(errval)) return errval;
	++mspi;
')dnl

	if( rsb_lib_exit(RSB_NULL_EXIT_OPTIONS) )
		return RSB_ERR_INTERNAL_ERROR;
	return RSB_ERR_NO_ERROR;
	err:
	return RSB_ERR_INTERNAL_ERROR;
}
')dnl
')dnl
dnl
dnl
dnl
dnl
dnl
dnl	RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION(types,mop,want_what)
dnl	---------------------------------------------------------------------
dnl
dnl	EDIT THIS MACRO TO SPECIFY ARGS TO NEW KERNELS
dnl
define(`RSB_M4_MULTI_BLOCK_KERNEL_TYPE_DISPATCH_FUNCTION',`dnl
pushdef(`types',$1)dnl
pushdef(`mop',$2)dnl
pushdef(`want_what',$3)dnl
pushdef(`args',`$1,$2')dnl
dnl
dnl
ifelse(want_what,`function_identifier',`dnl
RSB_M4_PREFIX`do_'mop`_with_macros_vbr'dnl
')dnl
dnl
ifelse(want_what,`function_declaration',`dnl
rsb_err_t $0(args,`function_identifier')dnl
($0(args,`function_args'));dnl
')dnl
dnl
ifelse(want_what,`function_args',`dnl
dnl
dnl
ifelse(RSB_M4_IS_READONLY_KERNEL_MOP(mop),1,`dnl
const struct rsb_mtx_t * mtxAp`'dnl
',`dnl
struct rsb_mtx_t * mtxAp`'dnl
')dnl
ifelse(RSB_M4_IS_SPXX_TWO_VECTORS_OPERATING_KERNEL_MOP(mop),1,`dnl
,const void * RSB_M4_RESTRICT rhs, void * RSB_M4_RESTRICT out`'dnl
')dnl
ifelse(RSB_M4_IS_OP_SCALING_KERNEL_MOP(mop),`1',`dnl
,const void * alphap`'dnl
')dnl
ifelse(RSB_M4_IS_SCALING_KERNEL_MOP(mop),`1',`dnl
,const void * betap`'dnl
')dnl
ifelse(RSB_M4_IS_STRIDED_KERNEL_MOP(mop),`1',`dnl
,rsb_coo_idx_t incx, rsb_coo_idx_t incy`'dnl
')dnl
dnl
,const rsb_trans_t transA`'dnl
ifelse(mop,`scale',`dnl
,const void * scale_factors`'dnl
')dnl
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
,void * row_sums`'dnl
')dnl
ifelse(mop,`negation',`dnl
,int please_fix_RSB_M4_ARGS_TO_ACTUAL_ARGS`'dnl
')dnl
dnl
dnl
dnl
')dnl
dnl
ifelse(want_what,`function_definition',`dnl
rsb_err_t $0(args,`function_identifier')dnl
($0(args,`function_args'))
{
RSB_M4_DEBUGINFO(``$0'')dnl
	/*!
	 * \ingroup rsb_doc_kernels
	 * Kernel function dispatching will be performed inline, after type dispatching, in a separate function.
         *
	 * \return \rsb_errval_inp_param_msg
	 */
dnl
dnl	removed old junk in revision 625 ...
dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
}
')dnl body
dnl
popdef(`args')dnl
popdef(`want_what')dnl
popdef(`mop')dnl
popdef(`types')dnl
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_TESTING_FUNCTION(types,mop)
dnl	---------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_TESTING_FUNCTION',`dnl
dnl
pushdef(`types',$1)dnl
pushdef(`mop',$2)dnl
dnl
dnl	
#ifdef RSB_WANT_KERNELS_DEBUG
rsb_err_t RSB_M4_PREFIX`'mop`_testing'dnl
RSB_M4_DIRECT_KERNEL_DISPATCH_FUNCTION_ARGS(mop)dnl
ifdef(`ONLY_WANT_HEADERS',`;
',`
{
RSB_M4_DEBUGINFO(``$0'')dnl
	/*!
	 * \ingroup gr_debug
	 * This is a trivial reference implementation of the "mop" kernel, and 
	 * its numerical results will be used to shed some evidence if bugs 
	 * should be introduced in performance computational kernels.
	 * 
	 * It should be used for debugging or comparing with performance optimized
	 * functions.
         *
	 * \return \rsb_errval_inp_param_msg
	 */
	
	register rsb_coo_idx_t baserow = RSB_INI,basecolumn = RSB_INI,rows = RSB_INI,columns = RSB_INI;
	register rsb_coo_idx_t blockrow = RSB_INI,blockcolumn = RSB_INI;
dnl	register char *bp=0;
	register rsb_byte_t *bp=0;
ifelse(RSB_M4_NOT(RSB_M4_IS_STRIDED_KERNEL_MOP(mop)),1,`dnl
	rsb_coo_idx_t incx=1,incy=1;
	incx=1,incy=1;	/* just to avoid "unused variable"-like  just to avoid "unused variable"-like warnings warnings */
')dnl

	if(!mtxAp /*|| !mtxAp->options*/ )return RSB_ERR_BADARGS;
	{
	RSB_GET_FIRST_BLOCK_POINTER(bp,mtxAp,baserow,basecolumn,rows,columns,blockrow,blockcolumn);
foreach(`mtype',types,`dnl
	if(mtxAp->`typecode' == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype))
	{

ifelse(RSB_M4_IS_SCALING_KERNEL_MOP(mop),1,`dnl
	rsb__cblas_Xscal(RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype), betap, mtxAp->nr, out, 1);/* we scale the destination vector */
')dnl
ifelse(RSB_M4_IS_ZEROING_KERNEL_MOP(mop),1,`dnl
	rsb__cblas_Xscal(RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype),mtxAp->nr,NULL,out,incy);
')dnl
ifelse(RSB_M4_IS_SPXM_KERNEL_MOP(mop),`1',`dnl
dnl	if(mtxAp && mout) rsb__cblas_Xscal(mtxAp->typecode,nrhs*mtxAp->nr,NULL,mout,incy);// NEW
	if(mtxAp && out) rsb__cblas_Xscal(mtxAp->typecode,nrhs*mtxAp->nr,NULL,out,incy);// NEW
')dnl
	
	while(!RSB_GOT_LAST_BLOCK_POINTER(mtxAp))
	{
ifelse(mop,`scale',`dnl
		mtype* a = (mtype*)bp;
		rsb_coo_idx_t i,j;
		for(i=0;i<rows;++i)
			for(j=0;j<columns;++j)
				a[i*columns+j]*=((const mtype*)scale_factors)[i];
')dnl
ifelse(RSB_M4_IS_SPMV_KERNEL_MOP(mop),1,`dnl
		const mtype* a = (const mtype*)bp;
		const mtype* b = ((const mtype*)rhs)+mtxAp->cpntr[blockcolumn];
		mtype* c = ((mtype*)out)+mtxAp->rpntr[blockrow];
		rsb_coo_idx_t i,j;
		c=c;/* here just to prevent from compiler warning */

#if 0
		for(i=0;i<rows;++i)
			for(j=0;j<columns;++j)
				c[i]+=a[i*columns+j]*b[j];
#else
		/*
		 * this code will emulate the same kernel order!
		 * it should generate the same numerical roundoff errors the current kernel would
		 * */
		for(i=0;i<rows;++i)
		{
			mtype rs=0;
			for(j=0;j<columns;++j)
				rs+=a[i*columns+j]*b[j];
ifelse(mop,`spmv_uaua',`dnl
			c[i]+=rs;
');
ifelse(mop,`spmv_unua',`dnl
			c[i]-=rs;
');
		}
#endif /* 0 */

')dnl
ifelse(RSB_M4_IS_SPXM_KERNEL_MOP(mop),`1',`dnl
		const mtype* a = (const mtype*)bp;
dnl		const mtype* b = ((const mtype*)mrhs)+mtxAp->cpntr[blockcolumn];
		const mtype* b = ((const mtype*)rhs)+mtxAp->cpntr[blockcolumn];
		mtype* c = ((mtype*)out)+mtxAp->rpntr[blockrow];
dnl		mtype* c = ((mtype*)mout)+mtxAp->rpntr[blockrow];
		rsb_coo_idx_t i,j,k;
		for(i=0;i<rows;++i)
			for(j=0;j<columns;++j)
				for(k=0;k<nrhs;++k)
					c[k*cstride+i]+=a[i*columns+j]*b[j+k*bstride];

')dnl
ifelse(RSB_M4_MEMBER(mop,`spsv_uxua',`spsv_sxsx'),1,`dnl
/*	FIXME : UNFINISHED */
')dnl
ifelse(RSB_M4_IS_STRIDED_KERNEL_MOP(mop),1,`dnl
	dnl	rsb_coo_idx_t incx=1,incy=1;
')dnl
ifelse(RSB_M4_MEMBER(mop,`spmv_sxsx',`spsv_sa',`spmv_uxux'),1,`dnl
/*	FIXME : UNFINISHED */
')dnl
ifelse(RSB_M4_IS_ACC_WRITING_KERNEL_MOP(mop),`1',`dnl
		const mtype* a = (const mtype*)bp;
		mtype* row_sums_=row_sums;
		rsb_coo_idx_t i,j;
		for(i=0;i<rows;++i)
			for(j=0;j<columns;++j)
ifelse(mop,`infty_norm',`dnl
				row_sums_[mtxAp->rpntr[blockrow]+i]+=RSB_M4_ABS(mtype,a[i*columns+j]);
')dnl
ifelse(mop,`rowssums',`dnl
				row_sums_[mtxAp->rpntr[blockrow]+i]+=a[i*columns+j];
')dnl
')dnl
	
ifelse(mop,`negation',`dnl
		mtype* a = (mtype*)bp;
		rsb_coo_idx_t i,j;
		for(i=0;i<rows;++i)
			for(j=0;j<columns;++j)
				a[i*columns+j]=-a[i*columns+j];
')dnl
		RSB_GET_NEXT_BLOCK_POINTER(bp,mtxAp,baserow,basecolumn,rows,columns,blockrow,blockcolumn);
	}
	}
	else
')dnl
	{
		RSB_ERROR("Sorry, data type \"%c\" currently not supported.\n",mtxAp->typecode);
		return RSB_ERR_UNSUPPORTED_TYPE	;
	}
	}
	return RSB_ERR_NO_ERROR;	
}
dnl
')dnl
dnl
#endif /* RSB_WANT_KERNELS_DEBUG */
dnl
')dnl
dnl
dnl
dnl
dnl	RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS(mtype)
dnl	-------------------------------------------------------------------------------
dnl
define(`RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ACTUAL_ARGS',`dnl
pushdef(`mtype',$1)dnl
dnl
RSB_M4_ARGS_TO_ACTUAL_ARGS(RSB_M4_DIRECT_KERNEL_DISPATCH_COMPLETETYPEBENCHMARK_FUNCTION_ARGS(mtype))dnl
dnl
popdef(`mtype')dnl
')dnl
dnl
dnl
