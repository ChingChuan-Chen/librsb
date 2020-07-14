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
 * @brief A Matrix Market files oriented `ls' program.
 */
#include "rsb_common.h"
#include "rsb_internals.h"
int rsb_mtx_ls_main(const int argc, char * argv[])
{
#if 0
	rsb_option options[] = {
	    {"matrix-ls",		no_argument, NULL,  0x006D6C73},/* should be synced to rsb_mtx_ls_main */
	    {0,0,0,0}
	};
	int opt_index = 0;
	const char * flags="";
	int c;
    	for (;;)
	{
		c = rsb_getopt_long(argc, argv, flags , options, &opt_index);
		if (c == -1)break;

		switch (c)
		{
			case 0x006D6C73:
				++a0;
			default:
			break;
	    	}
	}
#endif
	rsb_bool_t want_latex = RSB_BOOL_TRUE;
	int a,a0=1;

	if(want_latex)
		RSB_STDOUT(
			"\\begin{table}[]"
			"\\begin{footnotesize}"
			"\\begin{center} \\begin{tabular}"
//			"{l@{\extracolsep{.5em}}l@{\extracolsep{.5em}}l@{\extracolsep{.5em}}l@{\extracolsep{.5em}}l@{\extracolsep{.5em}}l}\hline"
			"{lllll}\\hline\n"
			"matrix & rows & columns & nnz & nnz/row \\\\\\hline\n"
			  );

	for(a=a0;a<argc;++a)
	if(argv[a][0]!='-')
	{
		const char * filename = argv[a];
		rsb_bool_t is_symmetric = RSB_BOOL_FALSE;
		rsb_bool_t is_hermitian = RSB_BOOL_FALSE;
		rsb_bool_t is_pattern = RSB_BOOL_FALSE;
		rsb_bool_t is_lower = RSB_BOOL_FALSE;
		rsb_bool_t is_upper = RSB_BOOL_FALSE;
		rsb_bool_t is_vector = RSB_BOOL_FALSE;
		rsb_nnz_idx_t nnz;
		rsb_coo_idx_t m,k;
		rsb_type_t typecode = RSB_NUMERICAL_TYPE_INVALID_TYPE;

		if(RSB_SOME_ERROR(rsb__util_mm_info_matrix_f(filename,&m,&k,&nnz,&typecode,&is_symmetric,&is_hermitian,&is_pattern,&is_lower,&is_upper,&is_vector)) || is_vector)
			RSB_STDERR("problems with \"%s\"\n",filename);
		else
		{
			if(want_latex)
			RSB_STDOUT("%s & %zd & %zd & %zd & %.0lf"
				"\\\\%s\n"
				//,filename
				,rsb__basename(filename)
				,(size_t)m,(size_t)k,(size_t)nnz
				,((double)nnz)/m
				,is_symmetric?"%%symm":"%%unsymm"
			);
			else
			RSB_STDOUT("%s\t%zd\t%zd\t%zd"
				"\t%s\t%s\t%s\n"
				,rsb__basename(filename)
				,(size_t)m,(size_t)k,(size_t)nnz
				,is_pattern?  "pattern":""
				,is_symmetric?"symmetric":""
				,is_hermitian?"hermitian":""
			);
		};
	}
	if(want_latex)
		RSB_STDOUT(
			"\\hline \\end{tabular} \\caption{Caption.}"
			"\\label{testbed_matrices}"
			"\\end{center}"
			"\\end{footnotesize}"
			"\\end{table}\n"
			);


	return 0;
}

/*
int main(const int argc, char * const argv[])
{
	return rsb_mtx_ls_main(argc,argv);
}
*/

/* @endcond */
