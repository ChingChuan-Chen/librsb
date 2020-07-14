#!/bin/bash
#
# Copyright (C) 2008-2015 Michele Martone
# 
# This file is part of librsb.
# 
# librsb is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# librsb is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with librsb; see the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.

SRCDIR=
BLDDIR=
if test $# = 0 ; then SRCDIR=. ; else SRCDIR="$1"; fi
if test $# = 1 ; then BLDDIR=. ; else BLDDIR="$2"; fi
IF=${SRCDIR}/rsb.h
TF=${BLDDIR}/rsb_types.h
CH2ICFB=./ch2icfb
(
${CH2ICFB} < ${IF} | grep -v 'END MODULE rsb' 
SHEXP='s/0x\([A-F0-9]\+\)/INT(Z"0\1",C_INT)/g'
#SHEXP='s/0x\([0-9]\+\)/Z"0\1"/g'
IPD="INTEGER(C_INT),PARAMETER::"
IPD2="INTEGER(C_INT),PARAMETER::"
FD="s/^\([^\s]\+\) \([^\s]\+\)/${IPD2}\1=\2/g"
CLEANUP='s/\s\s*/ /g;s/^,//g;s/=//g;s/\/.*$//g;s/^\s*//g;s/#define *//g'
D2N='s/#define //g'
DS='^#define '
SEE='s/\(PARAMETER::*\) *\(RSB[A-Z_0-9]*\)\(.*$\)/\1\2\3 !< See #\2./g'
IC='      '
SHORTEN_DC='s/\(::\)/\&\n'"${IC}"'\&\1/g;'
SHORTEN_EX='s/\([A-Z_]+\+\)/\1\&\n'"${IC}"'\&/g;'
SHORTEN_PA='s/\( *:: *[A-Z_]\+\)/\1\&\n'"${IC}"'\&/g;'"$SHORTEN_EX""${SHORTEN_DC}"
#SHORTEN_PM='s/\([=+]\)/\&\n'"${IC}"'\&\1/g;'
SHORTEN_TK='s/\s\s*/\&\n'"${IC}"'\&/g;'
NOTS='s/\s*$//g;'


echo '! Error values '
sed 's/\s\s*/ /g;s/^\(.define\) \(RSB_ERR[^ ]*\) RSB_ERR_CAST(0x\([^ ]*\))$/DEFINE \2 = -INT(Z"0\3",C_INT)/g;s/RSB_ERR_CAST/-/g;s/DEFINE */'"${IPD}"'/g;' < ${IF} | grep '^ *INTE.*RSB_ERR' | grep -v 'RSB_ERRS_UNSUPPORTED_FEATURES\|RSB_ERR_TO_PROGRAM_ERROR'  | sed "${SEE}"| sed "${NOTS}${SHORTEN_PA}"

echo '! Matrix flags values '
grep RSB_FLAG_ ${IF} | grep -v '\\$' | grep '^.define' | sed 's/\s\s*/ /g;'"${SHEXP}" | grep -v '\/.*' | sed 's/\s\s*/ /g;s/^\(.define\) \(RSB_FLAG[^\s]*\) \(INT(Z[^\s]*\)$/DEFINE\2 = \3/g;s/DEFINE/'"${IPD}"'/g;'  | grep '^ *INTE.*RSB_FLAG'  | sed "${SEE}"| sed "${NOTS}${SHORTEN_PA}"

echo '! Composite flags '
grep RSB_FLAG_ ${IF} | grep -v '[ 	]0x'  | sed 's/\s\s*/ /g;s/|/+/g;s/^\(.define\)\s\(RSB_FLAG[^	 ]*\)\s\(.*$\)/DEFINE \2 = \3/g;s/^ *//g;s/DEFINE/'"${IPD2}"'/g' | grep '^ *INTE.*RSB_FLAG' | sed "${SEE}" | sed "${NOTS}${SHORTEN_PA}"

echo '! Transposition constants '
grep "${DS} *"'RSB_TRANSPOSITION_[NTC]' "${TF}" | sed "${CLEANUP};${D2N};${SHEXP};${FD}"

echo '! Numerical types constants '
grep "${DS} *"'RSB_NUMERICAL_TYPE_FORTRAN_' "${TF}" | sed "${CLEANUP};${D2N};${SHEXP};${FD};s/_FORTRAN//g" | sed 's/C_INT/C_SIGNED_CHAR/g' | sed "${SHORTEN_DC}"

echo '! Other enumerations constants '
grep '^\(.define\|[ ,]*\) *RSB_\(IO_WANT\|MARF\|PRECF\|EXTF\|MIF\|ELOPF\)_' ${IF} | sed "${CLEANUP};${SHEXP};${FD};${SEE}"| sed "${NOTS}${SHORTEN_PA}${SHORTEN_EX}"
grep '^\(.define\) *RSB_\(NULL\)_' ${IF} | sed "${CLEANUP};${SHEXP};${FD};${SEE}" | sed "${NOTS}${SHORTEN_PA}"| sed 's/\<NULL\>/C_NULL_PTR/g;s/INTEGER(C_INT)/TYPE(C_PTR)/g'

echo 'END MODULE rsb'
) | sed 's/^/      /g;s/^\( *!\)/!/g'
