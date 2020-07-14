

/** @file
    @brief
    Macros and constants, which are type specific.
    \n
    Here reside declarations related to supported matrix numerical types, and other declarations
    according to the build time options.
    \n
    If you wish to use this library with different matrix numerical types, you shall regenerate
     the library source code accordingly; see the README file how to do this.
    \n
    Only a small part of these declarations is needed to the user (see \ref matrix_type_symbols_section).
    \n
    Therefore, only the declarations which are commented are actually meant to be used in functions;
    please regard the remaining ones as internal.
  */

/*

Copyright (C) 2008-2018 Michele Martone

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
/*
 The code in this file was generated automatically by an M4 script. 
 It is not meant to be used as an API (Application Programming Interface).
 p.s.: right now, only row major matrix access is considered.

 */
#ifndef RSB_TYPES_H_INCLUDED
#define RSB_TYPES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef __cplusplus
/* complex.h is ISO C99 */
#include <complex.h>
#endif /* __cplusplus */
/* 
   Each one of the following symbols is assigned to a type which is supported
   by an option set at library code generation time.
   Other types may be enabled by regenerating the whole library code.
   To enable types, please read the documentation.
 */

/* Miscellaneous version strings.
  Adopting a naming scheme similar to that of png.h.
 */
#define RSB_LIBRSB_VER_STRING		"1.2.0"	/*!< Library version string. */
#define RSB_HEADER_VERSION_STRING		"librsb version 1.2.0-rc8-pre - July 15, 2019"	/*!< Library header version string. */
#define RSB_LIBRSB_VER_MAJOR		1	/*!< Major version. */
#define RSB_LIBRSB_VER_MINOR		2	/*!< Minor version. */
#define RSB_LIBRSB_VER_PATCH		0	/*!< Patch version. */
#define RSB_LIBRSB_VER		10200	/*!< Version number. */
#define RSB_LIBRSB_VER_DATE		RSB_M4_WANT_RSB_LIBRSB_VER_DATE	/*!< Version release date. */

#define RSB_HAVE_TYPE_DOUBLE  1 /*!< Type double is supported, so RSB_HAVE_TYPE_DOUBLE  is defined .*/
#define RSB_HAVE_TYPE_FLOAT  1 /*!< Type float is supported, so RSB_HAVE_TYPE_FLOAT  is defined .*/
#define RSB_HAVE_TYPE_FLOAT_COMPLEX  1 /*!< Type float complex is supported, so RSB_HAVE_TYPE_FLOAT_COMPLEX  is defined .*/
#define RSB_HAVE_TYPE_DOUBLE_COMPLEX  1 /*!< Type double complex is supported, so RSB_HAVE_TYPE_DOUBLE_COMPLEX  is defined .*/
#define RSB_DEFAULT_TYPE double	/*!< The default numerical matrix type (can be used for declarations), used in the example programs. */
#define RSB_DEFAULT_POSSIBLY_INTEGER_TYPE double/*!< The default, integer if possible , numerical type (can be used for declarations). */
#define RSB_DEFAULT_POSSIBLY_FIRST_BLAS_TYPE float  /*!< The default, blas if possible , numerical type (can be used for declarations). */
#define RSB_DEFAULT_TYPE_STRING "double"	/*!< A string specifying the name of the default type. */
#define RSB_DEFAULT_POSSIBLY_INTEGER_TYPE_STRING "double" /*!< A string specifying the name of the default possibly integer type.*/
#define RSB_DEFAULT_SYMMETRY RSB_SYMMETRY_U	/*!< The default symmetry flag. */
#define RSB_DEFAULT_TRANSPOSITION RSB_TRANSPOSITION_N	/*!< The default transposition flag (no transposition). */
#define RSB_ROWS_TRANSPOSITIONS_ARRAY	{RSB_TRANSPOSITION_N, RSB_TRANSPOSITION_T, RSB_TRANSPOSITION_C, RSB_INVALID_TRANS } /*!< An array with transposition constants. */

/*!  This preprocessor index can be used to address the double-related arrays.  */
#define RSB_TYPE_INDEX_DOUBLE  0
/*!  This preprocessor index can be used to address the float-related arrays.  */
#define RSB_TYPE_INDEX_FLOAT  1
/*!  This preprocessor index can be used to address the float complex-related arrays.  */
#define RSB_TYPE_INDEX_FLOAT_COMPLEX  2
/*!  This preprocessor index can be used to address the double complex-related arrays.  */
#define RSB_TYPE_INDEX_DOUBLE_COMPLEX  3

/* @cond INNERDOC  */
/*
   Each one of the following symbols is assigned to an operation which is supported
   by an option set at library code generation time.
   \n
   Other operations may be enabled by regenerating the whole library code.
   To enable operations, please read the documentation.
 */
#define RSB_HAVE_OPTYPE_SPMV_UAUA  1
#define RSB_HAVE_OPTYPE_SPMV_UAUZ  1
#define RSB_HAVE_OPTYPE_SPMV_UXUA  1
#define RSB_HAVE_OPTYPE_SPMV_UNUA  1
#define RSB_HAVE_OPTYPE_SPMV_SASA  1
#define RSB_HAVE_OPTYPE_SPSV_UXUA  1
#define RSB_HAVE_OPTYPE_SPMV_SXSA  1
#define RSB_HAVE_OPTYPE_SPSV_SXSX  1
#define RSB_HAVE_OPTYPE_INFTY_NORM  1
#define RSB_HAVE_OPTYPE_ROWSSUMS  1
#define RSB_HAVE_OPTYPE_SCALE  1

/*!
 * These preprocessor indices can be used to address various mop-related arrays.
 */
#define RSB_OPTYPE_INDEX_SPMV_UAUA  0
#define RSB_OPTYPE_INDEX_SPMV_UAUZ  1
#define RSB_OPTYPE_INDEX_SPMV_UXUA  2
#define RSB_OPTYPE_INDEX_SPMV_UNUA  3
#define RSB_OPTYPE_INDEX_SPMV_SASA  4
#define RSB_OPTYPE_INDEX_SPSV_UXUA  5
#define RSB_OPTYPE_INDEX_SPMV_SXSA  6
#define RSB_OPTYPE_INDEX_SPSV_SXSX  7
#define RSB_OPTYPE_INDEX_INFTY_NORM  8
#define RSB_OPTYPE_INDEX_ROWSSUMS  9
#define RSB_OPTYPE_INDEX_SCALE  10
#define RSB_OPTYPE_INDEX_MAT_STATS  11

/**
 \name Values for valid matrix coordinate index types flags.
 */
#define  RSB_COORDINATE_TYPE_C 0x01 /*!< Character code for type rsb_coo_idx_t.*/
#define  RSB_COORDINATE_TYPE_H 0x02 /*!< Character code for type rsb_half_idx_t.*/
/* @endcond */
/**
 \name Values for valid matrix transposition flags.
 \anchor matrix_transposition_flags_section
 The Hermitian flag will act as simple transposed, for non complex types.
 */
#define  RSB_TRANSPOSITION_N 0x4E /*!< N: Non transposed flag, valid for \ref rsb_trans_t typed variables. */
#define  RSB_TRANSPOSITION_T 0x54 /*!< T: Transposed flag value, valid for \ref rsb_trans_t valued variables. */
#define  RSB_TRANSPOSITION_C 0x43 /*!< C: Conjugated transpose flag, valid for \ref rsb_trans_t typed variables. */
/* @cond INNERDOC  */
/**
 \name Values for valid matrix symmetry flags.
 \anchor matrix_symmetry_flags_section
 */
#define  RSB_SYMMETRY_U 0x00 /*  */
#define  RSB_SYMMETRY_S RSB_FLAG_SYMMETRIC /*  */
#define  RSB_SYMMETRY_H RSB_FLAG_HERMITIAN /*  */
/* @endcond */
/**
\name Values for inner diagonal specification values.
 \anchor matrix_diagonal_flags_section
 */
/* @cond INNERDOC  */
#define  RSB_DIAGONAL_E 0x01 /*  */ /*!< */
#define  RSB_DIAGONAL_I 0x02 /*  */ /*!< */
/* @endcond INNERDOC  */
/* @cond INNERDOC  */
/**
 \name Values for valid matrix storage formats.
 \anchor matrix_storage_flags_section
 */
#define  RSB_MATRIX_STORAGE_BCOR 0x40 /* */
#define  RSB_MATRIX_STORAGE_BCSR 0x01 /*  */
/**
 \name Values for valid matrix storage formats strings.
 \anchor matrix_storage_strings_section
 */
#define  RSB_MATRIX_STORAGE_BCOR_STRING "BCOR"
#define  RSB_MATRIX_STORAGE_BCSR_STRING "BCSR"
/* @endcond */

/**
 \name Valid symbol values for matrix numerical type specification -- type codes -- (type \see #rsb_type_t).
 \anchor matrix_type_symbols_section
 */
#define RSB_NUMERICAL_TYPE_SAME_TYPE  1 /*!< a bogus type flag for specifying no type conversion */
#define  RSB_NUMERICAL_TYPE_DOUBLE  'D' /*!< Character code for type double. */
#define RSB_NUMERICAL_TYPE_SAME_TYPE  1 /*!< a bogus type flag for specifying no type conversion */
#define  RSB_NUMERICAL_TYPE_FLOAT  'S' /*!< Character code for type float. */
#define RSB_NUMERICAL_TYPE_SAME_TYPE  1 /*!< a bogus type flag for specifying no type conversion */
#define  RSB_NUMERICAL_TYPE_FLOAT_COMPLEX  'C' /*!< Character code for type float complex. */
#define RSB_NUMERICAL_TYPE_SAME_TYPE  1 /*!< a bogus type flag for specifying no type conversion */
#define  RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX  'Z' /*!< Character code for type double complex. */

#define  RSB_NUMERICAL_TYPE_FORTRAN_SAME_TYPE  1 /*!< a bogus type flag for specifying no type conversion */
#define  RSB_NUMERICAL_TYPE_FORTRAN_INT  ICHAR('I') /*!< Character code for type int, to be used (only) from Fortran. */
#define  RSB_NUMERICAL_TYPE_FORTRAN_DOUBLE  ICHAR('D') /*!< Character code for type double, to be used (only) from Fortran. */
#define  RSB_NUMERICAL_TYPE_FORTRAN_FLOAT  ICHAR('S') /*!< Character code for type float, to be used (only) from Fortran. */
#define  RSB_NUMERICAL_TYPE_FORTRAN_FLOAT_COMPLEX  ICHAR('C') /*!< Character code for type float complex, to be used (only) from Fortran. */
#define  RSB_NUMERICAL_TYPE_FORTRAN_DOUBLE_COMPLEX  ICHAR('Z') /*!< Character code for type double complex, to be used (only) from Fortran. */

#define  RSB_NUMERICAL_TYPE_DEFAULT   RSB_NUMERICAL_TYPE_DOUBLE   /*!< A default numerical matrix type. */
#define  RSB_NUMERICAL_TYPE_DEFAULT_INTEGER   RSB_NUMERICAL_TYPE_DOUBLE   /*!< A default numerical matrix type; if possible, an integer one. */
#define  RSB_NUMERICAL_TYPE_INVALID_TYPE  '?' /*!< By definition, an invalid type code. */
#define  RSB_NUMERICAL_TYPE_FIRST_BLAS   RSB_NUMERICAL_TYPE_FLOAT   /*!< A default numerical matrix type; if possible, not integer one. If no such type is configured in, then the invalid type. */

#define  RSB_CHAR_AS_TRANSPOSITION(TRANSC)	\
(														\
		(TRANSC) == ('N') ? (RSB_TRANSPOSITION_N) : 		\
		(TRANSC) == ('n') ? (RSB_TRANSPOSITION_N) : 		\
		(TRANSC) == ('T') ? (RSB_TRANSPOSITION_T) : 		\
		(TRANSC) == ('t') ? (RSB_TRANSPOSITION_T) : 		\
		(TRANSC) == ('C') ? (RSB_TRANSPOSITION_C) : 		\
		(TRANSC) == ('c') ? (RSB_TRANSPOSITION_C) : 		\
		'?'												\
) /*!< Get the right transposition flag out of either n, c, t chars. */


/**
 \name Miscellaneous constants.
 */
#define RSB_CONST_MAX_TUNING_ROUNDS 16 /*!< Maximal count of tuning rounds in one invocation of (rsb_tune_spmm/rsb_tune_spsm). */

/* @cond INNERDOC  */
/**
 \name Values for other numerical type related macros.
*/
#define  RSB_NUMERICAL_TYPE_PREPROCESSOR_SYMBOLS "D S C Z "

/* a bogus type for pattern input (TODO : should also implement ANY, just for matrix input) */
#define RSB_NUMERICAL_TYPE_PATTERN  0
/* @endcond */
/* @cond INNERDOC */

#define  RSB_MATRIX_STORAGE_DOUBLE_PRINTF_STRING "%.17g"
#define  RSB_MATRIX_STORAGE_FLOAT_PRINTF_STRING "%.9g"
#define  RSB_MATRIX_STORAGE_FLOAT_COMPLEX_PRINTF_STRING "%.9g %.9g"
#define  RSB_MATRIX_STORAGE_DOUBLE_COMPLEX_PRINTF_STRING "%.17g %.17g"



#if 1
 
#define RSB_ROWS_TRANSPOSITIONS_ARRAY_AS_CHAR	{'n', 't', 'c', RSB_INVALID_TRANS_CHAR }


#define  RSB_TRANSPOSITIONS_PREPROCESSOR_SYMBOLS "n t c "

#define RSB_TRANSPOSITION_AS_CHAR(TRANSA) 										\
(														\
		(TRANSA) == (RSB_TRANSPOSITION_N) ? ('N') : 		\
		(TRANSA) == (RSB_TRANSPOSITION_T) ? ('T') : 		\
		(TRANSA) == (RSB_TRANSPOSITION_C) ? ('C') : 		\
		'?'												\
)


#define RSB_NUMERICAL_TYPE_STRING(CSP,TYPE) \
		{ \
		switch(TYPE) \
		{ \
			/* supported (double,float,float complex,double complex) */ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:CSP="double";break; 	\
			case RSB_NUMERICAL_TYPE_FLOAT 	:CSP="float";break; 	\
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:CSP="float_complex";break; 	\
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:CSP="double_complex";break; 	\
			/* unsupported type */ \
			default : CSP="?"; \
		} \
		}



#define RSB_NUMERICAL_TYPE_SIZE(TYPE) \
	( (TYPE)==(RSB_NUMERICAL_TYPE_DOUBLE ) ?  sizeof(double) : \
	(( (TYPE)==(RSB_NUMERICAL_TYPE_FLOAT ) ?  sizeof(float) : \
	(( (TYPE)==(RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ) ?  sizeof(float complex) : \
	(( (TYPE)==(RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX ) ?  sizeof(double complex) : \
	(0  ) )  ) )  ) )  ) ) 

#define RSB_SIZEOF_BACKUP(TYPE) /* This is for rsb__pr_load. Please feed in upper case char codes (toupper(...)). */ \
    	( (TYPE)==(73) ?  4 : \
	(( (TYPE)==(68) ?  8 : \
	(( (TYPE)==(83) ?  4 : \
	(( (TYPE)==(67) ?  8 : \
	(( (TYPE)==(90) ?  16 : \
	(0  ) )  ) )  ) )  ) )  ) ) 

#define RSB_NUMERICAL_TYPE_REAL_TYPE(TYPE) \
	( (TYPE)==(RSB_NUMERICAL_TYPE_DOUBLE ) ?  RSB_NUMERICAL_TYPE_DOUBLE  : \
	(( (TYPE)==(RSB_NUMERICAL_TYPE_FLOAT ) ?  RSB_NUMERICAL_TYPE_FLOAT  : \
	(( (TYPE)==(RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ) ?  RSB_NUMERICAL_TYPE_FLOAT  : \
	(( (TYPE)==(RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX ) ?  RSB_NUMERICAL_TYPE_DOUBLE  : \
	(0  ) )  ) )  ) )  ) ) 

#define RSB_NUMERICAL_TYPE_CAST_TO_ANY_P(CTYPE,CVAR,TYPE,TP,TOFF) \
		{ \
		switch(TYPE) \
		{ \
			/* supported (double,float,float complex,double complex) */ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:\
				(CVAR)=(CTYPE)((double*)TP)[TOFF] ; break; 	\
			case RSB_NUMERICAL_TYPE_FLOAT 	:\
				(CVAR)=(CTYPE)((float*)TP)[TOFF] ; break; 	\
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:\
				(CVAR)=(CTYPE)((float complex*)TP)[TOFF] ; break; 	\
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:\
				(CVAR)=(CTYPE)((double complex*)TP)[TOFF] ; break; 	\
			/* unsupported type */ \
			default : ; \
		} \
		}

/* *A += abs(*B) */
#define RSB_NUMERICAL_TYPE_ABS_SUM_AND_STORE_ELEMENTS(A,B,TYPE) \
		{ \
		switch(TYPE) \
		{ \
			/* supported (double,float,float complex,double complex) */ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:	*(double*)(A)+= (	\
				*(double*)(B) < (double)(0) ? - *(double*)(B) : *(double*)(B) ); break; 	\
			case RSB_NUMERICAL_TYPE_FLOAT 	:	*(float*)(A)+= (	\
				*(float*)(B) < (float)(0) ? - *(float*)(B) : *(float*)(B) ); break; 	\
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:	*(float complex*)(A)+= (	\
				*(float complex*)(B) < (float complex)(0) ? - *(float complex*)(B) : *(float complex*)(B) ); break; 	\
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:	*(double complex*)(A)+= (	\
				*(double complex*)(B) < (double complex)(0) ? - *(double complex*)(B) : *(double complex*)(B) ); break; 	\
			/* unsupported type */ \
			default : ; \
		} \
		}

/* *A += *B */
#define RSB_NUMERICAL_TYPE_SUM_AND_STORE_ELEMENTS(A,B,TYPE) \
		{ \
		switch(TYPE) \
		{ \
			/* supported (double,float,float complex,double complex) */ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:	*(double*)(A)+=*(double*)(B); break; \
			case RSB_NUMERICAL_TYPE_FLOAT 	:	*(float*)(A)+=*(float*)(B); break; \
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:	*(float complex*)(A)+=*(float complex*)(B); break; \
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:	*(double complex*)(A)+=*(double complex*)(B); break; \
			/* unsupported type */ \
			default : ; \
		} \
		}

#define RSB_NUMERICAL_TYPE_SET_ELEMENT(DST,SRC,TYPE) \
		{ \
		switch(TYPE) \
		{ \
			/* supported (double,float,float complex,double complex) */ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:	*(double*)(DST)=*(double*)(SRC); break; \
			case RSB_NUMERICAL_TYPE_FLOAT 	:	*(float*)(DST)=*(float*)(SRC); break; \
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:	*(float complex*)(DST)=*(float complex*)(SRC); break; \
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:	*(double complex*)(DST)=*(double complex*)(SRC); break; \
			/* unsupported type */ \
			default : ; \
		} \
		}

#define RSB_NUMERICAL_TYPE_SET_ELEMENT_REAL(DST,SRC,TYPE) \
		{ \
		switch(TYPE) \
		{ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:	*(double*)(DST)=(*(double*)(SRC)); break; \
			case RSB_NUMERICAL_TYPE_FLOAT 	:	*(float*)(DST)=(*(float*)(SRC)); break; \
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:	*(float*)(DST)=crealf(*(float complex*)(SRC)); break; \
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:	*(double*)(DST)=creal(*(double complex*)(SRC)); break; \
			/* unsupported type */ \
			default : ; \
		} \
		}

#define RSB_NUMERICAL_TYPE_SET_ELEMENT_FROM_DOUBLE(DST,DSRC,TYPE) \
		{ \
		switch(TYPE) \
		{ \
			/* supported (double,float,float complex,double complex) */ \
			case RSB_NUMERICAL_TYPE_DOUBLE 	:	*(double*)(DST)=(double)(DSRC); break; \
			case RSB_NUMERICAL_TYPE_FLOAT 	:	*(float*)(DST)=(float)(DSRC); break; \
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:	*(float complex*)(DST)=(float complex)(DSRC); break; \
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:	*(double complex*)(DST)=(double complex)(DSRC); break; \
			/* unsupported type */ \
			default : ; \
		} \
		}

/* CODE NOT DEBUGGED */
#define RSB_VECTOR_FIND_MAXIMAL_ELEMENT(INDEX,ARRAY,ELEMENTS,TYPE) 								\
		{ 													\
		int _index;												\
		switch(TYPE) 												\
		{ 													\
			/* supported (double,float,float complex,double complex) */ 									\
			case RSB_NUMERICAL_TYPE_DOUBLE 	:						\
			{												\
				double * _array = (double*)(ARRAY);								\
				double _maxel=(double)(0);									\
				int  _maxindex=0;									\
				_maxel=_maxel-_maxel;	/* could this be evil ? */					\
				for(_index=0;_index<(ELEMENTS);++_index)						\
					if(fabs(_maxel)<fabs(_array[_index])){_maxel=_array[_index];_maxindex=_index;}	\
					(INDEX)=_maxindex;								\
			}												\
			break;			\
			case RSB_NUMERICAL_TYPE_FLOAT 	:						\
			{												\
				float * _array = (float*)(ARRAY);								\
				float _maxel=(float)(0);									\
				int  _maxindex=0;									\
				_maxel=_maxel-_maxel;	/* could this be evil ? */					\
				for(_index=0;_index<(ELEMENTS);++_index)						\
					if(fabsf(_maxel)<fabsf(_array[_index])){_maxel=_array[_index];_maxindex=_index;}	\
					(INDEX)=_maxindex;								\
			}												\
			break;			\
			case RSB_NUMERICAL_TYPE_FLOAT_COMPLEX 	:						\
			{												\
				float complex * _array = (float complex*)(ARRAY);								\
				float complex _maxel=(float complex)(0);									\
				int  _maxindex=0;									\
				_maxel=_maxel-_maxel;	/* could this be evil ? */					\
				for(_index=0;_index<(ELEMENTS);++_index)						\
					if(cabsf(_maxel)<cabsf(_array[_index])){_maxel=_array[_index];_maxindex=_index;}	\
					(INDEX)=_maxindex;								\
			}												\
			break;			\
			case RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX 	:						\
			{												\
				double complex * _array = (double complex*)(ARRAY);								\
				double complex _maxel=(double complex)(0);									\
				int  _maxindex=0;									\
				_maxel=_maxel-_maxel;	/* could this be evil ? */					\
				for(_index=0;_index<(ELEMENTS);++_index)						\
					if(cabs(_maxel)<cabs(_array[_index])){_maxel=_array[_index];_maxindex=_index;}	\
					(INDEX)=_maxindex;								\
			}												\
			break;			\
			/* unsupported type */ \
			default :  (INDEX)=-1; \
		} \
		}

#define RSB_NUMERICAL_OP_INDEX_FROM_CODE(CODE) 								\
( ((CODE)==RSB_OPTYPE_INDEX_SPMV_UAUA )?(0):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPMV_UAUZ )?(1):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPMV_UXUA )?(2):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPMV_UNUA )?(3):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPMV_SASA )?(4):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPSV_UXUA )?(5):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPMV_SXSA )?(6):			\
( ((CODE)==RSB_OPTYPE_INDEX_SPSV_SXSX )?(7):			\
( ((CODE)==RSB_OPTYPE_INDEX_INFTY_NORM )?(8):			\
( ((CODE)==RSB_OPTYPE_INDEX_ROWSSUMS )?(9):			\
( ((CODE)==RSB_OPTYPE_INDEX_SCALE )?(10):			\
( ((CODE)==RSB_OPTYPE_INDEX_MAT_STATS )?(11):			\
-1 ) \
) \
) \
) \
) \
) \
) \
) \
) \
) \
) \
) \
/* uhm. does it seem redundant ? */
#define RSB_NUMERICAL_TYPE_INDEX_FROM_CODE(CODE) 								\
( ((CODE)==RSB_NUMERICAL_TYPE_DOUBLE )?(0):			\
( ((CODE)==RSB_NUMERICAL_TYPE_FLOAT )?(1):			\
( ((CODE)==RSB_NUMERICAL_TYPE_FLOAT_COMPLEX )?(2):			\
( ((CODE)==RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX )?(3):			\
-1 ) \
) \
) \
) \
/* uhm. seems redundant ? */


#define RSB_IS_ELEMENT_MINUS_ONE(SRC,TYPE) 										\
(														\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE ) ? (*(double*)(SRC)==(double)(-1)) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT ) ? (*(float*)(SRC)==(float)(-1)) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ) ? (*(float complex*)(SRC)==(float complex)(-1)) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX ) ? (*(double complex*)(SRC)==(double complex)(-1)) : 		\
		0												\
)

#define RSB_IS_ELEMENT_ONE(SRC,TYPE) 										\
(														\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE ) ? (*(double*)(SRC)==(double)1) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT ) ? (*(float*)(SRC)==(float)1) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ) ? (*(float complex*)(SRC)==(float complex)1) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX ) ? (*(double complex*)(SRC)==(double complex)1) : 		\
		0												\
)

#define RSB_IS_ELEMENT_ZERO(SRC,TYPE) 										\
(														\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE ) ? (*(double*)(SRC)==(double)0) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT ) ? (*(float*)(SRC)==(float)0) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ) ? (*(float complex*)(SRC)==(float complex)0) : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX ) ? (*(double complex*)(SRC)==(double complex)0) : 		\
		0												\
)

#define RSB_IS_ELEMENT_NONZERO(SRC,TYPE) 		(!(RSB_IS_ELEMENT_ZERO(SRC,TYPE)))

#define RSB_MATRIX_UNSUPPORTED_TYPE(TYPE) ( \
			(TYPE)!=RSB_NUMERICAL_TYPE_DOUBLE  && \
			(TYPE)!=RSB_NUMERICAL_TYPE_FLOAT  && \
			(TYPE)!=RSB_NUMERICAL_TYPE_FLOAT_COMPLEX  && \
			(TYPE)!=RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX  && \
			1 )

#define RSB_IS_MATRIX_TYPE_COMPLEX(TYPE) 										\
(														\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE ) ? 0 : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT ) ? 0 : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ) ? 1 : 		\
		(TYPE) == (RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX ) ? 1 : 		\
		0												\
)

#define RSB_IS_ELEMENT_LESS_THAN(SRC,CMPSRC,TYPE) \
( 			( (TYPE)==RSB_NUMERICAL_TYPE_DOUBLE  && (*(double*)(SRC))<(*(double*)(CMPSRC)) ) || \
			( (TYPE)==RSB_NUMERICAL_TYPE_FLOAT  && (*(float*)(SRC))<(*(float*)(CMPSRC)) ) || \
			( (TYPE)==RSB_NUMERICAL_TYPE_FLOAT_COMPLEX  && crealf(*(float complex*)(SRC))<crealf(*(float complex*)(CMPSRC)) ) || \
			( (TYPE)==RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX  && creal(*(double complex*)(SRC))<creal(*(double complex*)(CMPSRC)) ) || \
			0 )


/** use RSB_MAXIMAL_CONFIGURED_BLOCK_SIZE to oversize your arrays safely */
#define RSB_MAXIMAL_CONFIGURED_BLOCK_SIZE	 1 
/** use RSB_MAXIMAL_CONFIGURED_BLOCK_SIZE_EXTRA to oversize your arrays safely */
#define RSB_MAXIMAL_CONFIGURED_BLOCK_SIZE_EXTRA	 (1-1) 
#define RSB_CONST_MATRIX_IMPLEMENTATION_CODE_STRING_MAX_LENGTH (2*1024)	/** chars to reserve for a matrix implementation code */

/* Section dedicated to implemented operations on matrices. */



#define RSB_ROWS_UNROLL_ARRAY		{ 1 }
#define RSB_COLUMNS_UNROLL_ARRAY	{ 1 }


#define RSB_ROWS_UNROLL_ARRAY_LENGTH		1
#define RSB_COLUMNS_UNROLL_ARRAY_LENGTH		1
#define RSB_IMPLEMENTED_META_MOPS		12
#define RSB_IMPLEMENTED_MOPS		11
#define RSB_IMPLEMENTED_TYPES		4
#define RSB_IMPLEMENTED_SOME_BLAS_TYPES		1

#define RSB_MATRIX_OPS_ARRAY	{ "spmv_uaua","spmv_uauz","spmv_uxua","spmv_unua","spmv_sasa","spsv_uxua","spmv_sxsa","spsv_sxsx","infty_norm","rowssums","scale","mat_stats" }
#define RSB_MATRIX_TYPES_ARRAY	{ "double","float","float complex","double complex", }
#define RSB_MATRIX_TYPE_CODES_ARRAY	{ RSB_NUMERICAL_TYPE_DOUBLE ,RSB_NUMERICAL_TYPE_FLOAT ,RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ,RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX , }
#define RSB_MATRIX_SPBLAS_TYPE_CODES_ARRAY	{ RSB_NUMERICAL_TYPE_FLOAT ,RSB_NUMERICAL_TYPE_DOUBLE ,RSB_NUMERICAL_TYPE_FLOAT_COMPLEX ,RSB_NUMERICAL_TYPE_DOUBLE_COMPLEX , }

#define RSB_M4_MATRIX_META_OPS_STRING	"spmv_uaua,spmv_uauz,spmv_uxua,spmv_unua,spmv_sasa,spsv_uxua,spmv_sxsa,spsv_sxsx,infty_norm,rowssums,scale"
#define RSB_M4_MATRIX_TYPES_STRING		"double,float,float complex,double complex"
#define RSB_M4_WANT_COLUMN_UNLOOP_FACTORS_STRING		"1"
#define RSB_M4_WANT_ROW_UNLOOP_FACTORS_STRING		"1"

/**
 \name Macro to check matrix storage flags correctness
 */
#define  RSB_IS_MATRIX_STORAGE_ALLOWED_FOR_LEAF(MATRIX_STORAGE)	(( \
	((MATRIX_STORAGE)==RSB_MATRIX_STORAGE_BCOR) || \
	((MATRIX_STORAGE)==RSB_MATRIX_STORAGE_BCSR) || \
	0 ) ? RSB_BOOL_TRUE:RSB_BOOL_FALSE )

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
#endif /* RSB_TYPES_H_INCLUDED */
/* @endcond */
