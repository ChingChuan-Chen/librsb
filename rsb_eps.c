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
 * This source file contains postscript rendering functions.
 * */

#include "rsb_internals.h"

RSB_INTERNALS_COMMON_HEAD_DECLS

typedef float rsb_rf_t; /* real float */
#define RSB_RR_LEVELS 16

#define RSB_PRINTF_MATRIX_TIME_ARGS(MTXAP)  \
					"ect: %5.2le""  "	\
					"est: %5.2le""  "	\
					"sat: %5.2le""  "	\
					"eit: %5.2le""  "	\
					"cpt: %5.2le"	\
					, \
					(MTXAP)->ect, (MTXAP)->est, (MTXAP)->sat, (MTXAP)->eit, (MTXAP)->cpt

#define RSB_EPS_TRSL(DX,DY,SX,SY,XS,YS,MTXAP) DX = XS*(/*(MTXAP)->nc-*/(SX)); DY = YS*((MTXAP)->nr-(SY)); /* translate for EPS */
#define RSB_MTX_EFF_R(MTXAP) (MTXAP->bm-((MTXAP)->broff-(MTXAP)->roff)) /* effective rows count */
#define RSB_MTX_EFF_C(MTXAP) (MTXAP->bk-((MTXAP)->bcoff-(MTXAP)->coff)) /* effective columns count */
#define RSB_MTX_LAR(MTXAP) ((MTXAP)->roff+(MTXAP)->bm) /* last absolute row */
#define RSB_MTX_LAC(MTXAP) ((MTXAP)->coff+(MTXAP)->bk) /* last absolute column */
#define RSB_MTX_LER(MTXAP) ((MTXAP)->broff-(MTXAP)->roff) /* local empty rows */
#define RSB_MTX_LEC(MTXAP) ((MTXAP)->bcoff-(MTXAP)->coff) /* local empty columns */
#define RSB_EPS_NEWPATH "N" /* newpath */
#define RSB_EPS_MOVETO "M" /* moveto */
#define RSB_EPS_LINETO "L" /* lineto */
#define RSB_EPS_RLINETO "R" /* rlineto */
#define RSB_EPS_SCALEFONT "SCF" /* scalefont */
#define RSB_EPS_SETFONT "SF" /* setfont */
#define RSB_EPS_SETRGB "SRGB" /* setrgbcolor */
#define RSB_EPS_SETLINEWIDTH "SLW" /* setlinewidth */
#define RSB_EPS_CLOSEPATH "C" /* closepath */

static rsb_err_t render_ps_box(FILE*fd, int r0, int c0, int dr, int dc, rsb_coo_idx_t orows, rsb_rf_t xs, rsb_rf_t ys, rsb_rf_t r, rsb_rf_t g, rsb_rf_t b)
{
	/**
	   \ingroup gr_internals
	   Prints out a box in the postscript language.

	   \param r0 the box base row
	   \param c0 the box base column
	   \param dr the box height
	   \param dc the box width
	   \param orows the box offset row
	   \param xs the scale on x (rows)
	   \param ys the scale on y (column)
	   \param r red value
	   \param g green value
	   \param b blue value
	 */
#if RSB_ALLOW_STDOUT
		RSB_FPRINTF(fd,
			"newpath\n"
			"%g %g "RSB_EPS_MOVETO"\n"
			"%g %d "RSB_EPS_RLINETO"\n"
			"%d %g "RSB_EPS_RLINETO"\n"
			"%g %d "RSB_EPS_RLINETO"\n"
			"%d %d "RSB_EPS_RLINETO"\n"
			RSB_EPS_CLOSEPATH"\n"
			"%g %g %g "RSB_EPS_SETRGB"\n"
			"1 "RSB_EPS_SETLINEWIDTH"\n"
			"stroke\n\n"
			,
			c0*xs,(orows-r0)*ys,
			 (dc)*xs,  0,
			0,  -(dr)*ys,
			-(dc)*xs, 0,
/*			c0*xs,(orows-r0)*ys,
			 (submatrix->nc)*xs,  0,
			0,  -(submatrix->nr)*ys,
			-(submatrix->nc)*xs, 0,*/
			0, 0,
			r, g, b
			);
	return RSB_ERR_NO_ERROR;
#else /* RSB_ALLOW_STDOUT */
	return RSB_ERR_UNSUPPORTED_FEATURE;
#endif /* RSB_ALLOW_STDOUT */
}

static rsb_err_t rsb_dump_postscript_z_curve(FILE*fd, const struct rsb_mtx_t * mtxAp, rsb_coo_idx_t roff, rsb_coo_idx_t coff, rsb_rf_t xs, rsb_rf_t ys, rsb_coo_idx_t orows, int level,int * p, int * pv)
{
	/**
	   \ingroup gr_internals
	 */
#if RSB_ALLOW_STDOUT
	rsb_submatrix_idx_t i,j;
	struct rsb_mtx_t * submatrix=NULL;
	const int levels = RSB_RR_LEVELS;
	const int want_eb=0;/* effective boundaries (FIXME: option currently unfinished) */

	if(!mtxAp)
	{
		goto err;
	}

	if(level>=levels-1)
		level=levels;

#if 1
	if(pv)
	{
		rsb_submatrix_idx_t smi=0;
		RSB_SUBMATRIX_FOREACH_LEAF_PERMUTED(mtxAp,submatrix,smi,pv)
		//RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi)
		{
			rsb_dump_postscript_z_curve(fd,submatrix,submatrix->roff,submatrix->coff,xs,ys,orows,level+1,p,NULL);

			if(smi<mtxAp->all_leaf_matrices_n-1)
			{
				rsb_rf_t fcoff=(rsb_rf_t)submatrix->coff;
				rsb_rf_t froff=(rsb_rf_t)submatrix->roff;
				rsb_rf_t fnc=(rsb_rf_t)submatrix->nc;
				rsb_rf_t fnr=(rsb_rf_t)submatrix->nr;
				rsb_rf_t shade= .8 - (.8*smi)/(mtxAp->all_leaf_matrices_n);

				if(want_eb)
				{
					fcoff=(rsb_rf_t)submatrix->bcoff;
					froff=(rsb_rf_t)submatrix->broff;
					fnc=(rsb_rf_t)submatrix->bm;
					fnr=(rsb_rf_t)submatrix->bk;
				}

				RSB_FPRINTF(fd,"%g %g %g "RSB_EPS_SETRGB" 1 "RSB_EPS_SETLINEWIDTH" stroke "RSB_EPS_NEWPATH" %g %g "RSB_EPS_MOVETO"\n",
				shade,shade,1.0, (fcoff*xs+fnc*(xs/2)), ((-froff+orows))*ys-(fnr)*(ys/2));
			}

		}
		goto ret;
	}
#endif

	if(rsb__is_terminal_recursive_matrix(mtxAp))
	{
		rsb_coo_idx_t scoff=coff;
		rsb_coo_idx_t sroff=roff;
		rsb_coo_idx_t snc=mtxAp->nc;
		rsb_coo_idx_t snr=mtxAp->nr;
		const int want_sc = 1;/* want submatrix comments */

		if(want_eb)
		{
			sroff=roff+RSB_MTX_LER(mtxAp);
			scoff=coff+RSB_MTX_LEC(mtxAp);
			snr=mtxAp->bm;
			snc=mtxAp->bk;
		}

		if(want_sc)
		RSB_FPRINTF(fd,"%% matrix at %d %d, level %d, xs %g, ys %g, orows %d\n",sroff,scoff,level,xs,ys,orows);
		if(*p>0)
			RSB_FPRINTF(fd, "%g %g "RSB_EPS_LINETO"\n" , scoff*xs+snc*(xs/2), ((rsb_rf_t)(orows-sroff))*ys-((rsb_rf_t)snr)*(ys/2));
		else
			RSB_FPRINTF(fd, "%g %g "RSB_EPS_MOVETO"\n" , scoff*xs+snc*(xs/2), ((rsb_rf_t)(orows-sroff))*ys-((rsb_rf_t)snr)*(ys/2));
		++*p;
	}
	else
	{
		RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
		if(submatrix)
		{
//			rsb_coo_idx_t scoff=submatrix->coff;
//			rsb_coo_idx_t sroff=submatrix->roff;
			rsb_coo_idx_t snc=submatrix->nc;
			rsb_coo_idx_t snr=submatrix->nr;

			if(0)
			if(want_eb)
			{
		//		scoff=submatrix->bcoff;
		//		sroff=submatrix->broff;
		//		snr=submatrix->bm;
		//		snc=submatrix->bk;
			}

			rsb_dump_postscript_z_curve(fd,submatrix, roff+(i?(mtxAp->nr-snr):0), coff+(j?mtxAp->nc-snc:0),xs,ys,orows, level+1,p,NULL);
		}
	}
ret:
	return RSB_ERR_NO_ERROR;
err:
	return RSB_ERR_GENERIC_ERROR;
#else /* RSB_ALLOW_STDOUT */
	return RSB_ERR_UNSUPPORTED_FEATURE;
#endif /* RSB_ALLOW_STDOUT */
}

static rsb_err_t rsb__dump_postscript_ussv_order_curve(const struct rsb_mtx_t * mtxAp, rsb_rf_t xs, rsb_rf_t ys, int * p)
{
	/**
	   \ingroup gr_internals
	   NEW, EXPERIMENTAL
	 */
#if RSB_ALLOW_STDOUT
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	struct rsb_translated_matrix_t * all_leaf_matrices=NULL;
	rsb_submatrix_idx_t all_leaf_matrices_n=0,n;
	FILE*fd = RSB_DEFAULT_FD;

	if(!mtxAp)
	{
		goto err;
	}

	errval = rsb__do_get_submatrices_for_ussv(mtxAp,&all_leaf_matrices,&all_leaf_matrices_n,RSB_TRANSPOSITION_N);
	if(!all_leaf_matrices || RSB_SOME_ERROR(errval))
	{
		errval = RSB_ERR_ENOMEM;
	       	RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

	if(RSB_SOME_ERROR(errval))
	{ RSB_PERR_GOTO(err,RSB_ERRM_ES) }

	RSB_FPRINTF(fd,"%%%%there are %d leaves, %d for ussv\n",mtxAp->all_leaf_matrices_n,all_leaf_matrices_n);
#if 1
	for(n=0;n<all_leaf_matrices_n;++n)
	{
		rsb_coo_idx_t rows=all_leaf_matrices[n].nr;
		rsb_coo_idx_t cols=all_leaf_matrices[n].nc;
		rsb_coo_idx_t roff=all_leaf_matrices[n].roff;
		rsb_coo_idx_t coff=all_leaf_matrices[n].coff;
		rsb_coo_idx_t my=mtxAp->nr-((roff+rows/2));
		rsb_coo_idx_t mx=(coff+cols/2);
		rsb_rf_t mys=my;
		rsb_rf_t mxs=mx;
		mys*=ys/mtxAp->nc;
		mxs*=xs/mtxAp->nr;
//		my/=mtxAp->cols;
		RSB_FPRINTF(fd,"%% matrix %d at %d %d, %d x %d \n",n,roff,coff,rows,cols);
		if(*p>0)
			RSB_FPRINTF(fd, "%g %g "RSB_EPS_LINETO"\n" , mxs, mys);
		else
			RSB_FPRINTF(fd, "%g %g "RSB_EPS_MOVETO"\n" , mxs, mys);
		++*p;
	}
#endif
err:
	RSB_CONDITIONAL_FREE(all_leaf_matrices);
	RSB_DO_ERR_RETURN(errval)
#else /* RSB_ALLOW_STDOUT */
	RSB_DO_ERR_RETURN(RSB_ERR_UNSUPPORTED_FEATURE)
#endif /* RSB_ALLOW_STDOUT */
}

int rsb_dump_postscript(const int argc, char * const argv[])
{
	/**
	   \ingroup gr_internals
	*/
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_option options[] = {
	    {"matrix-filename",	required_argument, NULL, 0x66},/* f */  
	    {"dump-recursion",	no_argument, NULL, 'd'},/* NEW */
	    {"width",required_argument	, NULL, 0x5757},/* W */
	    {"height",required_argument	, NULL, 0x4848}, /* H */
	    {"auto-size",no_argument	, NULL, 'a'},
	    {"block-dump",no_argument	, NULL, 'B'},
	    {"nonzeros-dump",no_argument, NULL, 'N'},
	    {"block-rowsize",	required_argument, NULL, 0x72 },/* r */
	    {"block-columnsize",	required_argument, NULL, 0x63},/* c */  
	    {"z-dump",	no_argument, NULL, 'z'},
	    {"ussv-dump",	no_argument, NULL, 'S'},
	    RSB_BENCH_PROG_OPTS
	    {0,0,0,0}
	};

#ifdef RSB_NUMERICAL_TYPE_FLOAT
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_DOUBLE ;
#else /* RSB_NUMERICAL_TYPE_FLOAT */
#ifdef RSB_NUMERICAL_TYPE_DOUBLE
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_DOUBLE ;
#else /* RSB_NUMERICAL_TYPE_DOUBLE */
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_DEFAULT;
#endif /* RSB_NUMERICAL_TYPE_DOUBLE */
#endif /* RSB_NUMERICAL_TYPE_FLOAT */
	rsb_blk_idx_t br=1;
	rsb_blk_idx_t bc=1;

	const char * filename=NULL;
	int c,w = RSB_DEFAULT_MATRIX_RENDERING_COLS,h = RSB_DEFAULT_MATRIX_RENDERING_ROWS;
	int opt_index = 0;
	int dump_recursion=0;
	int g_auto_size=0;
	rsb_bool_t want_blocks=0,want_nonzeros=0,z_dump=0;

	rsb_flags_t flags = RSB_FLAG_NOFLAGS;

	if((errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS))!=RSB_ERR_NO_ERROR)
	{
		RSB_PERR_GOTO(err,RSB_ERRM_ES);
	}

    	for (;;)
	{
		c = rsb_getopt_long(argc,argv,RSB_SAMPLE_PROGRAM_OPTIONS_GET_FLAGS"ar:c:df:BNzSn:"/*"W:H:"*/,options,&opt_index);
		if (c == -1)
			break;
		RSB_DO_FLAG_ADD(flags,rsb__sample_program_options_get_flags(c,optarg));	/* FIXME : NEW */
		switch (c)
		{
			case 'r':
			br = rsb__util_atoi(optarg);
			if(br<1) { errval = RSB_ERR_BADARGS; RSB_PERR_GOTO(err,RSB_ERRM_ES); }
			break;
			case 'c':
			bc = rsb__util_atoi(optarg);
			if(br<1) { errval = RSB_ERR_BADARGS; RSB_PERR_GOTO(err,RSB_ERRM_ES); }
			break;
			case 'f':
			filename = optarg;
			break;
			case 'N':
			want_nonzeros=1;
			break;
			case 'B':
			want_blocks=1;
			break;
			case 'a':
			g_auto_size=1;
			break;
			case 'd':
			dump_recursion=1;
			break;
			case 'S':
			z_dump=2;
			RSB_DO_FLAG_ADD(flags,RSB_FLAG_LOWER_TRIANGULAR);
			break;
			case 'z':
			z_dump=1;
			break;
 			case 0x4848:
			h = rsb__util_atoi(optarg);
			if(h<1) { errval = RSB_ERR_BADARGS; RSB_PERR_GOTO(err,RSB_ERRM_ES); }
			break;
			case 0x5757:
			w = rsb__util_atoi(optarg);
			if(w<1) { errval = RSB_ERR_BADARGS; RSB_PERR_GOTO(err,RSB_ERRM_ES); }
			break;
			case 'T':
			typecode = toupper(*optarg);
			break;
			case 'n':
#if 1
			rsb__set_num_threads(rsb__util_atoi(optarg));
#else
			{
				rsb_thread_t ca_[1]={1};
				rsb_thread_t * ca=ca_;
				rsb_thread_t cn=1,ci=0;
				ca=NULL;cn=0;
				if(RSB_SOME_ERROR(errval = rsb__util_get_bx_array(optarg,&cn,&ca)))
				{
				       	RSB_PERR_GOTO(err,RSB_ERRM_ES); 
				}
			}
#endif
			default:
			;
	    	}
	}
	
	if(!filename)
	{
		const char*usagestring=" -aRzd -f pd.mtx";
		//errval = RSB_ERR_BADARGS;
		RSB_INFO("Did not specify a matrix file.\n");
		RSB_INFO("Usage example: %s %s\n",argv[0],usagestring);
		//RSB_ERROR(RSB_ERRM_ES);
		goto err;
	}

	RSB_DO_FLAG_DEL(flags,RSB_FLAG_EXPERIMENTAL_IN_PLACE_CSR); /* FIXME : breaks  -r1 -c1 -Fbr -aRzD */
	if(g_auto_size)
	{
		/* rescale smartly to reflect the desired area and keep proportions (but lose both dimensions) */
		size_t cols,rows;
		rsb_rf_t area=1,p;
		rsb_flags_t flags = RSB_FLAG_NOFLAGS;

		if(RSB_SOME_ERROR(rsb__do_util_get_matrix_dimensions(filename, &cols, &rows, NULL, &flags)))
			goto err;

		area*=w;
		area*=h;
		p=((rsb_rf_t)cols)/((rsb_rf_t)rows);
		h=sqrt(area/p);
		w=h*p;
	}

	if(!dump_recursion)
		want_nonzeros = 1;

	errval = rsb__dump_postscript_recursion_from_matrix(filename,br,bc,w,h,flags,want_blocks,z_dump,want_nonzeros,dump_recursion,typecode);
err:
	RSB_MASK_OUT_SOME_ERRORS(errval)
	rsb__do_perror(NULL,errval);
	return RSB_ERR_TO_PROGRAM_ERROR(errval);
}

static rsb_err_t rsb__dump_block_rectangles(FILE*fd, const struct rsb_mtx_t * mtxAp, rsb_coo_idx_t roff, rsb_coo_idx_t coff, rsb_rf_t xs, rsb_rf_t ys, rsb_coo_idx_t orows, int level)
{
	/**
	 */
#if RSB_ALLOW_STDOUT
	rsb_submatrix_idx_t i,j;
	struct rsb_mtx_t * submatrix = NULL;
	const int levels = RSB_RR_LEVELS;
	const int want_eb = 0;/* want effective boundaries (working) */
	const int want_sc = 1;/* want submatrix comments */
	rsb_rf_t shade;

	if(!mtxAp)
	{
		goto err;
	}

	if(level>=levels-1)
		level=levels;
	shade = 1.0*(level)/levels;

	if(want_sc)RSB_FPRINTF(fd,"%% matrix at %d %d, level %d\n",roff,coff,level);
	if(rsb__is_terminal_recursive_matrix(mtxAp))
	{
		rsb_coo_idx_t eroff=mtxAp->broff,ecoff=mtxAp->bcoff;
		rsb_coo_idx_t er=RSB_MTX_EFF_R(mtxAp),ec=RSB_MTX_EFF_C(mtxAp);
		if(want_eb==0)
			eroff=mtxAp->roff,ecoff=mtxAp->coff,er=mtxAp->nr,ec=mtxAp->nc;
		if(want_sc)RSB_FPRINTF(fd,"%% terminal matrix at %d %d\n",roff,coff);
		render_ps_box(fd,eroff, ecoff, er, ec, orows, xs, ys, shade, shade, shade);
	}

	RSB_SUBMATRIX_FOREACH(mtxAp,submatrix,i,j)
	if(submatrix)
	{
		rsb__dump_block_rectangles(fd,submatrix, roff+(i?(mtxAp->nr-submatrix->nr):0), coff+(j?mtxAp->nc-submatrix->nc:0),xs,ys,orows, level+1);
	}

	return RSB_ERR_NO_ERROR;
err:
	return RSB_ERR_GENERIC_ERROR;
#else /* RSB_ALLOW_STDOUT */
	return RSB_ERR_UNSUPPORTED_FEATURE;
#endif /* RSB_ALLOW_STDOUT */
}

rsb_err_t rsb__dump_postscript_recursion_from_mtx_t(FILE*fd, const char * filename, const struct rsb_mtx_t*mtxAp, rsb_blk_idx_t br, rsb_blk_idx_t bc, int width, int height, rsb_marf_t rflags, rsb_bool_t want_blocks, rsb_bool_t z_dump, rsb_bool_t want_nonzeros, int *pv )
{
	/*
	 * ( rflags == RSB_FLAG_NOFLAGS ) is allowed and implies defaults.
	 * */
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_coo_idx_t nrA = mtxAp->nr, ncA = mtxAp->nc;
	const int want_structure_comments_dump = 1;
	rsb_rf_t xs, ys;

	if(fd && filename)
	{
		errval = RSB_ERR_BADARGS;
		RSB_PERR_GOTO(err,RSB_ERRM_ES); 
	}

	if(filename)
	{
		fd = rsb__util_fopen(filename,"w");
		if( fd == NULL )
		{
			errval=RSB_ERR_GENERIC_ERROR;
			goto err;
		}
	}

	ys = ((rsb_rf_t)height)/nrA, xs = ((rsb_rf_t)width)/ncA;
	errval = rsb__do_print_postscript_header(fd, width, height, xs, ys );

	if(want_structure_comments_dump)
	{
		RSB_POSTSCRIPT_DUMP_COMMENT(fd,RSB_PRINTF_MTX_SUMMARY_ARGS(mtxAp));
		RSB_POSTSCRIPT_DUMP_COMMENT(fd,RSB_PRINTF_MATRIX_TIME_ARGS(mtxAp));
		RSB_FPRINTF(fd,"%%%% ");
		rsb__fprint_matrix_implementation_code(mtxAp, "", mtxAp->flags, fd);
		RSB_FPRINTF(fd,"\n");
	}

	if( rflags == RSB_MARF_EPS_L )
	{
			struct rsb_mtx_t * submatrix = NULL;
			rsb_submatrix_idx_t smi;
			double mnnz = 0, annz = 0;
			rsb_coo_idx_t hoo = -RSB_MIN(RSB_MIN(mtxAp->nr,mtxAp->nc)/1000,10); /* how much out; this shall turn some coloured lines inwards the box; on smaller matrices this shall be limited */

			RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi) 
				mnnz = RSB_MAX(mnnz,submatrix->nnz),
				annz += submatrix->nnz;
			annz /= mtxAp->all_leaf_matrices_n;

			RSB_FPRINTF(fd,"%% colored boxes dump\n");
			RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi) 
			{
				rsb_rf_t fx,fy;
				double rv,gv,bv, iv;

				RSB_EPS_TRSL(fx,fy,submatrix->bcoff,submatrix->broff,xs,ys,mtxAp);
				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" %g %g "RSB_EPS_MOVETO" ",fx,fy);
				RSB_FPRINTF(fd,"%g %g "RSB_EPS_RLINETO" ", xs*RSB_MTX_EFF_C(submatrix),0.0);
				RSB_FPRINTF(fd,"%g %g "RSB_EPS_RLINETO" ",0.0,-ys*RSB_MTX_EFF_R(submatrix));
				RSB_FPRINTF(fd,"%g %g "RSB_EPS_RLINETO" ",-xs*RSB_MTX_EFF_C(submatrix),0.0);

				if(submatrix->nnz > annz)
					iv = 0.3 * ( ( - annz + submatrix->nnz ) / submatrix->nnz ), 
					rv = gv = bv = 0.7,
					rv+=iv;
				else
					iv = 0.3 * ( ( + annz - submatrix->nnz ) / annz ),
					rv = gv = bv = 0.7,
				       	gv+=iv;
				RSB_FPRINTF(fd,RSB_EPS_CLOSEPATH" %g %g %g "RSB_EPS_SETRGB" 1 "RSB_EPS_SETLINEWIDTH" fill ",rv,gv,bv);
				RSB_FPRINTF(fd,"%% submatrix %d square\n",smi);
			}

			RSB_FPRINTF(fd,"%% lhs dump\n");
			RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi) 
			{
				rsb_rf_t fx,fy,tx,ty;
/*
				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" ");
				RSB_EPS_TRSL(fx,fy,submatrix->coff ,submatrix->roff,xs,ys,mtxAp);
				RSB_EPS_TRSL(tx,ty,0               ,submatrix->roff,xs,ys,mtxAp);
				RSB_FPRINTF(fd,"%g %g "RSB_EPS_MOVETO" ",fx,fy);
				RSB_FPRINTF(fd,"%g %g lineto ",tx,ty);
				px=tx,py=ty;
				RSB_EPS_TRSL(fx,fy,submatrix->coff ,submatrix->roff+submatrix->nr,xs,ys,mtxAp);
				RSB_EPS_TRSL(tx,ty,0               ,submatrix->roff+submatrix->nr,xs,ys,mtxAp);
				RSB_FPRINTF(fd,"%g %g "RSB_EPS_MOVETO" ",fx,fy);
				RSB_FPRINTF(fd,"%g %g lineto ",tx,ty);

				RSB_FPRINTF(fd,"closepath "); RSB_FPRINTF(fd,"1 0 1 "RSB_EPS_SETRGB" "); RSB_FPRINTF(fd,"1 setlinewidth "); RSB_FPRINTF(fd,"1 stroke ");
				RSB_FPRINTF(fd,"%% submatrix %d to-lhs\n",smi);

				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" ");
				RSB_FPRINTF(fd,"%g %g "RSB_EPS_MOVETO" ",px,py);
				RSB_FPRINTF(fd,"%g %g lineto ",tx,ty);
				RSB_FPRINTF(fd,"closepath "); RSB_FPRINTF(fd,"1 0 1 "RSB_EPS_SETRGB" "); RSB_FPRINTF(fd,"5 setlinewidth "); RSB_FPRINTF(fd,"1 stroke ");
				RSB_FPRINTF(fd,"%% submatrix %d lhs\n",smi);
				*/

				RSB_EPS_TRSL(fx,fy,-hoo+submatrix->bcoff,-hoo+submatrix->broff      ,xs,ys,mtxAp);
				RSB_EPS_TRSL(tx,ty,-hoo+submatrix->bcoff,-hoo+RSB_MTX_LAR(submatrix),xs,ys,mtxAp);
				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" %g %g "RSB_EPS_MOVETO" %g %g "RSB_EPS_LINETO" ",fx,fy,tx,ty);
				RSB_FPRINTF(fd,RSB_EPS_CLOSEPATH" 1 0 1 "RSB_EPS_SETRGB" 1 "RSB_EPS_SETLINEWIDTH" 1 stroke ");
				RSB_FPRINTF(fd,"%% submatrix %d lhs\n",smi);
			}

			RSB_FPRINTF(fd,"%% rhs dump\n");
			RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi) 
			{
				rsb_rf_t fx,fy,tx,ty;
				//rsb_rf_t ih = (RSB_DO_FLAG_HAS(submatrix->flags,RSB_FLAG_USE_HALFWORD_INDICES)) ? 1.0 : 0.0;
/*
				RSB_FPRINTF(fd,"%% submatrix %d\n",smi);
				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" ");
				
				RSB_EPS_TRSL(fx,fy,submatrix->coff ,submatrix->roff,xs,ys,mtxAp);
				RSB_EPS_TRSL(tx,ty,mtxAp->nr       ,submatrix->coff,xs,ys,mtxAp);
				RSB_FPRINTF(fd,"%g %g moveto ",fx,fy);
				RSB_FPRINTF(fd,"%g %g lineto ",tx,ty);
				px=tx,py=ty;
				RSB_EPS_TRSL(fx,fy,submatrix->coff+submatrix->nc,submatrix->roff,              xs,ys,mtxAp);
				RSB_EPS_TRSL(tx,ty,mtxAp->nr                    ,submatrix->coff+submatrix->nc,xs,ys,mtxAp);
				RSB_FPRINTF(fd,"%g %g moveto ",fx,fy);
				RSB_FPRINTF(fd,"%g %g lineto ",tx,ty);
				RSB_FPRINTF(fd,"closepath "); RSB_FPRINTF(fd,"0.5 1 0.5 setrgbcolor "); RSB_FPRINTF(fd,"1 setlinewidth "); RSB_FPRINTF(fd,"1 stroke ");
				RSB_FPRINTF(fd,"%% submatrix %d to-rhs\n",smi);

				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" ");
				RSB_FPRINTF(fd,"%g %g moveto ",px,py);
				RSB_FPRINTF(fd,"%g %g lineto ",tx,ty);
				RSB_FPRINTF(fd,"closepath "); RSB_FPRINTF(fd,"0.5 1 0.5 setrgbcolor "); RSB_FPRINTF(fd,"5 setlinewidth "); RSB_FPRINTF(fd,"1 stroke ");
				RSB_FPRINTF(fd,"%% submatrix %d rhs\n",smi);
			*/
				RSB_EPS_TRSL(fx,fy,-hoo+submatrix->bcoff,             -hoo+submatrix->broff,xs,ys,mtxAp);
				RSB_EPS_TRSL(tx,ty,-hoo+submatrix->coff+submatrix->bk,-hoo+submatrix->broff,xs,ys,mtxAp);
				RSB_FPRINTF(fd,RSB_EPS_NEWPATH" %g %g "RSB_EPS_MOVETO" %g %g "RSB_EPS_LINETO" ",fx,fy,tx,ty);
				RSB_FPRINTF(fd,RSB_EPS_CLOSEPATH" .5 1 .5 "RSB_EPS_SETRGB" 1 "RSB_EPS_SETLINEWIDTH" 1 stroke ");
				RSB_FPRINTF(fd,"%% submatrix %d rhs\n",smi);
			}

			RSB_FPRINTF(fd,"%% node content labels\n");
			RSB_SUBMATRIX_FOREACH_LEAF(mtxAp,submatrix,smi) 
			{
				rsb_rf_t ox,oy;
				char fstr[RSB_MAX_STRERRLEN];
				double fs;

				RSB_EPS_TRSL(ox,oy,submatrix->coff,submatrix->roff+submatrix->nr/2,xs,ys,mtxAp);
				sprintf(fstr," %d/%d %s%s %0.1e",1+smi,mtxAp->all_leaf_matrices_n,(RSB_DO_FLAG_HAS(submatrix->flags,RSB_FLAG_USE_HALFWORD_INDICES))?"H":"",(submatrix->matrix_storage == RSB_MATRIX_STORAGE_BCOR)?"COO":"CSR",(double)(submatrix->nnz));
				fs = ( xs * submatrix->nc ) /  ( strlen(fstr) ) * 1.3 ;
				RSB_FPRINTF(fd,"/Courier-Bold findfont %g "RSB_EPS_SCALEFONT" "RSB_EPS_SETFONT" %g %g "RSB_EPS_MOVETO" (%s) 0 0 0 "RSB_EPS_SETRGB" show\n",fs,ox,oy,fstr);
			}
	}

	RSB_POSTSCRIPT_DUMP_COMMENT(fd,"sparse blocks dump");
	errval = rsb__dump_block_rectangles(fd,mtxAp,0,0,xs,ys,mtxAp->nr,0);

	if(z_dump)
	{
		int p = 0;
		RSB_POSTSCRIPT_DUMP_COMMENT(fd,"z dump\nnewpath");

		if(z_dump==1)
			errval = rsb_dump_postscript_z_curve(fd,mtxAp, 0,0,xs,ys,mtxAp->nr,0,&p,pv);
		else
			errval = rsb__dump_postscript_ussv_order_curve(mtxAp,(rsb_rf_t)height,(rsb_rf_t)width,&p);
		RSB_FPRINTF(fd,
			"%d %d  %d setrgbcolor\n"
			"1 setlinewidth\n"
			"stroke\n\n"
			,
			0,0,1
			);
	}
	if(want_blocks)
		;/* dead code removed from here */
	if(filename)
	{
		fclose(fd);
	}
err:
	return errval;
}

static rsb_err_t rsb_dump_postscript_from_coo(FILE*fd, rsb_coo_idx_t *IA, rsb_coo_idx_t *JA, void *VA, rsb_coo_idx_t m, rsb_coo_idx_t k, rsb_nnz_idx_t nnz, rsb_blk_idx_t br, rsb_blk_idx_t bc, int width, int height, rsb_bool_t all_nnz, rsb_type_t typecode)
{
	/**
	 \ingroup gr_internals
	 Need better error handling.
	 This function is experimentally used to render the sparse matrix.
	 */
#if RSB_ALLOW_STDOUT
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_nnz_idx_t n=0;
	rsb_flags_t flags = RSB_FLAG_NOFLAGS;
	rsb_rf_t ys,xs;
	rsb_rf_t csh,csw,dx=0.0,dy=0.0,rd=1.0;

	RSB_DO_FLAG_ADD(flags,RSB_FLAG_WANT_BCSS_STORAGE) ;
	RSB_DO_FLAG_ADD(flags,RSB_FLAG_SORTED_INPUT) ;

if(1)
{
	       	rsb_coo_idx_t /* ri=0,ci=0,*/nr=m,nc=k;
	       	rsb_nnz_idx_t nzi=0;
	       	rsb_nnz_idx_t onnz=nnz;
		// RSB_STDERR("%s","FIXME: this is functioning code to render PostScript spy plots; it just needs to be called the right way ..\n");
		rsb_aligned_t max[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		rsb_aligned_t min[RSB_CONST_ENOUGH_ALIGNED_FOR_ANY_TYPE];
		const int want_points_plot = 0; /* was 1 */

		rsb__fill_with_ones(VA,typecode,nnz,1);
#if 1
		while( (nnz>100000 || nr>512 || nc>512 ) && (nr>2 && nc>2))
		{
			/*
				May be better to write a function *resize_to_nnz*.
				This code is quite poor but does the job.
			 */
		       	rsb_coo_idx_t nnr=nr/2, nnc=nc/2;
			rsb_flags_t flags = RSB_FLAG_NOFLAGS;
			// RSB_DO_FLAG_ADD(flags,RSB_FLAG_SORTED_INPUT) ;
			// RSB_STDERR("will rescale %d %d (%d nz) to %d %d...\n",nr,nc,nnz,nnr,nnc);
			errval = rsb__mtx_as_pixmap_resize(VA,IA,JA,nnz,&nnz,nr,nc,nnr,nnc,typecode,flags);
			RSB_DO_FLAG_ADD(flags,RSB_FLAG_SORTED_INPUT) ;
			// nnz = rsb_weed_out_duplicates(IA,JA,VA,nnz,typecode,RSB_FLAG_DUPLICATES_SUM/*|RSB_FLAG_SORTED_INPUT*/);
			if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
			IA = rsb__realloc(IA,sizeof(rsb_coo_idx_t)*(nnz));
			JA = rsb__realloc(JA,sizeof(rsb_coo_idx_t)*(nnz));
			VA = rsb__realloc_vector(VA,nnz,typecode);
			nc=nnc; nr=nnr;
			nnc/=2; nnr/=2;
		}
#endif
#if 1
		// if( (nnz>100000 || nr>height || nc>width ) && (nr>2 && nc>2))
		if(1)
		{
		       	rsb_coo_idx_t nnr=height, nnc=width;
			rsb_flags_t flags = RSB_FLAG_NOFLAGS;
			RSB_DO_FLAG_ADD(flags,RSB_FLAG_SORTED_INPUT) ;
			// RSB_STDERR("will rescale further %d %d (%d nz) to %d %d...\n",nr,nc,nnz,nnr,nnc);
			if(nnr>nr)
				rd=((rsb_rf_t)nnr)/((rsb_rf_t)nr);
			errval = rsb__mtx_as_pixmap_resize(VA,IA,JA,nnz,&nnz,nr,nc,nnr,nnc,typecode,flags);
			if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
			IA = rsb__realloc(IA,sizeof(rsb_coo_idx_t)*(nnz));
			JA = rsb__realloc(JA,sizeof(rsb_coo_idx_t)*(nnz));
			VA = rsb__realloc_vector(VA,nnz,typecode);
			nc=nnc; nr=nnr;
		}
#endif
		errval = rsb_util_sort_row_major_inner(VA,IA,JA,nnz,nr,nc, typecode,RSB_FLAG_NOFLAGS);
		if(RSB_SOME_ERROR(errval)) 
		{
		       	RSB_ERROR(RSB_ERRM_ES); /* RSB_PERR_GOTO(err,RSB_ERRM_ES)*/ /*not a critical error; however shall emit a warning : */
		}
		nnz = rsb_weed_out_duplicates(IA,JA,VA,nnz,typecode,RSB_FLAG_DUPLICATES_SUM|RSB_FLAG_SORTED_INPUT);

		RSB_FPRINTF(fd,""
			"%%!PS-Adobe-3.0 EPSF-3.0\n"
			"%%%%Creator: "RSB_PACKAGE_STRING"\n"
			"%%%%Title: matrix spy plot (originally %d x %d / %d, here %d x %d/%d)\n"
			"%%%%CreationDate: \n"
			"%%%%DocumentData: Clean7Bit\n"
			"%%%%Origin: 0 0\n"
			"%%%%BoundingBox: 0 0 %d %d\n"
			"%%%%LanguageLevel: 2\n"
			"%%%%Pages: 1\n"
			"%%%%Page: 1 1\n"
			/* "0.5 0.5 0.5 setrgbcolor\n" */
			,m,k,onnz,nr,nc,nnz,nc,nr);
		RSB_FPRINTF(fd,"save /$LIBRSB_DICT 3 dict def $LIBRSB_DICT begin /M {moveto} bind def /Z {gsave currentpoint lineto %g setlinewidth 1 setlinecap stroke grestore} bind def /D {M Z} bind def /K {0.5 0.5 setrgbcolor} bind def\n",rd);
		RSB_FPRINTF(fd,"/R {rlineto} bind def\n");
		RSB_FPRINTF(fd,"/N {newpath} bind def\n");
		RSB_FPRINTF(fd,"/L {lineto} bind def\n");
		RSB_FPRINTF(fd,"/C {closepath} bind def\n");
		RSB_FPRINTF(fd,"/SLW {setlinewidth} bind def\n");
		RSB_FPRINTF(fd,"/SRGB {setrgbcolor} bind def\n");
		RSB_FPRINTF(fd,"/SCF {scalefont} bind def\n");
		RSB_FPRINTF(fd,"/SF {setfont} bind def\n");
		rsb__util_find_max(&max[0],VA,typecode,nnz,1);
		rsb__util_find_min(&min[0],VA,typecode,nnz,1);
		// RSB_STDERR("%lf %lf\n", *(double*)(&min[0]), *(double*)(&max[0]));
		rsb__util_do_negate(&min[0],typecode,1);
		rsb__util_vector_add(&max[0],&min[0],typecode,1);
		// RSB_STDERR("%lf %lf\n", *(double*)(&min[0]), *(double*)(&max[0]));
		rsb__vector_scale_inv(VA,&max[0],typecode,nnz);

		if(want_points_plot)
		{
		RSB_FPRINTF(fd,"%% dots plot\n");
		for(nzi=0;nzi<nnz;++nzi)
		{
			// RSB_FPRINTF(fd,"%d %d D\n",IA[nzi],JA[nzi]);
			// RSB_FPRINTF(fd,"%d %d D ",nr-1-IA[nzi],JA[nzi]);
			rsb_rf_t cv=0.0;
			RSB_NUMERICAL_TYPE_CAST_TO_ANY_P(rsb_rf_t,cv,typecode,VA,nzi);
			// cv=1.0f-cv;
			cv=0.5+cv/2.0; /* stronger */
			//cv=0.0+cv/2;
			// cv=0.5;
			// gray ... red
			// RSB_FPRINTF(fd,"%0.2f %0.2f %0.2f setrgbcolor\n",cv,0.5,0.5);
			RSB_FPRINTF(fd,"%.2f K ",cv);
			//RSB_FPRINTF(fd,"%d %d D ",nr-1-IA[nzi],JA[nzi]);
			RSB_FPRINTF(fd,"%d %d D ",JA[nzi],nr-1-IA[nzi]);
			if(nzi%32==0)
				RSB_FPRINTF(fd,"\n");
		}
		}
		// RSB_FPRINTF(fd,"gsave grestore showpage\n");
		RSB_FPRINTF(fd,"stroke\n");
		goto err;
	}

	if(!all_nnz)
	{
		/* rsb__mtx_as_pixmap_resize is optional */
		if( RSB_SOME_ERROR(errval = rsb__mtx_as_pixmap_resize(VA, IA, JA, nnz, &nnz, m, k, height, width, typecode, flags)))
			goto err;
	}
	/*	if(m<=height)
			ys=1.0;
		else
			ys=((rsb_rf_t)height)/m;
		if(k<=width)
			xs=1.0;
		else*/
		ys=((rsb_rf_t)height)/m;
		xs=((rsb_rf_t)width)/k;
		csw=ys;
		csh=xs;
	
//	{
//		ys=((rsb_rf_t)height)/m;
//		xs=((rsb_rf_t)width)/k;
//	}
	if(width>k)
		dx=.1*csw, csw*=.8;
	else
		xs=csw=1.0;
	if(height>m)
		dy=.1*csh, csh*=.8;
	else
		ys=csh=1.0;
/*
	if(height>m)
		yps=ys*.8;
	else
		yps=ys;
	if(width>m)
		xps=xs*.8;
	else
		xps=xs;

	if(!all_nnz)
	{
		m=height;
		k=width;
	}*/

	rsb__do_print_postscript_header(fd, width, height, csw, csh);

	RSB_FPRINTF(fd,"%%%% nnz dump\n");
	RSB_FPRINTF(fd,"%%%% scales : %g %g\n",xs,ys);


	if(xs>1.0) xs=1.0;
	if(ys>1.0)ys=1.0;

	for(n=0;n<nnz;++n)
	{
		RSB_FPRINTF(fd,"%%%% at : %d %d\n",(int)IA[n],(int)JA[n]);
		RSB_FPRINTF(fd,
			"%g %g translate\n"
			".85 .85 .85 csquare\n"
			"-%g -%g translate\n"
			, dx+((rsb_rf_t) (JA[n]))*xs, -dy+((rsb_rf_t)height)-((rsb_rf_t) (IA[n]))*ys
			, dx+((rsb_rf_t) (JA[n]))*xs, -dy+((rsb_rf_t)height)-((rsb_rf_t) (IA[n]))*ys);
	}

	//RSB_FPRINTF(fd, "%%%%EOF\n");

err:
	RSB_DO_ERR_RETURN(errval)
#else /* RSB_ALLOW_STDOUT */
	RSB_DO_ERR_RETURN(RSB_ERR_UNSUPPORTED_FEATURE);
#endif /* RSB_ALLOW_STDOUT */
}

rsb_err_t rsb__dump_postscript_recursion_from_matrix(const char * filename, rsb_blk_idx_t br, rsb_blk_idx_t bc, int width, int height, rsb_flags_t flags, rsb_bool_t want_blocks, rsb_bool_t z_dump , rsb_bool_t want_nonzeros, rsb_bool_t want_recursion, rsb_type_t typecode)
{
	/**
	 \ingroup gr_internals
	 */
#if RSB_ALLOW_STDOUT
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_coo_idx_t *IA=NULL, *JA=NULL;
	void *VA=NULL;
	rsb_coo_idx_t m=0,k=0;
       	rsb_nnz_idx_t nnz=0;
	struct rsb_mtx_t * mtxAp=NULL;
	FILE*fd = RSB_DEFAULT_FD;

	if(!filename )
	{
		RSB_ERROR(RSB_ERRM_ES); 
		return RSB_ERR_BADARGS;
	}

	errval = rsb__util_mm_load_matrix_f(filename,&IA,&JA,&VA,&m,&k,&nnz,typecode,flags,NULL,NULL);
	if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }

	mtxAp = rsb__do_mtx_alloc_from_coo_const(VA,IA,JA,nnz,typecode,m,k,br,bc,flags,&errval);
	if(!mtxAp || RSB_SOME_ERROR(errval))
	{
		RSB_PERR_GOTO(err,RSB_ERRM_ES); 
	}

	if( want_nonzeros )
	{
		// RSB_POSTSCRIPT_DUMP_COMMENT(fd,"nonzeros structure dump");
		errval = rsb_dump_postscript_from_coo(fd, IA, JA, VA, m, k, nnz, br, bc, width, height, want_nonzeros, typecode);
		want_nonzeros = 0;
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
	}

	RSB_CONDITIONAL_FREE(IA);
       	RSB_CONDITIONAL_FREE(JA);
       	RSB_CONDITIONAL_FREE(VA);

	if(want_recursion)
	{
		errval = rsb__dump_postscript_recursion_from_mtx_t(fd, NULL, mtxAp, br, bc, width, height, RSB_FLAG_NOFLAGS/* FIXME */, want_blocks, z_dump , 0 /*want_nonzeros*/, NULL );
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
	}

	RSB_MTX_FREE(mtxAp);/* we don't need it anymore here */

	if( want_nonzeros )
	{
		RSB_POSTSCRIPT_DUMP_COMMENT(fd,"nonzeros structure dump");
		errval = rsb__dump_postscript_from_matrix(filename, br, bc, width, height, 1);
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
	}

	errval = RSB_ERR_NO_ERROR;
	goto ret;
err:
	errval = RSB_ERR_GENERIC_ERROR;
ret:
	RSB_CONDITIONAL_FREE(IA); RSB_CONDITIONAL_FREE(JA); RSB_CONDITIONAL_FREE(VA);
	RSB_MTX_FREE(mtxAp);
	return errval;
#else  /* RSB_ALLOW_STDOUT */
	return RSB_ERR_UNSUPPORTED_FEATURE;
#endif /* RSB_ALLOW_STDOUT */
}

static rsb_err_t rsb_dump_postscript_from_mtx_t(FILE*fd, const struct rsb_mtx_t*mtxAp, rsb_blk_idx_t br, rsb_blk_idx_t bc, int width, int height, rsb_bool_t all_nnz)
{
	struct rsb_coo_matrix_t coo;
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	RSB_INIT_COO_FROM_MTX(&coo,mtxAp);
	if(rsb__allocate_coo_matrix_t(&coo)!=&coo)
       	{
	       	RSB_PERR_GOTO(err,RSB_ERRM_ES); 
	}
	errval = rsb__do_get_coo(mtxAp,(rsb_byte_t**)(&coo.VA),&coo.IA,&coo.JA,RSB_FLAG_NOFLAGS);
	if(!RSB_SOME_ERROR(errval))
		errval = rsb_dump_postscript_from_coo(fd, coo.IA, coo.JA, coo.VA, coo.nr, coo.nc, coo.nnz, br, bc, width, height, all_nnz, mtxAp->typecode);
	if(RSB_SOME_ERROR(errval))
	{
		RSB_PERR_GOTO(merr,RSB_ERRM_ES);
	}
merr:
	rsb__destroy_coo_matrix_t(&coo);
err:
	return errval;
}

rsb_err_t rsb__dump_postscript_from_matrix(const char * filename, rsb_blk_idx_t br, rsb_blk_idx_t bc, int width, int height, rsb_bool_t all_nnz)
{
	/**
	 \ingroup gr_internals
	 This function is experimentally used to render the sparse matrix.
	 Needs better error handling.
	 */
#if RSB_ALLOW_STDOUT
	rsb_err_t errval = RSB_ERR_NO_ERROR;
	rsb_coo_idx_t *IA=NULL, *JA=NULL;
	void *VA=NULL;
	rsb_coo_idx_t m=0,k=0;
	rsb_nnz_idx_t nnz=0;
#ifdef RSB_NUMERICAL_TYPE_DOUBLE
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_DOUBLE ;
#else /* RSB_NUMERICAL_TYPE_DOUBLE */
	rsb_type_t typecode = RSB_NUMERICAL_TYPE_DEFAULT;
#endif /* RSB_NUMERICAL_TYPE_DOUBLE */
	rsb_flags_t flags = RSB_FLAG_NOFLAGS;
	rsb_time_t t=0;

	RSB_DO_FLAG_ADD(flags,RSB_FLAG_WANT_BCSS_STORAGE) ;
	RSB_DO_FLAG_ADD(flags,RSB_FLAG_SORTED_INPUT) ;

	if(!filename )
		return RSB_ERR_BADARGS;

	t = - rsb_time();
	if( RSB_SOME_ERROR(errval = rsb__util_mm_load_matrix_f(filename, &IA, &JA,&VA , &m, &k, &nnz , typecode, flags, NULL, NULL)) )
		goto err;
	t += rsb_time();

#if 1
	errval = rsb_dump_postscript_from_coo(/*fd*/RSB_DEFAULT_FD, IA, JA, VA, m, k, nnz, br, bc, width, height, all_nnz, typecode);
#else
#if 0
	{
		RSB_STDERR("%s","FIXME: this is functioning code to render PostScript raster spy plots; it just needs the right place to be employed ..\n");
		FILE*fd = RSB_DEFAULT_FD;
	       	rsb_coo_idx_t ri=0,ci=0,nr=m,nc=k;
	       	const rsb_coo_idx_t nnr=16/*0*2*/;
		const rsb_coo_idx_t nnc=nc/(nr/nnr);
	       	rsb_nnz_idx_t nzi=0;
		errval = rsb__mtx_as_pixmap_resize(VA,IA,JA,nnz,&nnz,nr,nc,nnr,nnc,typecode,RSB_FLAG_NOFLAGS);
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
		nc=nnc;
		nr=nnr;
		errval = rsb_util_sort_row_major_inner(VA,IA,JA,nnz,nr,nc, typecode,RSB_FLAG_NOFLAGS);
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
		errval = rsb__util_compress_to_row_pointers_array(NULL,nnz,nr,RSB_FLAG_NOFLAGS,RSB_FLAG_NOFLAGS,IA);
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
		// errval = rsb__do_switch_rsb_mtx_to_csr_sorted(mtxAp, &VA, &IA, &JA, RSB_FLAG_NOFLAGS);
		if(RSB_SOME_ERROR(errval)) { RSB_PERR_GOTO(err,RSB_ERRM_ES) }
		// RSB_POSTSCRIPT_DUMP_COMMENT(fd,"raster dump\n");
		RSB_FPRINTF(fd,""
			"%%!PS-Adobe-3.0 EPSF-3.0\n"
			"%%%%Creator: "RSB_PACKAGE_STRING"\n"
			"%%%%Title: matrix spy plot\n"
			"%%%%CreationDate: \n"
			"%%%%DocumentData: Clean7Bit\n"
			"%%%%Origin: 0 0\n"
			"%%%%BoundingBox: 0 0 %d %d\n"
			"%%%%LanguageLevel: 2\n"
			"%%%%Pages: 1\n"
			"%%%%Page: 1 1\n"
		,nc,nr);
		RSB_FPRINTF(fd,"gsave\n""0 %d translate\n""%d %d scale\n""%d %d 8 [%d 0 0 -%d 0 0]\n"" {<",nr,nc,nr,nc,nr,nc,nr);
		for(ri=0;ri<nr;++ri)
		{
	       		rsb_coo_idx_t fc=0,lc=0;
	       		rsb_coo_idx_t crp=IA[ri],nrp=IA[ri+1];
			if(nrp==crp)
				lc=nc-1;
			else
				lc=JA[crp]-1;
			for(ci=fc;ci<=lc;++ci)
				RSB_FPRINTF(fd,"FF");
			for(nzi=crp;nzi<nrp;++nzi)
			{
				RSB_FPRINTF(fd,"00");
				fc=JA[nzi]+1;
				lc=fc-1;
				if(JA[nzi]==nc-1)
					lc=nc-1;
				else
				{
					if(nzi+1 < nrp)
						lc=JA[nzi+1]-1;
					else
						lc=nc-1;
				}
				for(ci=fc;ci<=lc;++ci)
					RSB_FPRINTF(fd,"FF");
			}
			RSB_FPRINTF(fd,"\n");
		}
		RSB_FPRINTF(fd,">}\n""image\n""grestore\n""showpage\n");
	}
#else
		goto err;
#endif
#endif
err:
	RSB_CONDITIONAL_FREE(IA);
	RSB_CONDITIONAL_FREE(JA);
	RSB_CONDITIONAL_FREE(VA);
	RSB_DO_ERR_RETURN(errval)
#else /* RSB_ALLOW_STDOUT */
	RSB_DO_ERR_RETURN(RSB_ERR_UNSUPPORTED_FEATURE);
#endif /* RSB_ALLOW_STDOUT */
}

rsb_err_t rsb__do_mtx_render(const char * filename, const struct rsb_mtx_t*mtxAp, rsb_coo_idx_t pmWidth, rsb_coo_idx_t pmHeight, rsb_marf_t rflags)
{
	rsb_err_t errval = RSB_ERR_NO_ERROR;

	switch(rflags)
	{
		case(RSB_MARF_RGB):
		RSB_DO_ERROR_CUMULATE(errval,RSB_ERR_UNIMPLEMENTED_YET);
		break;
		case(RSB_MARF_EPS_L):
		case(RSB_MARF_EPS_B):
		case(RSB_MARF_EPS_S):
		case(RSB_MARF_EPS):
		{
			FILE * fd = NULL;
			rsb_time_t dt = rsb_time();
			/* filename = filename ? filename : RSB_DEFAULT_DUMPFILENAME; */
			if( ! filename )
			{
		       		fd = RSB_DEFAULT_FD;
			}
			else
		       		fd = rsb__util_fopen(filename,"w");

			if( rflags == RSB_MARF_EPS || rflags == RSB_MARF_EPS_S || rflags == RSB_MARF_EPS_L )
				RSB_DO_ERROR_CUMULATE(errval,rsb_dump_postscript_from_mtx_t(fd,mtxAp,1,1,pmWidth,pmHeight,1));
			if( rflags == RSB_MARF_EPS || rflags == RSB_MARF_EPS_B || rflags == RSB_MARF_EPS_L )
				RSB_DO_ERROR_CUMULATE(errval,rsb__dump_postscript_recursion_from_mtx_t(fd,NULL,mtxAp,1,1,pmWidth,pmHeight,rflags,0,1,0,NULL));
			if( fd )
			{
				dt = rsb_time() - dt;
				RSB_FPRINTF(fd,"%% rendering time ~ %lg s\n",dt);
			}
			
			if( filename )
				fclose(fd);
		}
		break;
		default: {errval = RSB_ERR_UNIMPLEMENTED_YET; goto err;}
	}
err:
	return errval;
}

/* @endcond */
