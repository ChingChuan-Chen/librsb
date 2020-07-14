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
 * @brief Initialization code.
 * @author Michele Martone
 * */
 
#include "rsb_do.h"
#include "rsb_common.h"

#define RSB_WANT_PERFORMANCE_COUNTERS_IN_RSB_INIT defined(RSB_WANT_PERFORMANCE_COUNTERS) && (RSB_WANT_PERFORMANCE_COUNTERS>1)

RSB_INTERNALS_COMMON_HEAD_DECLS

const rsb_char_t * rsb__init_get_mem_hierarchy_info_string(rsb_bool_t verbose)
{
	/*!
	 * \ingroup gr_internals
	 *
	 * Determines the memory hierarchy info string.
	 * First queries the environment first for RSB_USER_SET_MEM_HIERARCHY_INFO.
	 * If such variable exists, it returns it.
	 * If no such variable exists, it returns the corresponding preprocessor symbol, if defined.
	 * Otherwise returns NULL.
	 * */
	rsb_char_t * usmhi = NULL;
#ifdef RSB_HAVE_GETENV
	if(verbose) RSB_INFO("Checking environment RSB_USER_SET_MEM_HIERARCHY_INFO variable.\n");
	if((usmhi = getenv("RSB_USER_SET_MEM_HIERARCHY_INFO"))!=NULL && *usmhi)
		goto done;
#endif /* RSB_HAVE_GETENV */
#ifdef RSB_USER_SET_MEM_HIERARCHY_INFO
	if(verbose) RSB_INFO("Checking hardcoded RSB_USER_SET_MEM_HIERARCHY_INFO symbol\n");
	usmhi = RSB_USER_SET_MEM_HIERARCHY_INFO;
	if( usmhi && *usmhi )
		goto done;
#endif /* RSB_USER_SET_MEM_HIERARCHY_INFO */
#ifdef RSB_DETECTED_MEM_HIERARCHY_INFO
	if(verbose) RSB_INFO("Checking hardcoded RSB_DETECTED_MEM_HIERARCHY_INFO symbol\n");
	usmhi = RSB_DETECTED_MEM_HIERARCHY_INFO;
	if( usmhi && *usmhi )
		goto done;
#endif /* RSB_DETECTED_MEM_HIERARCHY_INFO */
done:
	if(verbose) RSB_INFO("Available memory hierarchy info string: \"%s\"\n",usmhi);
	return usmhi;
}

/* FIXME: move these constants outta here ! */
#define RSB_CONST_KB	(1024)
#define RSB_CONST_MB	(RSB_CONST_KB*1024)
#define RSB_CONST_GB	(RSB_CONST_MB*1024)

static int rsb_do_numerical_sprintf(rsb_char_t *s, long n)
{
	/*!
	 * \ingroup gr_internals
	 * FIXME: temporarily here!
	 * FIXME: only for long
	 * */
	if(!s)
		return 0;
	if((n%RSB_CONST_GB)==0 && n >= RSB_CONST_GB)
		sprintf(s,"%ldG",n/RSB_CONST_GB);
	else
	if((n%RSB_CONST_MB)==0 && n >= RSB_CONST_MB)
		sprintf(s,"%ldM",n/RSB_CONST_MB);
	else
	if((n%RSB_CONST_KB)==0 && n >= RSB_CONST_KB)
		sprintf(s,"%ldK",n/RSB_CONST_KB);
	else
		sprintf(s,"%ld",n);
	return strlen(s);
}

const rsb_char_t * rsb__get_mem_hierarchy_info_string(rsb_char_t *usmhib)
{
	/*!
	 * \ingroup gr_internals
	 * min RSB_MAX_LINE_LENGTH
	 * FIXME: no check
	 * */
	long level = 0;
	usmhib[0] = '\0';
#if 0
#error The memory hierarchy info string should the info in the struct!
	const rsb_char_t * usmhi = rsb__init_get_mem_hierarchy_info_string(RSB_BOOL_FALSE);
	/*  e.g.: #define RSB_USER_SET_MEM_HIERARCHY_INFO "L2:4/64/512K;L1:8/64/32K;" */

	usmhib[0] = '\0';
	if(usmhi)
		return usmhi;
#endif
	/* FIXME: potential overflows here */
	for(level = rsb_global_session_handle.memory_hierarchy_levels;level>0;--level)
	{
#if 0
		sprintf(usmhib+strlen(usmhib),"L%ld:%ld/%ld/%ld;",(rsb_long_t)level,
				(rsb_long_t)rsb_global_session_handle.caches[level].associativity,
				(rsb_long_t)rsb_global_session_handle.caches[level].linesize,
				(rsb_long_t)rsb_global_session_handle.caches[level].size
				);
#else
		/* FIXME: TODO : this code was written in a hurry: it should be made less stoopidly inefficient!  */
		sprintf(usmhib+strlen(usmhib),"L%ld:",(rsb_long_t)level),
		rsb_do_numerical_sprintf(usmhib+strlen(usmhib),(rsb_long_t)rsb_global_session_handle.caches[level].associativity),
		sprintf(usmhib+strlen(usmhib),"/"),
		rsb_do_numerical_sprintf(usmhib+strlen(usmhib),(rsb_long_t)rsb_global_session_handle.caches[level].linesize),
		sprintf(usmhib+strlen(usmhib),"/"),
		rsb_do_numerical_sprintf(usmhib+strlen(usmhib),(rsb_long_t)rsb_global_session_handle.caches[level].size);
		if(level>1)
			sprintf(usmhib+strlen(usmhib),",");
	}
#endif
	return usmhib;
}

rsb_err_t rsb__dump_mem_hierarchy_info(void)
{
	/*!
	 * \ingroup gr_internals
	 * */
#if RSB_ALLOW_STDOUT
	const rsb_char_t * usmhi = rsb__init_get_mem_hierarchy_info_string(RSB_BOOL_FALSE);
	if(usmhi && *usmhi)
		RSB_STDOUT("%s",usmhi);
	return RSB_ERR_NO_ERROR;
#else /* RSB_ALLOW_STDOUT */
	return RSB_ERR_UNSUPPORTED_FEATURE;
#endif /* RSB_ALLOW_STDOUT */
}

rsb_err_t rsb__init_mem_hierarchy_info(void)
{
	/*!
	 * \ingroup gr_internals
	*/
	return rsb__set_mem_hierarchy_info(NULL);
}

rsb_err_t rsb__set_mem_hierarchy_info(const rsb_char_t * usmhi)
{
	/*!
	 * \ingroup gr_internals
	 *
	 * TODO: needs some testing code.
	 * Calling this code should be possible also after initialization.
	 * */
	const rsb_char_t * mhi = usmhi?usmhi:rsb__init_get_mem_hierarchy_info_string(RSB_BOOL_FALSE);
	const rsb_char_t * s = mhi;
	struct rsb_memory_level_t caches[RSB_MAX_SUPPORTED_CACHE_LEVELS];	/* */
	long memory_hierarchy_levels = 0;		/*  */
	
	if(!mhi || !*mhi)
		return RSB_ERR_NO_ERROR;	/* keep defaults */

	if(sizeof(rsb_global_session_handle.caches)!=sizeof(caches))
	{
		return RSB_ERR_INTERNAL_ERROR;
	}

	rsb_memcpy(caches,rsb_global_session_handle.caches,sizeof(caches));

	//RSB_INFO("rsb__init_mem_hierarchy_info:\"%s\"\n",usmhi);
	       /*  e.g.:"L2:4/64/512K,L1:8/64/32K" */
		memory_hierarchy_levels = 0;
		while(*s)
		{
			long level = 0;
			if(*s=='L' && s[1] && isdigit(s[1]))
			{
//				RSB_INFO("uhm: %s",mhi);
				level = rsb__util_atoi(s+1);
				memory_hierarchy_levels = RSB_MAX(level,memory_hierarchy_levels);
				caches[level].level = level;
				++s;
				while(isdigit(*s))++s;
				if(*s!=':')goto cerr;
				++s;
				if(!isdigit(*s))goto cerr;
				caches[level].associativity = rsb__util_atoi(s);
				while(isdigit(*s))++s;
				if(toupper(*s)=='K')
					caches[level].associativity *= 1024,++s;
				if(toupper(*s)=='M')
					caches[level].associativity *= 1024*1024,++s;
				if(*s!='/')goto cerr;
				++s;
				if(!isdigit(*s))goto cerr;
				caches[level].linesize = rsb__util_atoi(s);
				while(isdigit(*s))++s;
				if(toupper(*s)=='K')
					caches[level].linesize *= 1024,++s;
				if(toupper(*s)=='M')
					caches[level].linesize *= 1024*1024,++s;
				if(*s!='/')goto cerr;
				++s;
				if(!isdigit(*s))goto cerr;
				caches[level].size = rsb__util_atoi(s);
				while(isdigit(*s))++s;
				if(toupper(*s)=='K')
					caches[level].size *= 1024,++s;
				if(toupper(*s)=='M')
					caches[level].size *= 1024*1024,++s;
				if(toupper(*s)=='G')
					caches[level].size *= 1024*1024*1024,++s;
				if(level>1)
				{
					if(*s!=',')
						goto cerr;
					else
						++s;
				}
			}
			else break;
		}
/*  				RSB_INFO("parsing memory hierarchy string succeeded\n"
						"%d %d %d\n",
					rsb_global_session_handle.memory_hierarchy_levels,
					rsb_global_session_handle.caches[1].size,
					rsb_global_session_handle.caches[2].size
						);*/
		goto cok;
		/* FIXME: this code is not complete: it does not check for complete nor correct information */
cerr:
	RSB_ERROR("error parsing memory hierarchy string (at:%s)\n",s);
	return RSB_ERR_NO_ERROR; /* FIXME */
cok:
	rsb_memcpy(rsb_global_session_handle.caches,caches,sizeof(caches));
	rsb_global_session_handle.memory_hierarchy_levels = memory_hierarchy_levels;
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__init_check_for_constants_correctness(void)
{
	/*!
	 * \ingroup gr_internals
	 *
	 * TODO: needs some testing code.
	 * */
	/* basic compatibility checks (there are programs relying on this, and so this test should reveal inconsistencies) */
#ifdef  RSB_NUMERICAL_TYPE_DOUBLE
	RSB_ASSERT(RSB_NUMERICAL_TYPE_DOUBLE        =='D');
#endif /* RSB_NUMERICAL_TYPE_DOUBLE */
#ifdef  RSB_NUMERICAL_TYPE_FLOAT
	RSB_ASSERT(RSB_NUMERICAL_TYPE_FLOAT         =='S');
#endif /* RSB_NUMERICAL_TYPE_FLOAT */
#ifdef  RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX
	RSB_ASSERT(RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX=='Z');
#endif /* RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX */
#ifdef  RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 
	RSB_ASSERT(RSB_NUMERICAL_TYPE_FLOAT_COMPLEX =='C');
#endif /* RSB_NUMERICAL_TYPE_FLOAT_COMPLEX */

	/* basic sanity checks */
	RSB_ASSERT(RSB_FITTING_SAMPLES>0);
	RSB_ASSERT(RSB_FIRST_FITTING_SAMPLE_BW_MAX>0);
	RSB_ASSERT(RSB_FIRST_FITTING_SAMPLE_BW_MIN>0);
	RSB_ASSERT(RSB_FIRST_FITTING_SAMPLE_BW_MIN <= RSB_FIRST_FITTING_SAMPLE_BW_MAX);
	RSB_ASSERT(RSB_NNZ_BLK_MAX>RSB_MAXIMAL_CONFIGURED_BLOCK_SIZE);
	RSB_ASSERT(RSB_BENCHMARK_MIN_SECONDS>0.0);
	RSB_ASSERT(RSB_BENCHMARK_MIN_RUNS>1);

	/* TODO : should check for signedness, too */
	RSB_ASSERT(sizeof(rsb_non_overflowing_t) >= sizeof(rsb_nnz_idx_t));	/* */
	RSB_ASSERT(sizeof(rsb_nnz_idx_t) >= sizeof(rsb_coo_idx_t));
	RSB_ASSERT(sizeof(rsb_coo_idx_t) >= sizeof(rsb_blk_idx_t));
	RSB_ASSERT(sizeof(rsb_flags_t) >= 4 );
	RSB_ASSERT(sizeof(rsb_err_t  ) >= 4 );
	RSB_ASSERT(sizeof(rsb_int_t) == sizeof(int) );

	RSB_ASSERT((rsb_half_idx_t)((RSB_MAX_VALUE_FOR_TYPE(rsb_half_idx_t)+1))==0);
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_nnz_idx_t)>0);
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t)>0);
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_blk_idx_t)>0);

	//RSB_ASSERT(RSB_IS_SIGNED(char));
	RSB_ASSERT(RSB_IS_SIGNED(signed char));
	RSB_ASSERT(RSB_IS_SIGNED(short int));
	RSB_ASSERT(RSB_IS_SIGNED(signed int));
	RSB_ASSERT(RSB_IS_SIGNED(int));
	RSB_ASSERT(RSB_IS_SIGNED(long));

	RSB_ASSERT(RSB_IS_UNSIGNED(unsigned int));
	RSB_ASSERT(RSB_IS_UNSIGNED(unsigned short int));
	RSB_ASSERT(RSB_IS_UNSIGNED(unsigned char));
	RSB_ASSERT(RSB_IS_UNSIGNED(unsigned long));
	RSB_ASSERT(RSB_IS_UNSIGNED(size_t));

	RSB_ASSERT(RSB_ERR_NO_ERROR==0);
	RSB_ASSERT(RSB_ERR_GENERIC_ERROR==-1);

	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(short int)<RSB_MAX_VALUE_FOR_TYPE(unsigned short int));
	/* 
		FIXME
		We found  RSB_MAX_VALUE_FOR_TYPE(char) == RSB_MAX_VALUE_FOR_TYPE(unsigned char)  (==255) on
		 IBM XL C/C++ Enterprise Edition V7.0
		 Version: 07.00.0000.0005
	 */
#if   defined(__xlC__)
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(signed char)	<RSB_MAX_VALUE_FOR_TYPE(unsigned char));
#else
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(char)	<RSB_MAX_VALUE_FOR_TYPE(unsigned char));
#endif /* __xlC__ */

	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(signed int)	<RSB_MAX_VALUE_FOR_TYPE(unsigned int));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(int)	<RSB_MAX_VALUE_FOR_TYPE(unsigned int));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(signed long)	<RSB_MAX_VALUE_FOR_TYPE(unsigned long));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(long)	<RSB_MAX_VALUE_FOR_TYPE(unsigned long));

	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_nnz_idx_t)>=RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t)>=RSB_MAX_VALUE_FOR_TYPE(rsb_blk_idx_t));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t)>=RSB_MAX_VALUE_FOR_TYPE(rsb_submatrix_idx_t));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t)>=RSB_MAX_VALUE_FOR_TYPE(rsb_half_idx_t));

	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(size_t)         >=RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(size_t)         >=RSB_MAX_VALUE_FOR_TYPE(rsb_nnz_idx_t));
	RSB_ASSERT(RSB_MAX_VALUE_FOR_TYPE(size_t)         >=RSB_MAX_VALUE_FOR_TYPE(rsb_blk_idx_t));

	RSB_ASSERT(RSB_MAX_MATRIX_DIM>255);
	RSB_ASSERT(RSB_MAX_MATRIX_NNZ>255);
	RSB_ASSERT(RSB_MAX_MATRIX_NNZ >= RSB_MAX_MATRIX_DIM);

	RSB_ASSERT( RSB_IS_VALID_NNZ_SUM(RSB_MAX_MATRIX_NNZ/2,RSB_MAX_MATRIX_NNZ/2));
	RSB_ASSERT(!RSB_IS_VALID_NNZ_SUM(RSB_MAX_MATRIX_NNZ,1));

	RSB_ASSERT(RSB_MARKER_COO_VALUE>0);
	RSB_ASSERT(RSB_MAX_ALLOCATABLE_MEMORY_CHUNK>0);

	RSB_ASSERT(rsb__do_is_candidate_size_for_halfword_csr(30000,30000,30000,RSB_FLAG_USE_HALFWORD_INDICES_CSR));
	RSB_ASSERT(rsb__do_is_candidate_size_for_halfword_csr(64000,64000,64000,RSB_FLAG_USE_HALFWORD_INDICES_CSR));

	/* if any of the following is not true, the library would lose consistence */
	RSB_ASSERT(RSB_IS_UNSIGNED(rsb_half_idx_t));
	RSB_ASSERT(RSB_IS_SIGNED(rsb_coo_idx_t));
	RSB_ASSERT(RSB_IS_SIGNED(rsb_nnz_idx_t));
	{
		rsb_half_idx_t h = RSB_MAX_VALUE_FOR_TYPE(rsb_half_idx_t);
		rsb_coo_idx_t c = RSB_MAX_VALUE_FOR_TYPE(rsb_coo_idx_t);
		RSB_ASSERT(c>=h);
		RSB_ASSERT((c-h)>=0);
	}
	
	RSB_ASSERT(rsb__util_strlen(RSB_PERFORMANCE_BINARY_DUMP_FILE_SIGNATURE)<RSB_PERFORMANCE_BINARY_DUMP_FILE_SIGNATURE_MAX_CHARS);
	{
		/* EXPERIMENTAL, FIXME */
		rsb_int_t ti;
		rsb_type_t types [] = RSB_MATRIX_TYPE_CODES_ARRAY;
		for(ti=0;ti<RSB_IMPLEMENTED_TYPES	;++ti)
			RSB_ASSERT(rsb__do_sizeof(types[ti])<=RSB_CONST_ENOUGH_BYTES_FOR_ANY_TYPE);
	}
	return RSB_ERR_NO_ERROR;
}

rsb_err_t rsb__init_check_for_system_constants_correctness(void)
{
	/*!
	 * \ingroup gr_internals
	 *
	 * TODO: needs some testing code.
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
#if 1
	/* Let's see if some invariants hold despite user set options. */

	if(sizeof(rsb_blk_idx_t)>sizeof(rsb_coo_idx_t))
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
	if(sizeof(rsb_coo_idx_t)>sizeof(rsb_nnz_idx_t))
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
#ifdef CHAR_BIT
	if(sizeof(rsb_flags_t)<(4*CHAR_BIT)/8)
#else /* CHAR_BIT */
	if(sizeof(rsb_flags_t)<4)/* remember that the C standard does not mandate 8 bits per byte */
#endif /* CHAR_BIT */
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
#ifdef CHAR_BIT
	if(sizeof(rsb_err_t)<(4*CHAR_BIT)/8)
#else /* CHAR_BIT */
	if(sizeof(rsb_err_t)<4)/* remember that the C standard does not mandate 8 bits per byte */
#endif /* CHAR_BIT */
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
#endif
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__do_init_inner(void)
{
	/*!
	 * \ingroup gr_internals
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	RSB_BZERO_P(&rsb_global_session_handle);
	rsb_global_session_handle.rsb_g_initialized = RSB_BOOL_FALSE;
	rsb_global_session_handle.memory_hierarchy_levels = 0;
#if RSB_WANT_PERFORMANCE_FILE
	rsb_global_session_handle.performance_binary_dump_file = RSB_PERFORMANCE_BINARY_DUMP_FILE;
#endif /* RSB_WANT_PERFORMANCE_FILE */
	rsb_global_session_handle.asm_sort_method = 0;
	rsb_global_session_handle.cache_blocking_method = 0;
	rsb_global_session_handle.mhis = NULL;
	rsb_global_session_handle.subdivision_multiplier = 1.0;
#if RSB_WANT_BOUNDED_BOXES
	rsb_global_session_handle.want_bounded_box = 1;
#endif /* RSB_WANT_BOUNDED_BOXES */
#if RSB_WANT_DEBUG_VERBOSE_INTERFACE_NOTICE
	rsb_global_session_handle.rsb_g_verbose_interface = 0;
#endif /* RSB_WANT_DEBUG_VERBOSE_INTERFACE_NOTICE */
#if RSB_HAVE_STREAMS
	rsb_global_session_handle.init_stream = NULL;
	rsb_global_session_handle.exit_stream = NULL;
	rsb_global_session_handle.error_stream = stderr;
	rsb_global_session_handle.out_stream = stdout;
#endif /* RSB_HAVE_STREAMS */
#if RSB_WANT_LIBRSB_TIMER
	rsb_global_session_handle.etime = RSB_TIME_ZERO;
#endif /* RSB_WANT_LIBRSB_TIMER */
#if RSB_WANT_OMP_RECURSIVE_KERNELS
#if 0
	#pragma omp parallel  RSB_NTC
	{
		rsb_global_session_handle.rsb_g_threads = omp_get_num_threads();
		rsb_global_session_handle.rsb_want_threads = rsb_global_session_handle.rsb_g_threads;
		/* the user may modify rsb_want_threads in a second moment */
	}
#else
		rsb_global_session_handle.rsb_g_threads = omp_get_max_threads();
		rsb_global_session_handle.rsb_want_threads = rsb_global_session_handle.rsb_g_threads;
#endif
	if(rsb_global_session_handle.rsb_g_threads>RSB_CONST_MAX_SUPPORTED_CORES)
	{
		errval = RSB_ERR_UNSUPPORTED_FEATURE;
		RSB_PERR_GOTO(err,"seems like your machine supports %ld threads. this code was compiled to support max %ld\n"
				,rsb_global_session_handle.rsb_g_threads
				,RSB_CONST_MAX_SUPPORTED_CORES
				);
	}
#endif /* RSB_WANT_OMP_RECURSIVE_KERNELS */

	RSB_DO_ERROR_CUMULATE(errval,rsb__init_mem_hierarchy_info());
	RSB_DO_ERROR_CUMULATE(errval,rsb__init_check_for_constants_correctness());
	RSB_DO_ERROR_CUMULATE(errval,rsb__init_check_for_system_constants_correctness());

	/* basic sanity checks */
	errval = rsb__util_m4_sanity_check();
	if(RSB_SOME_ERROR(errval))
	{
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	if(
			0 // RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS!=0x40000000) // 20130109 shall fix PSBLAS accordingly
			//(RSB_FLAG_EXTERNALLY_ALLOCATED_ARRAYS!=0x02)
			||(RSB_FLAG_SORTED_INPUT!=0x04)
			||(RSB_FLAG_FORTRAN_INDICES_INTERFACE!=0x01))
	{
		// these values are fixed, as PSBLAS uses them hardcoded (20101124)
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	if((errval = rsb__sys_init())!=RSB_ERR_NO_ERROR)
	{
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	if((errval = rsb__do_bindump_init())!=RSB_ERR_NO_ERROR)
	{
		if(errval != RSB_ERR_UNSUPPORTED_FEATURE)
		{ RSB_PERR_GOTO(err,RSB_ERRM_ES); }
		errval = RSB_ERR_NO_ERROR;	/* we ignore such an error, for now */
	}

	rsb__g_rsb_memory_counter_init();/* .. if any */

#if RSB_WANT_PERFORMANCE_COUNTERS_IN_RSB_INIT
	if((errval = rsb_perf_counters_init())!=RSB_ERR_NO_ERROR)
	{	
		RSB_STDERR("problem initializing performance counters (rsb_perf_counters_init gave %d)\n",(int)errval);
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}
#endif /* RSB_WANT_PERFORMANCE_COUNTERS_IN_RSB_INIT */
	/* checking the global memory counter */
	if(rsb__get_g_rsb_memory_count()!=0)
	{
		errval = RSB_ERR_INTERNAL_ERROR;
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	rsb__perf_init();
	if(RSB_SHOULD_FAIL_INIT_IF_MEMHIER_DETECTION_FAILS)
	{
		long cbs = rsb__get_cache_block_byte_size();
		long lcs = rsb__get_lastlevel_c_size();
		if( cbs<RSB_MIN_ALLOWED_CACHE_BLOCK_SIZE || cbs>RSB_MAX_ALLOWED_CACHE_BLOCK_SIZE )
		{
			errval = RSB_ERR_FAILED_MEMHIER_DETECTION;
			RSB_PERR_GOTO(herr,"Detected cache block size (%ld) value seems wrong.\n",cbs);
		}
		if( lcs<RSB_MIN_ALLOWED_CACHE_BLOCK_SIZE || lcs>RSB_MAX_ALLOWED_CACHE_BLOCK_SIZE )
		{
			errval = RSB_ERR_FAILED_MEMHIER_DETECTION;
			RSB_PERR_GOTO(herr,"Detected last level cache block size (%ld) value seems wrong.\n",lcs);
		}
	}

	rsb_global_session_handle.rsb_g_initialized = RSB_BOOL_TRUE;
	goto err;
herr:
	{
		const char * mhis = rsb__init_get_mem_hierarchy_info_string(RSB_BOOL_TRUE);
		if(mhis)
			RSB_ERROR("Please check your memory hierarchy info string, detected as: \"%s\"\n",mhis);
		else
			RSB_ERROR("Your memory hierarchy info string has not been detected\n");
	}
err:
	RSB_DO_ERR_RETURN(errval)
}

rsb_err_t rsb__do_init(struct rsb_initopts * io)
{
	/*!
	 */

#if 0
	rsb_int_t oi,on = io?io->n_pairs:0,ko = 0,uo = 0;
#endif
	rsb_err_t errval = RSB_ERR_NO_ERROR;
#if 0
	// pre-options
	for(oi=0;oi<on;++oi)
	switch(io->keys[oi])
	{
		default: uo++; // we ignore further error processing here
	}
#endif

	errval = rsb__do_init_inner();

	if(RSB_SOME_ERROR(errval))
	{
		RSB_PERR_GOTO(err,RSB_ERRM_ES); 
	}

	errval = rsb__do_reinit(io);
#if 0
	if(ko!=uo)
	{
		RSB_PERR_GOTO(err,RSB_ERRM_ES); 
		// FIXME: place unknown option error processing here
	}
#endif
err:
	return errval;
}

rsb_err_t rsb__do_reinit(struct rsb_initopts * io)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_int_t oi,on = io?io->n_pairs:0,ko = 0,uo = 0;

	if(!io)
	{
		goto err;
	}

	if(on && ( !io->keys || !io->values ) )
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,"init options struct given with allegedly %d pairs, but NULL pointers ?",on);
	}

#if 0
	if(io && io->action == RSB_IO_SPECIFIER_GET)
	for(oi=0;oi<on;++oi)
	{
		/* FIXME: shall modify RSB_IF_NOT_NULL_SET_TO_CASTED for performing either input or output */
		if(io->keys[oi]==RSB_IO_WANT_EXECUTING_THREADS)
		{
			//RSB_IF_NOT_NULL_SET_TO_CASTED(*(rsb_int_t*)io->values[oi],&rsb_global_session_handle.rsb_want_threads,rsb_int_t);
			RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.rsb_want_threads,(rsb_int_t*)(io->values[oi]),rsb_int_t,io->action,errval);
		}
	}
#endif

	if((io!=NULL) && (((io->action == RSB_IO_SPECIFIER_GET)) || (io->action == RSB_IO_SPECIFIER_SET)))
	for(oi=0;oi<on;++oi)
	switch(io->keys[oi])
	{
		case RSB_IO_WANT_VERBOSE_INIT:
#if RSB_HAVE_STREAMS
		//RSB_IF_NOT_NULL_SET_TO_CASTED(rsb_global_session_handle.init_stream,io->values[oi],FILE*);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.init_stream,io->values[oi],FILE*,io->action,errval);
#else /* RSB_HAVE_STREAMS */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_NO_STREAM_OUTPUT_CONFIGURED_OUT); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_HAVE_STREAMS */
		ko++;
		break;
		case RSB_IO_WANT_VERBOSE_EXIT:
#if RSB_HAVE_STREAMS
		//RSB_IF_NOT_NULL_SET_TO_CASTED(rsb_global_session_handle.exit_stream,io->values[oi],FILE*);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.exit_stream,io->values[oi],FILE*,io->action,errval);
#else /* RSB_HAVE_STREAMS */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_NO_STREAM_OUTPUT_CONFIGURED_OUT); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_HAVE_STREAMS */
		ko++;
		break;
		case RSB_IO_WANT_OUTPUT_STREAM:
#if RSB_HAVE_STREAMS
		//RSB_IF_NOT_NULL_SET_TO_CASTED(rsb_global_session_handle.out_stream,io->values[oi],FILE*);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.out_stream,io->values[oi],FILE*,io->action,errval);
#else /* RSB_HAVE_STREAMS */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_NO_STREAM_OUTPUT_CONFIGURED_OUT); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_HAVE_STREAMS */
		ko++;
		break;
		case RSB_IO_WANT_EXTRA_VERBOSE_INTERFACE:
#if RSB_WANT_DEBUG_VERBOSE_INTERFACE_NOTICE
		//RSB_IF_NOT_NULL_SET_TO_CASTED(rsb_global_session_handle.rsb_g_verbose_interface,io->values[oi],rsb_int_t);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.rsb_g_verbose_interface,io->values[oi],rsb_int_t,io->action,errval);
#else
		if(io->action == RSB_IO_SPECIFIER_GET)
		{ rsb_int_t mone = -1; RSB_IF_NOT_NULL_GET_SET_TO_CASTED(mone,io->values[oi],rsb_int_t,io->action,errval); }
		else
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_NO_STREAM_OUTPUT_CONFIGURED_OUT); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_WANT_DEBUG_VERBOSE_INTERFACE_NOTICE */
		ko++;
		break;
		case RSB_IO_WANT_VERBOSE_ERRORS:
#if RSB_HAVE_STREAMS
		//RSB_IF_NOT_NULL_SET_TO_CASTED(rsb_global_session_handle.error_stream,io->values[oi],FILE*);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.error_stream,io->values[oi],FILE*,io->action,errval);
#else /* RSB_HAVE_STREAMS */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_NO_STREAM_OUTPUT_CONFIGURED_OUT); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_HAVE_STREAMS */
		ko++;
		break;
		case RSB_IO_WANT_SORT_METHOD:
		//rsb_global_session_handle.asm_sort_method = (io->values[oi])?*(rsb_int_t*)(io->values[oi]):0;
		//rsb_global_session_handle.asm_sort_method = RSB_IF_NOT_NULL_CAST_TO(io->values[oi],rsb_int_t,0);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.asm_sort_method,io->values[oi],rsb_int_t,io->action,errval);
		ko++;
		break;
		case RSB_IO_WANT_BOUNDED_BOX_COMPUTATION:
#if RSB_WANT_BOUNDED_BOXES
		//rsb_global_session_handle.want_bounded_box = RSB_IF_NOT_NULL_CAST_TO(io->values[oi],rsb_int_t,1);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.want_bounded_box,io->values[oi],rsb_int_t,io->action,errval);
		ko++;
#else /* RSB_WANT_BOUNDED_BOXES */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_UNSUPPORTED_FEATURE); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_WANT_BOUNDED_BOXES */
		break;
		case RSB_IO_WANT_SUBDIVISION_MULTIPLIER:
		//rsb_global_session_handle.subdivision_multiplier = RSB_IF_NOT_NULL_CAST_TO(io->values[oi],rsb_real_t,0);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.subdivision_multiplier,io->values[oi],rsb_real_t,io->action,errval);
		ko++;
		break;
		case RSB_IO_WANT_CACHE_BLOCKING_METHOD:
		//rsb_global_session_handle.cache_blocking_method = RSB_IF_NOT_NULL_CAST_TO(io->values[oi],rsb_int_t,0);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.cache_blocking_method,io->values[oi],rsb_int_t,io->action,errval);
		ko++;
		break;
		case RSB_IO_WANT_MEMORY_HIERARCHY_INFO_STRING:
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.mhis,io->values[oi],const rsb_char_t*,io->action,errval);
		if((!RSB_SOME_ERROR(errval)) && ((io->action == RSB_IO_SPECIFIER_SET)))
			RSB_DO_ERROR_CUMULATE(errval,rsb__set_mem_hierarchy_info(rsb_global_session_handle.mhis));
		ko++;
		break;
		case RSB_IO_WANT_IS_INITIALIZED_MARKER:
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.rsb_g_initialized,io->values[oi],rsb_bool_t,io->action,errval);
		ko++;
		break;
		case RSB_IO_WANT_EXECUTING_THREADS:
		//RSB_IF_NOT_NULL_SET_TO_CASTED(rsb_global_session_handle.rsb_want_threads,io->values[oi],rsb_int_t);
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.rsb_want_threads,io->values[oi],rsb_int_t,io->action,errval);
		ko++;
		if(rsb__do_was_initialized())
			/*errval| = */rsb__set_num_threads(rsb_global_session_handle.rsb_want_threads);
		break;
		case RSB_IO_WANT_MEM_ALLOC_TOT:
		if(io->action == RSB_IO_SPECIFIER_GET)
		{
			size_t val = 0;
	#ifndef RSB_DISABLE_ALLOCATOR_WRAPPER
			val = rsb_global_session_handle.allocated_memory;
	#else
			RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_UNSUPPORTED_FEATURE); RSB_PERR_GOTO(err,RSB_ERRM_ES);
	#endif /* RSB_DISABLE_ALLOCATOR_WRAPPER */
			RSB_IF_NOT_NULL_GET_SET_TO_CASTED(val,io->values[oi],size_t,io->action,errval);
		}
		ko++;
		break;
		case RSB_IO_WANT_MEM_ALLOC_CNT:
		if(io->action == RSB_IO_SPECIFIER_GET)
		{
			size_t val = 0;
	#ifndef RSB_DISABLE_ALLOCATOR_WRAPPER
			val = rsb_global_session_handle.allocations_count;
	#else
			RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_UNSUPPORTED_FEATURE); RSB_PERR_GOTO(err,RSB_ERRM_ES);
	#endif /* RSB_DISABLE_ALLOCATOR_WRAPPER */
			RSB_IF_NOT_NULL_GET_SET_TO_CASTED(val,io->values[oi],size_t,io->action,errval);
		}
		ko++;
		break;
		case RSB_IO_WANT_LEAF_LEVEL_MULTIVEC:
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.want_outer_spmm,io->values[oi],rsb_int_t,io->action,errval);
		ko++;
		break;
		case RSB_IO_WANT_LIBRSB_ETIME:
		{
#if RSB_WANT_LIBRSB_TIMER
			RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.etime,io->values[oi],rsb_time_t,io->action,errval);
#else /* RSB_WANT_LIBRSB_TIMER */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_UNSUPPORTED_FEATURE); RSB_PERR_GOTO(err,RSB_ERRM_ES);
#endif /* RSB_WANT_LIBRSB_TIMER */
			ko++;
		}
		break;
#if RSB_WANT_ALLOCATOR_LIMITS
		case RSB_IO_WANT_MAX_MEMORY_ALLOCATIONS:
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.allocations_count_max,io->values[oi],size_t,io->action,errval);
		ko++;
		break;
		case RSB_IO_WANT_MAX_MEMORY_ALLOCATED:
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.memory_count_max,io->values[oi],size_t,io->action,errval);
		ko++;
		break;
#else /* RSB_WANT_ALLOCATOR_LIMITS */
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_UNSUPPORTED_FEATURE); RSB_PERR_GOTO(err,RSB_ERRM_NOLP);
#endif /* RSB_WANT_ALLOCATOR_LIMITS */
		case RSB_IO_WANT_VERBOSE_TUNING:
		RSB_IF_NOT_NULL_GET_SET_TO_CASTED(rsb_global_session_handle.verbose_tuning,io->values[oi],rsb_int_t,io->action,errval);
		ko++;
		break;
		default: uo++; // we ignore further error processing here
	}

err:
	return errval;
}

rsb_err_t rsb__do_exit(void)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;

#if RSB_WITH_SPARSE_BLAS_INTERFACE
	errval = rsb__BLAS_handles_free();
	if(RSB_SOME_ERROR(errval))
	{ RSB_PERR_GOTO(err,RSB_ERRM_ES); }
#endif /* RSB_WITH_SPARSE_BLAS_INTERFACE */

#if RSB_WANT_PERFORMANCE_COUNTERS_IN_RSB_INIT
	errval = rsb_perf_counters_finalize();
	if(RSB_SOME_ERROR(errval))
	{ RSB_PERR_GOTO(err,RSB_ERRM_ES); }
#endif /* RSB_WANT_PERFORMANCE_COUNTERS_IN_RSB_INIT */
	errval = rsb__perf_exit();
	if(RSB_SOME_ERROR(errval))
	{ RSB_PERR_GOTO(err,RSB_ERRM_ES); }

	errval = rsb__do_check_leak();
	//if(errval != RSB_ERR_NO_ERROR)goto err;
	if(errval != RSB_ERR_MEMORY_LEAK)
	{ goto err; }

	rsb_global_session_handle.rsb_g_initialized = RSB_BOOL_FALSE;
err:
	return errval;
}


/* @endcond */
