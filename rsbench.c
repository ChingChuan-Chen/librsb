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
/*!
 @file
 @author Michele Martone
 @brief
 This is the main program used to benchmark and test our library.
 This should be the swiss army knife program for our library.
 */
/*
  This not an example program: to be built, it needs all of the internal library headers.
 */

#include <stdlib.h>
#include "rsb.h"
#include "rsb_test_matops.h"
#include "rsb_failure_tests.h"
#include "rsb_internals.h"
#if RSB_WITH_SPARSE_BLAS_INTERFACE 
#include "rsb_libspblas_handle.h"
#endif /* RSB_WITH_SPARSE_BLAS_INTERFACE  */
#include "rsb_libspblas_tests.h"
//#include "rsb-config.h"
#if RSB_WANT_ACTION
#include <signal.h>
#if defined(RSB_WANT_ACTION_SIGNAL)
#else /* defined(RSB_WANT_ACTION_SIGNAL) */
#include <bits/sigaction.h>
#endif /* defined(RSB_WANT_ACTION_SIGNAL) */
#endif /* RSB_WANT_ACTION */

#define RSB_WANT_PERMISSIVE_RSBENCH 1
#define RSB_SHALL_UPDATE_COMPLETEBENCHS 1
#define RSB_WANT_REDUCED_RSB_M4_MATRIX_META_OPS 1 


#if RSB_WITH_LIKWID
#define RSB_RSBENCH_EXEC(FEXP) {rsb_err_t errval;RSB_LIKWID_MARKER_INIT;errval=RSB_ERR_TO_PROGRAM_ERROR(FEXP);RSB_LIKWID_MARKER_EXIT;return errval;}
#else /* RSB_WITH_LIKWID */
#define RSB_RSBENCH_EXEC(FEXP) {return RSB_ERR_TO_PROGRAM_ERROR(FEXP);}
#endif /* RSB_WITH_LIKWID */

#if RSB_WANT_ACTION
	int rsb__quit_rsbench;
#if defined(RSB_WANT_ACTION_SIGNAL)
#else /* defined(RSB_WANT_ACTION_SIGNAL) */
	struct sigaction rsb_osa;
#endif /* defined(RSB_WANT_ACTION_SIGNAL) */

void rsb__sigh(int signal)
{
	/* TODO: extend this mechanism optionally to the library itself. */

	if( rsb__quit_rsbench == 0 )
	{
		RSBENCH_STDOUT("\n");
		RSBENCH_STDOUT("====================================================\n");
		RSBENCH_STDOUT("Caught signal %d: will terminate as soon as possible.\n",signal);
		RSBENCH_STDOUT("  ( next time won't catch the signal anymore ).\n");
		RSBENCH_STDOUT("====================================================\n");
		RSBENCH_STDOUT("\n");
		rsb__quit_rsbench++;
	}
	else
	if( rsb__quit_rsbench == 1 )
	{
#if defined(RSB_WANT_ACTION_SIGNAL)
#else /* defined(RSB_WANT_ACTION_SIGNAL) */
		sigaction(SIGINT,&rsb_osa,NULL);
#endif /* defined(RSB_WANT_ACTION_SIGNAL) */
	}
}

void rsb__sigr(void)
{
	rsb__quit_rsbench = 0;
	{
#if RSB_WANT_ACTION_SIGNAL
		/* signal() is part of C99 */
		signal(SIGINT,&rsb__sigh); /* not to be called from a threaded environment ... */
#else /* RSB_WANT_ACTION_SIGNAL */
		/* sigaction() is part of POSIX, not part of C99 */
		struct sigaction act;
		RSB_BZERO_P(&act);
		RSB_BZERO_P(&rsb_osa);
		act.sa_handler  = rsb__sigh;
		sigemptyset(&act.sa_mask);
    		sigaction(SIGINT, &act,&rsb_osa);
/*
		sigaction(SIGUSR1, &act, &rsb_osa);
		sigaction(SIGUSR2, &act, &rsb_osa);

		sigaction(SIGQUIT,&act,&rsb_osa);
		sigaction(SIGTERM,&act,&rsb_osa);

		sigaction(SIGABRT,&act,&rsb_osa);
		sigaction(SIGTSTP,&act,&rsb_osa);

		sigaction(SIGBUS, &act,&rsb_osa);
		sigaction(SIGILL, &act,&rsb_osa);
	    	sigaction(SIGSEGV,&act,&rsb_osa);
*/
#endif /* RSB_WANT_ACTION_SIGNAL */
	}
}
#endif /* RSB_WANT_ACTION */

rsb_err_t rsb__print_configuration_string_rsbench(const char *pn, rsb_char_t * cs, rsb_bool_t wci)
{
	/* TODO: output buffer length check */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
#if RSB_WANT_MKL
#ifdef mkl_get_version
	MKLVersion mv;
#endif /* mkl_get_version */
#endif /* RSB_WANT_MKL */
	if(!cs)
	{
		errval = RSB_ERR_BADARGS;
		goto err;
	}
	errval = rsb__print_configuration_string(pn, cs, wci);
	if(wci == RSB_BOOL_FALSE)
		goto err;
#if RSB_WANT_MKL
#ifdef mkl_get_version
	mkl_get_version(&mv);
	sprintf(cs+strlen(cs),"MKL:%d.%d-%d, %s, %s, %s, %s\n",mv.MajorVersion,mv.MinorVersion,mv.UpdateVersion,mv.ProductStatus,mv.Build,mv.Processor,mv.Platform);
#else /* mkl_get_version */
	sprintf(cs+strlen(cs),"MKL:version unknown.\n");
#endif /* mkl_get_version */
#else /* RSB_WANT_MKL */
	sprintf(cs+strlen(cs),"MKL:not linked.\n");
#endif /* RSB_WANT_MKL */
#if RSB_WANT_XDR_SUPPORT
	sprintf(cs+strlen(cs),"XDR support: on.\n");
#else /* RSB_WANT_XDR_SUPPORT */
	sprintf(cs+strlen(cs),"XDR support: off.\n");
#endif /* RSB_WANT_XDR_SUPPORT */
err:
	return errval;
}

static int rsb__main_help(const int argc, char * const argv[], int default_program_operation, const char * program_codes, rsb_option *options)
{
			const char * pbn = rsb__basename(argv[0]);

			//RSB_STDOUT(
			printf(
				/*"[OBSOLETE DOCUMENTATION] \n"*/
				"Usage: %s [OPTIONS] \n"
				"  or:  %s [ -o OPCODE] [ -O {subprogram-code}] [ {subprogram-specific-arguments} ] \n"
				"%s "RSB_INFOMSG_SAK"."
				"\n"
				"\n"
				//"\tOne may choose {option} among:\n"
				//"\t-I for getting system information and some micro benchmarking\n"
				"\t\n"
				 "Choose {subprogram-code} among:\n\n"
				"\tr for the reference benchmark (will produce a machine specific file)\n\n"
				"\tc for the complete benchmark\n\n"
				"\te for the matrix experimentation code\n\n"
				"\td for a single matrix dumpout\n\n"
				"\tb for the (current, going to be obsoleted) benchmark\n\n"
				"\tt for some matrix construction tests\n\n"
				"\to obsolete, will soon be removed\n"
				"\n"
				 "{subprogram-specific-arguments} will be available from the subprograms.\n\n"
				"\te.g.: %s      -O b -h   will show the current benchmark subprogram's options\n\n"
				"\te.g.: %s -o a -O b -h   will show the spmv     benchmark subprogram's options\n\n"
				"\te.g.: %s -o n -O b -h   will show the negation benchmark subprogram's options\n\n"
//				"\te.g.: %s -o A -O b    will run all of the benchmark programs.\n"
				"\nThe default {subprogram-code} is '%c'\n"
				"\n\tWith OPCODE among '%s'\n"/* TODO: fix this description, as it is too laconic. */
				"\n"
				,pbn
				,pbn
				,pbn
				,pbn
				,pbn
				,pbn
				,default_program_operation
				,program_codes
				);
			if(options)
				rsb_test_help_and_exit(pbn,options,0);
	return 0;
}

int rsb_genmm_main(int argc,char *argv[]);
int rsb_mtx_ls_main(int argc,char *argv[]);

int main(const int argc, char * argv[])
{
	rsb_option options[] = {
	    {"help",			no_argument, NULL, 'h' },
	    {"matrix-operation",	required_argument, NULL, 'o' },
	    {"subprogram-operation",	required_argument, NULL, 'O' },
	    {"information",		no_argument, NULL, 'I' },
	    {"configuration",		no_argument, NULL, 'C' },
	    {"hardware-counters",	no_argument, NULL, 'H' },
	    {"experiments",		no_argument, NULL, 'e' },
	    {"version",			no_argument, NULL, 'v' },
	    {"blas-testing",		no_argument, NULL, 'B' },
	    {"quick-blas-testing",		required_argument, NULL, 'Q' },
	    {"error-testing",		required_argument, NULL, 'E' },
	    {"fp-bench",		no_argument, NULL, 'F' },
	    {"transpose-test",		no_argument, NULL, 't' },
	    {"limits-testing",		no_argument, NULL, 0x6c696d74 },
	    {"guess-blocking",		no_argument, NULL, 'G' },	/* will pass guess parameters here some day (FIXME: obsolete) */
	    {"generate-matrix",		no_argument, NULL, 'g' }, /* should be synced to rsb_genmm_main */
	    {"plot-matrix",		no_argument, NULL,  0x50505050},/* should be synced to rsb_dump_postscript */
	    {"matrix-ls",		no_argument, NULL,  0x006D6C73},/* should be synced to rsb_mtx_ls_main */
	    {"read-performance-record",		required_argument, NULL,  0x7270720a},/*  */
	    {"help-read-performance-record",		no_argument, NULL,  0x72707268},/*  */
	    {0,0,0,0}
	};

	/*
	 * NOTE: this implies that unless an argument reset mechanism is implemented here,
	 * the o and O options will be forwarded to the host program!
	 * */
	//const char default_operation='v';
	const char default_operation='a';
	char operation=default_operation;
	//const char default_program_operation='r';
	const char default_program_operation='b';
	char program_operation=default_program_operation;
	rsb_err_t errval = RSB_ERR_NO_ERROR;
#if RSB_WANT_REDUCED_RSB_M4_MATRIX_META_OPS
	const char * program_codes = "a"
#else /* RSB_WANT_REDUCED_RSB_M4_MATRIX_META_OPS */
	const char * program_codes = "avms"
#endif /* RSB_WANT_REDUCED_RSB_M4_MATRIX_META_OPS */
#if !RSB_SHALL_UPDATE_COMPLETEBENCHS
		"c"
#endif /* RSB_SHALL_UPDATE_COMPLETEBENCHS */
#ifdef RSB_OPTYPE_INDEX_SPSV_UXUA
		"t"
#endif
		"inS";
	int program_not_chosen=1;
	struct rsb_tester_options_t to;
	rsb_char_t cs[RSB_MAX_VERSION_STRING_LENGTH];
	int c;

	rsb_blas_tester_options_init(&to);

	for (;program_not_chosen;)
	{
		int opt_index = 0;
		c = rsb_getopt_long(argc,argv,"CP:"
				"gBGvMHIho:O:"
/* #if RSB_WANT_EXPERIMENTS_CODE
				"e"
#endif */ /* RSB_WANT_EXPERIMENTS_CODE */
				"FQ:E:",options,&opt_index);
		if (c == -1)break;
		switch (c)
		{
			case 'F':
				/*
				 * Floating point mini-benchmark
				 * */
				return (rsb_lib_init(RSB_NULL_INIT_OPTIONS) == RSB_ERR_NO_ERROR && rsb__fp_benchmark() == RSB_ERR_NO_ERROR) ?RSB_PROGRAM_ERROR:RSB_PROGRAM_SUCCESS;
			break;
			case 'G':
				/*
				 * Sparse GEMM preliminary code.
				 * TODO: remove this temporary case, as it may break other functionality with the G flag.
				 * */
				return RSB_ERR_TO_PROGRAM_ERROR(rsb_do_spgemm_test_code(argc-1,argv+1));
			case 'g':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb_genmm_main(argc,argv));
			break;
			case 0x006D6C73:
				return RSB_ERR_TO_PROGRAM_ERROR(rsb_mtx_ls_main(argc,argv));
			break;
			case 0x7270720a:
			case 0x72707268:
			goto qos;
			break;
			case 0x50505050:
				return RSB_ERR_TO_PROGRAM_ERROR(rsb_dump_postscript(argc,argv));
			break;
			case 0x6c696d74:
			{
				if((errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS)) != RSB_ERR_NO_ERROR)
					goto berr;
				RSB_DO_ERROR_CUMULATE(errval,rsb_blas_limit_cases_tester());
				if((!RSB_WANT_PERMISSIVE_RSBENCH) && RSB_SOME_ERROR(errval))goto ferr;
				return RSB_ERR_TO_PROGRAM_ERROR(errval);
			}
			break;
			case 'E':
			{
				if((errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS)) != RSB_ERR_NO_ERROR)
					goto berr;
				RSB_DO_ERROR_CUMULATE(errval,rsb_blas_failure_tester(optarg));
				if((!RSB_WANT_PERMISSIVE_RSBENCH) && RSB_SOME_ERROR(errval))goto ferr;
				return RSB_ERR_TO_PROGRAM_ERROR(errval);
			}
			case 'Q':
				to.mtt = rsb__util_atof(optarg);
				if(strstr(optarg,"R")!=NULL)to.rrm=RSB_BOOL_TRUE;
				if(strstr(optarg,"U")!=NULL)to.tur=RSB_BOOL_TRUE;
				if(strstr(optarg,"Q")!=NULL)to.wqt=RSB_BOOL_TRUE;
				if(strstr(optarg,"q")!=NULL)to.wqc=RSB_BOOL_TRUE;
				if(strstr(optarg,"C")!=NULL)to.wcs=RSB_BOOL_TRUE;
			case 'B':
			RSB_SIGHR
			/* Sparse BLAS test.  */
//#if RSB_WITH_SPARSE_BLAS_INTERFACE 
#if 1
			{
				if((errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS)) != RSB_ERR_NO_ERROR)
					goto berr;
#if RSB_ALLOW_INTERNAL_GETENVS
				if(getenv("RSB_RSBENCH_BBMB") && rsb__util_atoi(getenv("RSB_RSBENCH_BBMB")) )
					goto bbmb;
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
				RSB_DO_ERROR_CUMULATE(errval,rsb_blas_runtime_limits_tester());
				if((!RSB_WANT_PERMISSIVE_RSBENCH) && RSB_SOME_ERROR(errval))goto ferr;
#if 0
				/*  TODO: this is here temporarily */
				RSB_DO_ERROR_CUMULATE(errval,rsb__do_lock_test());
#endif
				RSB_DO_ERROR_CUMULATE(errval,rsb_blas_mini_tester());
				if((!RSB_WANT_PERMISSIVE_RSBENCH) && RSB_SOME_ERROR(errval))goto ferr;
				//RSB_LIKWID_MARKER_INIT;
				//RSB_LIKWID_MARKER_R_START("RSB-QUICKTEST");
#if RSB_ALLOW_INTERNAL_GETENVS
bbmb:
#endif /* RSB_ALLOW_INTERNAL_GETENVS */
				RSB_DO_ERROR_CUMULATE(errval,rsb_blas_bigger_matrices_tester(&to));/* TODO: options should be passed here */
				//RSB_LIKWID_MARKER_R_STOP("RSB-QUICKTEST");
				//RSB_LIKWID_MARKER_EXIT;
				if((!RSB_WANT_PERMISSIVE_RSBENCH) && RSB_SOME_ERROR(errval))
					goto ferr;
				goto ferr;
			}
#else
				RSB_STDERR("no Sparse BLAS interface built.\n");
				//return -1;
				return 0;
#endif
			break;
			case 'C':
				if((errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS)) != RSB_ERR_NO_ERROR)
					goto err;
				errval = rsb__print_configuration_string_rsbench(argv[0],cs,RSB_BOOL_TRUE);
				printf("%s",cs);
				goto verr;
			break;
			case 'v':
				if((errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS)) != RSB_ERR_NO_ERROR)
					goto err;
				errval = rsb__print_configuration_string_rsbench(argv[0],cs,RSB_BOOL_FALSE);
				printf("%s",cs);
				goto verr;
			break;
			case 'M':
				return
				(rsb_lib_init(RSB_NULL_INIT_OPTIONS) == RSB_ERR_NO_ERROR && rsb__memory_benchmark() == RSB_ERR_NO_ERROR) ?RSB_PROGRAM_ERROR:RSB_PROGRAM_SUCCESS;
			break;
			case 'H':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb_hc_main());		/* preliminary */
			break;
			case 'I':
				rsb_lib_init(RSB_NULL_INIT_OPTIONS);
			return
				RSB_ERR_TO_PROGRAM_ERROR(rsb_perror(NULL,rsb__sys_info()));
			break;
			/*
#if RSB_WANT_EXPERIMENTS_CODE
			case 'e':
				return rsb_exp_bcsr_guess_experiments(argc,argv);
			break;
#endif */ /* RSB_WANT_EXPERIMENTS_CODE */
			case 'P':
		{
			errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS);
			if(RSB_SOME_ERROR(errval))
				goto err;
			errval = rsb_file_mtx_save(rsb_file_mtx_load(optarg,RSB_FLAG_DEFAULT_MATRIX_FLAGS,RSB_NUMERICAL_TYPE_DEFAULT,NULL),NULL);
			RSB_MASK_OUT_SOME_ERRORS(errval)
			goto verr;
		}
			break;
			case 'o':
				operation=*optarg;
			break;
			case 'O':
				program_operation=*optarg;
				program_not_chosen=0;
			break;
			case 'h':
				/* getsubopt may come in help here */
				return rsb__main_help(argc, argv,default_program_operation,program_codes,options);
			break;
			/*
			case 't':
				return rsb__main_transpose(argc,argv);
			break;	    	
			*/
			default:
			{
			}
		}
	}

qos:	/* quit option selection */

	if(c == 0x72707268)
	{
		errval = rsb__pr_dumpfiles(NULL,0);
		return RSB_ERR_TO_PROGRAM_ERROR(errval);
	}
	if(c == 0x7270720a)
	{
/*
		if(argc == 3)
			return rsb__pr_dumpfile(optarg);
		if(argc == 3)
			return rsb__pr_dumpfiles(&optarg,1);
 */

		if(argc >= 3)
		{
			const int RSB__PR_DUMP_MAXARGS = 1024; /* TODO: temporary */
			const rsb_char_t*fna[RSB__PR_DUMP_MAXARGS];
			int i;
			for(i=2;i<RSB_MIN(RSB__PR_DUMP_MAXARGS,argc);++i)
				fna[i-2] = argv[i];
			errval = rsb__pr_dumpfiles(fna,i-2);
			return RSB_ERR_TO_PROGRAM_ERROR(errval);
		}
	}

	if(program_not_chosen)
		return rsb__main_help(argc, argv,default_program_operation,program_codes,options);

	switch (program_operation)	/* O */
	{
		case 'r':
	{
			/*
			 * A benchmark to compute (machine,compiled) reference performance values.
			 * */
			errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS);
			if(errval == RSB_ERR_NO_ERROR)
				goto verr;
			errval = rsb__do_referencebenchmark(); /* FIXME: probably obsolete */
			RSB_MASK_OUT_SOME_ERRORS(errval)
			goto verr;
	}
		break;
		case 'R':
			/**/
			/*
			 * Dump current (hardcoded) performance info without computing anything.
			 * */
			errval = rsb__dump_current_global_reference_performance_info(); /* FIXME: probably obsolete */
			RSB_MASK_OUT_SOME_ERRORS(errval)
			goto verr;
		break;
#if !RSB_SHALL_UPDATE_COMPLETEBENCHS
		case 'c':
			/* A complete benchmark.  TODO: this is broken / old; needs a revamp, or oblivion  */
			errval = rsb_do_completebenchmark(argc,argv); /* FIXME: probably obsolete */
			RSB_MASK_OUT_SOME_ERRORS(errval)
			goto verr;
		break;
#endif /* RSB_SHALL_UPDATE_COMPLETEBENCHS */
		case 'd':
			/*
			 * A single matrix dump (almost useless).
			 * */
			return rsb_test_dump_main(argc,argv); /* FIXME: probably obsolete */
		break;
		case 'e':
			/*
			 * The matrix experimentation code.
			 * */
			RSB_STDERR("this option was obsoleted by -oS -Ob\n");
			/*return rsb_test_main_block_partitioned_matrix_stats(argc,argv); */ /* FIXME: probably obsolete */
			return -1;
		break;
		case 'b':
		{
			/*
			 * The current reference benchmark.
			 * */
			RSB_SIGHR
			switch(operation)	/* o */
			{
#ifdef RSB_HAVE_OPTYPE_SPMV_UAUA
				case 'a':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_spmv_uaua(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_SPMV_UAUA */
#if !RSB_WANT_REDUCED_RSB_M4_MATRIX_META_OPS
#ifdef RSB_HAVE_OPTYPE_SPMV_UAUZ
				case 'v':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_spmv_uauz(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_SPMV_UAUZ */
#ifdef RSB_HAVE_OPTYPE_SPMM_AZ
				case 'm':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_spmm_az(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_SPMM_AZ */
#ifdef RSB_HAVE_OPTYPE_SCALE
				case 's':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_scale(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_SCALE */
#ifdef RSB_HAVE_OPTYPE_SPMV_UXUX
				case 'c':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_spmv_uxux(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_SPMV_UXUX */
#ifdef RSB_HAVE_OPTYPE_INFTY_NORM
				case 'i':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_infty_norm(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_INFTY_NORM */
#ifdef RSB_HAVE_OPTYPE_NEGATION
				case 'n':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_negation(argc,argv));
				break;
#endif /* RSB_HAVE_OPTYPE_NEGATION */
#endif /* RSB_WANT_REDUCED_RSB_M4_MATRIX_META_OPS */
#ifdef RSB_OPTYPE_INDEX_SPSV_UXUA
				case 't':
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_spsv_uxua(argc,argv));
				break;
#endif /* RSB_OPTYPE_INDEX_SPSV_UXUA */
#if 1	/* this is a special case */
				case 'S':
				//return rsb__main_block_partitioned_sort_only(argc,argv);//old
				return RSB_ERR_TO_PROGRAM_ERROR(rsb__main_block_partitioned_mat_stats(argc,argv));//new
				break;
#endif
				default:
				RSB_STDERR(
					"You did not choose a correct operation code.\n"
					"Choose one among %s.\n",program_codes
					);
				errval = RSB_ERR_UNSUPPORTED_OPERATION;
				RSB_DO_ERR_RETURN(errval)
			}
		}
		break;
#if 0
		case 't': /* to reintegrate, add 't' to program_codes */
			/*
			 * A whole matrix repartitioning test.
			 * */
			return RSB_ERR_TO_PROGRAM_ERROR(rsb_test_main_block_partitioned_construction_test(argc,argv));
		break;
#endif
		default:
			RSB_STDERR("You did not choose an action. See help:\n");
			return rsb__main_help(argc, argv,default_program_operation,program_codes,NULL);
		return RSB_PROGRAM_SUCCESS;
    	}
	goto err;
ferr:
	/* rsb__getrusage(); */
	RSB_DO_ERROR_CUMULATE(errval,rsb_lib_exit(RSB_NULL_EXIT_OPTIONS));
berr:
	if(RSB_SOME_ERROR(errval))
		rsb_perror(NULL,errval);
verr:
	return RSB_ERR_TO_PROGRAM_ERROR(errval);
err:
	if(RSB_SOME_ERROR(errval))
		rsb_perror(NULL,errval);
	return RSB_PROGRAM_ERROR;
}

/* @endcond */
