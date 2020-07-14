/* @cond INNERDOC */
/*!
 * @file
 * @author Michele Martone
 * @brief
 * This source file contains some MKL interfacing functions.
 * */
include(`rsb_misc.m4')dnl
include(`do_unroll.m4')dnl
include(`libspblas_macros.m4')dnl
RSB_M4_HEADER_MESSAGE()dnl
dnl
ifdef(`ONLY_WANT_HEADERS',`
#ifndef RSB_RSB_MKL_H_INCLUDED
#define RSB_RSB_MKL_H_INCLUDED
#include "rsb_internals.h"
#if RSB_WANT_MKL
#include <mkl.h>
#include <mkl_blas.h>	/* dgemm, ... */
#include <mkl_spblas.h>
/* #include <mkl_types.h> */
/* #include <mkl_service.h> */ /* mkl_get_version */
',`dnl
#include "rsb_mkl.h"
#if RSB_WANT_MKL
')
dnl

dnl
define(`RSB_M4_RSB_TYPE_TO_MKL_TYPE',`dnl
pushdef(`type',$1)`'dnl
dnl
ifelse(type,`double complex',`MKL_Complex16')`'dnl
ifelse(type,`float complex',`MKL_Complex8')`'dnl
ifelse(type,`long double',`long double')`'dnl
ifelse(type,`double',`double')`'dnl
ifelse(type,`float',`float')`'dnl
ifelse(type,`int',`MKL_INT')`'dnl
dnl
popdef(`type')`'dnl
dnl
')dnl
dnl

rsb_err_t rsb__mkl_gemv(rsb_type_t typecode, const void * Mp, const void*Bp, void*Xp, rsb_nnz_idx_t mdim, rsb_coo_idx_t vdim, rsb_coo_idx_t*udimp)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	/* FIXME: TODO: incX != 1 */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	const MKL_INT dim=(rsb_coo_idx_t)sqrt((double)mdim);
	const MKL_INT incX=1;
	char transA_mkl=110;
dnl ,transB=110;
	if(!Mp || !Xp || !Bp)
		goto err;
	if(dim<1 || dim>vdim)
		goto err;
foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	{
		const mtype alpha=RSB_M4_ONE(mtype), beta=RSB_M4_ONE(mtype);
		`'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`gemv'(&transA_mkl,&dim,&dim, (RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)(&alpha),(const RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)Mp,&dim,(const RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)Bp,&incX,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)&beta,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)Xp,&incX);
dnl		`'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`gemm'(&transA_mkl,&transB,&dim,&dim,&dim,&alpha,(const mtype*)Mp,&dim,(const mtype*)Bp,&dim,&beta,(mtype*)Xp,&dim);
	}
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
	else 
')dnl
		errval=RSB_ERR_BADARGS;

	if(udimp)
		*udimp=dim;
err:
	return errval;
}
')dnl
dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
static char rsb_rsb_to_mkl_trans(rsb_trans_t transA_mkl)
{
	/**
	 * \ingroup gr_internals
	 */
	switch(transA_mkl)
	{
		case(RSB_TRANSPOSITION_N):
		return singlequote(n);
		break;
		case(RSB_TRANSPOSITION_T):
		return singlequote(t);
		break;
		case(RSB_TRANSPOSITION_C):
		return singlequote(c);
		break;
		default:
		return singlequote(n);	// FIXME
	}
}
')dnl
dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
static char rsb_rsb_to_mkl_sym(rsb_flags_t flags)
{
	if(RSB_DO_FLAG_HAS(flags,RSB_FLAG_SYMMETRIC))
		return singlequote(s);
	if(RSB_DO_FLAG_HAS(flags,RSB_FLAG_TRIANGULAR))
		return singlequote(t);
	else
		return singlequote(g);
}
')dnl
dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
static char rsb_rsb_to_mkl_upl(rsb_flags_t flags)
{
	if(RSB_DO_FLAG_HAS(flags,RSB_FLAG_LOWER))
		return singlequote(l);
	else
		return singlequote(u);
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_coo_spmv(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * x, void * y, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing

foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`coomv'(&transA_mkl,(MKL_INT*)(&m),(MKL_INT*)(&k),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)IA,(MKL_INT*)JA,(MKL_INT*)(&nnz),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)x,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)betap,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)y);
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_coo_spmm(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT nrhs, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * b, const MKL_INT ldb, void * c, const MKL_INT ldc, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing

foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`coomm'(&transA_mkl,(MKL_INT*)(&m),(MKL_INT*)(&nrhs),(MKL_INT*)(&k),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)IA,(MKL_INT*)JA,(MKL_INT*)(&nnz),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)b,(MKL_INT*)(&ldb),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)betap,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)c,(MKL_INT*)(&ldc));
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_coo_spsv(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * x, void * y, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing
	/* 20101118	MKL 9.1 reference manual declares also k among the parameters */
foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`coosv'(&transA_mkl,(MKL_INT*)(&m),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)IA,(MKL_INT*)JA,(MKL_INT*)(&nnz),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)x,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)y);
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
static rsb_err_t rsb__do_mkl_csr_spmv(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * x, void * y, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags)dnl
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing

foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`csrmv'(&transA_mkl,(MKL_INT*)(&m),(MKL_INT*)(&k),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)JA,(MKL_INT*)IA,(MKL_INT*)(IA+1),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)x,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)betap,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)y);
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
/* The following three look weird, I know. */
#define RSB_GET_MKL_MAX_THREADS rsb__set_num_threads(RSB_THREADS_GET_MAX_SYS)
#define RSB_GET_MKL_BASE_THREADS 1 /* FIXME: no mkl_get_num_threads */
#define RSB_GET_MKL_DEFAULT_THREADS mkl_get_max_threads() /* omp_get_num_threads(); */
#define RSB_MKL_MAX_AT_TIME 1.0 /* FIXME */

#define RSB_MKL_SET_THREADS_RANGE(lnt,unt,otnp)				\
		if(RSB_DT_SAME_THREADS_TNP(otnp))			\
			lnt = unt = 0; 					\
		else							\
		{							\
			if(RSB_DT_THREADS_TUNE_TNP(otnp))		\
				; /* ok */				\
			else						\
				if(RSB_DT_SPEC_THREADS_TNP(otnp))	\
			lnt = unt = *otnp;				\
		}

#define RSB_MKL_THREADS_TUNING_ODECLS					\
		rsb_time_t tinf = rsb__timer_granularity();		\
		rsb_time_t best = RSB_CONST_IMPOSSIBLY_BIG_TIME;	\
		rsb_thread_t ont = RSB_GET_MKL_BASE_THREADS;		\
		rsb_thread_t nt, lnt = 1, unt = RSB_GET_MKL_MAX_THREADS;\
		rsb_thread_t otn = ont;					\
		rsb_thread_t dtn = RSB_GET_MKL_DEFAULT_THREADS;


#define RSB_MKL_THREADS_TUNING_IDECLS									\
			rsb_time_t it = rsb_time(), ct = RSB_TIME_ZERO;	/* initial/current time */	\
			rsb_time_t dt = it, tt = RSB_TIME_ZERO; /* elapsed (delta) / total  time */	\
			rsb_time_t bt = RSB_CONST_IMPOSSIBLY_BIG_TIME, wt = RSB_TIME_ZERO; /* best / worst  time */	\
			rsb_time_t ss = RSB_TIME_ZERO; /* sum of squares */				\
			rsb_time_t mint = RSB_TIME_ZERO; /* minimal time */				\
			rsb_int_t times = 0, mintimes = RSB_AT_MIN_TIMES, maxtimes = RSB_AUT0_TUNING_DEFAULT_TIMES ;	\
			rsb_time_t maxt = RSB_AT_MAX_TIME/* RSB_MKL_MAX_AT_TIME*/;
')dnl
dnl

dnl
rsb_err_t rsb__mkl_csr_spmv_bench(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * x, void * y, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags, rsb_thread_t *otnp, rsb_time_t *tpop, struct rsb_tattr_t* ttrp, struct rsb_ts_t*tstp)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t dt, tpo;

	if(otnp)
	{
		RSB_MKL_THREADS_TUNING_ODECLS
		RSB_MKL_SET_THREADS_RANGE(lnt,unt,otnp)

		for(nt=lnt;nt<=unt;++nt)
		{
			RSB_MKL_THREADS_TUNING_IDECLS
			if(nt) mkl_set_num_threads(nt);

			do
			{
				errval = rsb__do_mkl_csr_spmv(VA, m, k, nnz, IA, JA, x, y, alphap, betap, transA, typecode, flags);
				RSB_SAMPLE_STAT(it,ct,dt,tt,bt,wt,ss,tinf,times);
			}
			while(RSB_REPEAT(ct-it,times,mint,mintimes,maxt,maxtimes));

			dt = bt;
			if(dt < best )
			{
				otn = nt;
				best = RSB_MIN_ABOVE_INF(best,dt,tinf);
				RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp);
			}
			rsb__tattr_sets(ttrp,dtn,nt,dt,otn,times);/* FIXME: if no threads tuning, shall set dtpo = btpo, as well as ttrp.optt=0 */
			if(dtn == nt) RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp+1);
		}
		mkl_set_num_threads(ont);
done:
		ttrp->ttt += rsb_time(); /* ttrp->ttt = tt; */
		tpo = best; /* tpo = 1.0 / best; */
		*otnp = otn;
	}
	else
	{
		dt = -rsb_time();
		errval = rsb__do_mkl_csr_spmv(VA, m, k, nnz, IA, JA, x, y, alphap, betap, transA, typecode, flags);
		dt += rsb_time();
		/* tpo = 1.0 / dt; */
		tpo = dt;
	}
	if(tpop)
		*tpop = tpo;
	return errval;
}
')dnl
dnl

dnl
ifdef(`ONLY_WANT_HEADERS',`',`dnl
static rsb_err_t rsb__do_mkl_csr_spmm(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT n, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * b, const MKL_INT ldb, void * c, const MKL_INT ldc, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags)dnl
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing
	MKL_INT ldb_ = n, ldc_ = n; /* for zero based indexing */

	if(RSB_DO_FLAG_HAS(flags,RSB_FLAG_FORTRAN_INDICES_INTERFACE))
		ldb_ = k, ldc_ = m, /* for one based indexing */
		matdescra[3] = singlequote(f); // one based indexing

	#if 1
	/* n = nrhs */
foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`csrmm'(&transA_mkl,(MKL_INT*)(&m),(MKL_INT*)(&n),(MKL_INT*)(&k),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)JA,(MKL_INT*)IA,(MKL_INT*)(IA+1),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)b,(MKL_INT*)(&ldb_),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)betap,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)c,(MKL_INT*)(&ldc_));
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
#endif /* 1 */
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_csr_spmm_bench(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT n, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * b, const MKL_INT ldb, void * c, const MKL_INT ldc, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags, rsb_thread_t *otnp, rsb_time_t *tpop, struct rsb_tattr_t* ttrp, struct rsb_ts_t*tstp)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t dt, tpo;

	if(otnp)
	{
		RSB_MKL_THREADS_TUNING_ODECLS
		RSB_MKL_SET_THREADS_RANGE(lnt,unt,otnp)

		for(nt=lnt;nt<=unt;++nt)
		{
			RSB_MKL_THREADS_TUNING_IDECLS
			if(nt) mkl_set_num_threads(nt);

			do
			{
				errval = rsb__do_mkl_csr_spmm(VA, m, k, n, nnz, IA, JA, b, ldb, c, ldc, alphap, betap, transA, typecode, flags);
				RSB_SAMPLE_STAT(it,ct,dt,tt,bt,wt,ss,tinf,times);
			}
			while(RSB_REPEAT(ct-it,times,mint,mintimes,maxt,maxtimes));

			dt = bt;
			if(dt < best )
			{
				otn = nt;
				best = RSB_MIN_ABOVE_INF(best,dt,tinf);
				RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp);
			}
			rsb__tattr_sets(ttrp,dtn,nt,dt,otn,times);/* FIXME: if no threads tuning, shall set dtpo = btpo, as well as ttrp.optt=0 */
			if(dtn == nt) RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp+1);
		}
		mkl_set_num_threads(ont);
done:
		ttrp->ttt += rsb_time(); /* ttrp->ttt = tt; */
		tpo = best; /* tpo = 1.0 / best; */
		*otnp = otn;
	}
	else
	{
		dt = -rsb_time();
		errval = rsb__do_mkl_csr_spmm(VA, m, k, n, nnz, IA, JA, b, ldb, c, ldc, alphap, betap, transA, typecode, flags);
		dt += rsb_time();
		/* tpo = 1.0 / dt; */
		tpo = dt;
	}
	if(tpop)
		*tpop = tpo;
	return errval;
}
')dnl
dnl

dnl
rsb_err_t rsb__do_mkl_csr_spsv(const void *VA, const MKL_INT m, const MKL_INT k, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * x, void * y, const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing

foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`csrsv'(&transA_mkl,(MKL_INT*)(&m),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)JA,(MKL_INT*)IA,(MKL_INT*)(IA+1),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)x,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)y);
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
rsb_err_t rsb__do_mkl_csr_spsm(const void *VA, const MKL_INT m, const MKL_INT nrhs, const MKL_INT * IA, const MKL_INT *JA, const void * b, void * c, const void *alphap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags, const MKL_INT ldb, const MKL_INT ldc)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	char transA_mkl = rsb_rsb_to_mkl_trans(transA);
	char matdescra[]={RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL,RSB_NUL};
	matdescra[0] = rsb_rsb_to_mkl_sym(flags); // general ?
	matdescra[1] = rsb_rsb_to_mkl_upl(flags); // up or lo ?
	matdescra[2] = singlequote(n); // not unit diagonal
	matdescra[3] = singlequote(c); // zero based indexing

foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`csrsm'(&transA_mkl,(MKL_INT*)(&m),(MKL_INT*)(&nrhs),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)alphap,matdescra,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)VA,(MKL_INT*)JA,(MKL_INT*)IA,(MKL_INT*)(IA+1),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)b,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(int)*)&ldb,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)c,(RSB_M4_RSB_TYPE_TO_MKL_TYPE(int)*)&ldc);
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_csr_spsv_bench(const void *VA, const MKL_INT m, const MKL_INT k/*, const MKL_INT n*/, const MKL_INT nnz, const MKL_INT * IA, const MKL_INT *JA, const void * b, /*const MKL_INT ldb,*/ void * c,/* const MKL_INT ldc,*/ const void *alphap, const void * betap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags, rsb_thread_t *otnp, rsb_time_t *tpop, struct rsb_tattr_t* ttrp, struct rsb_ts_t*tstp)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t dt, tpo;

	if(otnp)
	{
		RSB_MKL_THREADS_TUNING_ODECLS
		RSB_MKL_SET_THREADS_RANGE(lnt,unt,otnp)

		for(nt=lnt;nt<=unt;++nt)
		{
			RSB_MKL_THREADS_TUNING_IDECLS
			if(nt) mkl_set_num_threads(nt);

			do
			{
				errval = rsb__do_mkl_csr_spsv(VA, m, k, nnz, IA, JA, b, c, alphap, betap, transA, typecode, flags);
				RSB_SAMPLE_STAT(it,ct,dt,tt,bt,wt,ss,tinf,times);
			}
			while(RSB_REPEAT(ct-it,times,mint,mintimes,maxt,maxtimes));

			dt = bt;
			if(dt < best )
			{
				otn = nt;
				best = RSB_MIN_ABOVE_INF(best,dt,tinf);
				RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp);
			}
			rsb__tattr_sets(ttrp,dtn,nt,dt,otn,times);/* FIXME: if no threads tuning, shall set dtpo = btpo, as well as ttrp.optt=0 */
			if(dtn == nt) RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp+1);
		}
		mkl_set_num_threads(ont);
done:
		ttrp->ttt += rsb_time(); /* ttrp->ttt = tt; */
		tpo = best; /* tpo = 1.0 / best; */
		*otnp = otn;
	}
	else
	{
		dt = -rsb_time();
		errval = rsb__do_mkl_csr_spsv(VA, m, k, nnz, IA, JA, b, c, alphap, betap, transA, typecode, flags);
		dt += rsb_time();
		/* tpo = 1.0 / dt; */
		tpo = dt;
	}
	if(tpop)
		*tpop = tpo;
	return errval;
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_csr_spsm_bench(const void *VA, const MKL_INT m, const MKL_INT nrhs, const MKL_INT * IA, const MKL_INT *JA, const void * b, void * c, const void *alphap, rsb_trans_t transA, rsb_type_t typecode, rsb_flags_t flags, const MKL_INT ldb, const MKL_INT ldc, rsb_thread_t *otnp, rsb_time_t *tpop, struct rsb_tattr_t* ttrp, struct rsb_ts_t*tstp)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_time_t dt, tpo;

	if(otnp)
	{
		RSB_MKL_THREADS_TUNING_ODECLS
		RSB_MKL_SET_THREADS_RANGE(lnt,unt,otnp)

		for(nt=lnt;nt<=unt;++nt)
		{
			RSB_MKL_THREADS_TUNING_IDECLS
			if(nt) mkl_set_num_threads(nt);

			do
			{
				errval = rsb__do_mkl_csr_spsm(VA, m, nrhs, IA, JA, b, c, alphap, transA, typecode, flags, ldb, ldc);
				RSB_SAMPLE_STAT(it,ct,dt,tt,bt,wt,ss,tinf,times);
			}
			while(RSB_REPEAT(ct-it,times,mint,mintimes,maxt,maxtimes));

			dt = bt;
			if(dt < best )
			{
				otn = nt;
				best = RSB_MIN_ABOVE_INF(best,dt,tinf);
				RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp);
			}
			rsb__tattr_sets(ttrp,dtn,nt,dt,otn,times);/* FIXME: if no threads tuning, shall set dtpo = btpo, as well as ttrp.optt=0 */
			if(dtn == nt) RSB_STAT_TAKE(it,otn,ct,dt,tt,bt,wt,ss,times,tstp+1);
		}
		mkl_set_num_threads(ont);
done:
		ttrp->ttt += rsb_time(); /* ttrp->ttt = tt; */
		tpo = best; /* tpo = 1.0 / best; */
		*otnp = otn;
	}
	else
	{
		dt = -rsb_time();
		errval = rsb__do_mkl_csr_spsm(VA, m, nrhs, IA, JA, b, c, alphap, transA, typecode, flags, ldb, ldc);
		dt += rsb_time();
		/* tpo = 1.0 / dt; */
		tpo = dt;
	}
	if(tpop)
		*tpop = tpo;
	return errval;
}
')dnl
dnl

dnl
rsb_err_t rsb__mkl_coo2csr(const MKL_INT m, const MKL_INT k, const MKL_INT nnz, const void *IVA, const MKL_INT * IIA, const MKL_INT *IJA, const void *OVA, const MKL_INT * OIA, const MKL_INT *OJA, rsb_type_t typecode, const MKL_INT mib)dnl
ifdef(`ONLY_WANT_HEADERS',`;',`
{
	int info;
	int job[6];
	job[0] = 1; // coo2csr (=1, the matrix in the coordinate format is converted to the CSR;=2, the matrix in the coordinate format is converted to the CSR format, and the column indices in CSR representation are sorted in the increasing order within each row.)
	job[1] = mib; // 0 based csr
	job[2] = 0; // 0 based coo
	job[3] = 0; // ignored
	job[4] = nnz; // ignored here
	job[5] = 0; // fill all three arrays

foreach(`mtype',RSB_M4_SPBLAS_MATRIX_SUPPORTED_TYPES,`dnl
`#ifdef 'RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype)
	if( typecode == RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) )
	`mkl_'RSB_M4_SPBLAS_TYPE_CHARCODE(mtype)`csrcoo'(job,(MKL_INT*)(&m),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)OVA,(MKL_INT*)OJA,(MKL_INT*)OIA,(MKL_INT*)(&nnz),(RSB_M4_RSB_TYPE_TO_MKL_TYPE(mtype)*)(IVA),(MKL_INT*)IIA,(MKL_INT*)IJA,&info);
	else 
#endif /* RSB_M4_NUMERICAL_TYPE_PREPROCESSOR_SYMBOL(mtype) */
')dnl
	return RSB_ERR_UNSUPPORTED_TYPE	;
	return RSB_ERR_NO_ERROR;
}
')dnl
dnl

dnl
#endif /* RSB_WANT_MKL */
dnl
ifdef(`ONLY_WANT_HEADERS',`dnl
#endif  /* RSB_RSB_MKL_H_INCLUDED */
')dnl
dnl
/* @endcond */

