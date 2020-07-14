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
 * @author Michele Martone
 * @brief
 * */
#include "rsb_common.h"
#include "rsb_util.h"
#include "rsb.h"

#define rsb__strcpy strcpy
#define rsb__sprintf sprintf

RSB_INTERNALS_COMMON_HEAD_DECLS

rsb_err_t rsb__do_strerror_r(rsb_err_t errval, rsb_char_t * buf, size_t buflen)
{
	/* TODO: what if buflen is not enough ? shall report this somehow. */
	rsb_char_t*sbuf = buf;
	const rsb_char_t *s = "No error occurred (success). The return value that means function operation success, in most cases.\n";

	if((errval == RSB_ERR_NO_ERROR))
		goto err;

	if( buf == NULL)
	{
		errval = RSB_ERR_BADARGS;
		goto err;
	}

	switch(errval)
	{
		case RSB_ERR_GENERIC_ERROR:
		s = "An unspecified error occurred.";
		break;
		case RSB_ERR_UNSUPPORTED_OPERATION:
		s = "The user requested an operation which is not supported (e.g.: was opted out at build time).";
		break;
		case RSB_ERR_UNSUPPORTED_TYPE:
		s = "The user requested to use a type which is not supported (e.g.: was opted out at build time).";
		break;
		case RSB_ERR_UNSUPPORTED_FORMAT:
		s = "The user requested to use a matrix storage format which is not supported (e.g.: was opted out at build time).";
		break;
		case RSB_ERR_INTERNAL_ERROR:
		s = "An error occurred which is not apparently caused by a user's fault (internal error).";
		break;
		case RSB_ERR_BADARGS:
		s = "The user supplied some corrupt data as argument.";
		break;
		case RSB_ERR_ENOMEM:
		s = "There is not enough dynamical memory to perform the requested operation.";
		break;
		case RSB_ERR_UNIMPLEMENTED_YET:
		s = "The requested operation was not implemented yet in this code revision.";
		break;
		case RSB_ERR_LIMITS:
		s = "The requested operation could not be executed, or index overflow will happen.";
		break;
		case RSB_ERR_NO_USER_CONFIGURATION:
		s = "A file containing user set configuration was not present.";
		break;
		case RSB_ERR_CORRUPT_INPUT_DATA:
		s = "User supplied data (e.g.: from file) was corrupt.";
		break;
		case RSB_ERR_FAILED_MEMHIER_DETECTION:
		s = "Memory hierarchy info failed to be detected. You can bypass this by setting a meaningful RSB_USER_SET_MEM_HIERARCHY_INFO environment variable.";
		break;
		case RSB_ERR_COULD_NOT_HONOUR_EXTERNALLY_ALLOCATION_FLAGS:
		s = "User gave flags for an inplace constructor in a copy-based routine.";
		break;
		case RSB_ERR_UNSUPPORTED_FEATURE:
		s = "The requested feature (e.g.:blocking) is not available because it was opted out or not configured at built time.";
		break;
		case RSB_ERR_NO_STREAM_OUTPUT_CONFIGURED_OUT:
		s = "Output to stream feature has been disabled at configure time.";
		break;
		case RSB_ERR_INVALID_NUMERICAL_DATA:
		s = "User gave some input with invalid numerical data.";
		break;
		case RSB_ERR_MEMORY_LEAK:
		s = "Probable memory leak (user did not deallocate librsb structures before calling rsb_lib_exit()).";
		break;
		/*
		case RSB_ERR_FORTRAN_ERROR:
		s = "A Fortran specific error occurred.";
		break;
		*/
		default:
		{
			rsb__sprintf(sbuf,"Unknown error code (%x).",errval);
			s = sbuf;
			errval = RSB_ERR_BADARGS;
			goto err;
		}
	}
	errval = RSB_ERR_NO_ERROR;
	rsb__sprintf(sbuf,"%s",s);
err:
	return errval;
}

rsb_err_t rsb__do_perror(FILE *stream, rsb_err_t errval)
{
	rsb_char_t sbuf[RSB_MAX_STRERRLEN];
	/*!
	 * \ingroup gr_internals
	 */
	if((errval == RSB_ERR_NO_ERROR))
		goto err;

	rsb__do_strerror_r(errval,sbuf,sizeof(sbuf)/sizeof(sbuf[0]));
	 
	if(stream)
		fprintf(stream,"ERROR 0x%x : %s\n",(unsigned int)errval,sbuf);
	else
		RSB_STDERR("ERROR 0x%x : %s\n",(unsigned int)errval,sbuf);
err:
	RSB_DO_ERR_RETURN(RSB_ERR_NO_ERROR)
}

/* @endcond */
