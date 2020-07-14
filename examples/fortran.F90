! 
! Copyright (C) 2008-2019 Michele Martone
! 
! This file is part of librsb.
! 
! librsb is free software; you can redistribute it and/or modify it
! under the terms of the GNU Lesser General Public License as published
! by the Free Software Foundation; either version 3 of the License, or
! (at your option) any later version.
! 
! librsb is distributed in the hope that it will be useful, but WITHOUT
! ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
! FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
! License for more details.
! 
! You should have received a copy of the GNU Lesser General Public
! License along with librsb; see the file COPYING.
! If not, see <http://www.gnu.org/licenses/>.
! 

      SUBROUTINE blas_sparse_mod_example(res)
      USE blas_sparse
      USE rsb ! For the second part of the example
      IMPLICIT NONE
      INTEGER :: res, istat = 0, i
      TYPE(C_PTR),TARGET :: mtxAp = C_NULL_PTR ! matrix pointer
      INTEGER :: A
      INTEGER,PARAMETER :: transn = blas_no_trans
      INTEGER,PARAMETER :: incx = 1
      INTEGER,PARAMETER :: incy = 1
      REAL(KIND=8),PARAMETER :: alpha = 3
! Symmetric (declared via lower triangle) matrix based example, e.g.:
! 1 0
! 1 1
      ! declaration of VA,IA,JA 
      !INTEGER,PARAMETER :: nr = 100
      INTEGER,PARAMETER :: nr = 20
      INTEGER,PARAMETER :: nc = nr
      INTEGER,PARAMETER :: nnz = (nr*(nr+1))/2 ! half the square
      INTEGER :: nt = 0
      INTEGER :: ic, ir
      INTEGER,PARAMETER :: IA(nnz) = (/ (((ir), ic=1,ir), ir=1,nr ) /) ! (/1, 2, 2/)
      INTEGER,PARAMETER :: JA(nnz) = (/ (((ic), ic=1,ir), ir=1,nr ) /) ! (/1, 1, 2/)
      REAL(KIND=8),PARAMETER :: VA(nnz) = (/ ((1, ic=1,ir), ir=1,nr ) /) ! (/1, 1, 1/)
      REAL(KIND=8) :: x(nc) = (/((1), ir=1,nc)/) ! reference x ! (/1, 1/)
      REAL(KIND=8),PARAMETER :: cy(nr) = (/((alpha+alpha*nr), ir=1,nr)/) ! reference cy after ! (/9, 9/)
      REAL(KIND=8) :: y(nr) = (/((alpha), ir=1,nr)/) ! y will be overwritten ! (/3, 3/)
      ! First example part: pure blas_sparse code.
      res = 0
      CALL duscr_begin(nr,nc,A,res)
      IF (res.NE.0) GOTO 9999
      CALL ussp(A,blas_lower_symmetric,istat)
      IF (istat.NE.0) GOTO 9997
      CALL ussp(A,blas_rsb_spmv_autotuning_on,istat) ! (experimental) turns auto-tuning + thread setting on
      IF (istat.NE.0) PRINT *,"autotuning returned nonzero:", istat &
       &," ...did you enable autotuning ?"
      !
      ! First style example 
      CALL uscr_insert_entries(A,nnz,VA,IA,JA,istat)
      IF (istat.NE.0) GOTO 9997
      CALL uscr_end(A,istat)
      IF (istat.NE.0) GOTO 9997
      ! CALL ussp(A,blas_rsb_duplicates_sum,istat)
      ! CALL uscr_insert_entries(A,nnz,VA,IA,JA,istat) ! uncomment this to activate add of coefficients to pattern
      CALL usgp(A,blas_rsb_spmv_autotuning_on,nt)  ! (experimental)
      IF (nt.NE.0) PRINT*,"autotuner chose ",nt," threads"
      CALL ussp(A,blas_rsb_spmv_autotuning_off,istat) ! (experimental) turns auto-tuning + thread setting off
      IF (istat.NE.0) GOTO 9997

      CALL usmv(transn,alpha,A,x,incx,y,incy,istat)
      IF (istat.NE.0) GOTO 9997
      !
      DO i = 1, nr
            IF (y(i).NE.cy(i)) PRINT *, "first check results are not ok"
            IF (y(i).NE.cy(i)) GOTO 9997
      END DO
      !
      y(:) = alpha ! reset
      !
      ! Second style example 
      CALL ussp(A,blas_rsb_autotune_next_operation,istat) ! (experimental) turns auto-tuning + thread setting on
      IF (istat.NE.0) GOTO 9997
      CALL usmv(transn,alpha,A,x,incx,y,incy,istat)
      CALL usmm(blas_colmajor,transn,1, alpha,A,x,nr,y,nc,istat) ! Equivalent to the above (as long as incx=incy=1).
      CALL usmm(blas_colmajor,transn,1,-alpha,A,x,nr,y,nc,istat) ! Subtract the last usmm call contribution.
      IF (istat.NE.0) GOTO 9997
      !
      DO i = 1, nr
            IF (y(i).NE.cy(i)) PRINT *,"second check results are not ok"
            IF (y(i).NE.cy(i)) GOTO 9997
      END DO
      !
      PRINT *, "check results are ok"
      
      ! Second part of the example: access to the rsb.h interface via
      ! the ISO C Binding interface.
      mtxAp = rsb_BLAS_get_mtx(A) ! get pointer to rsb structure (as in the rsb.h API)
      IF(nr.LT.5) istat = rsb_file_mtx_save(mtxAp,C_NULL_PTR) ! write to stdout (only if matrix small enough)

      GOTO 9998
9997      res = -1
9998      CONTINUE
      CALL usds(A,istat)
      IF (istat.NE.0) res = -1
9999      CONTINUE
      end SUBROUTINE blas_sparse_mod_example

      PROGRAM main
      USE rsb, ONLY: rsb_lib_init, rsb_lib_exit, C_PTR, C_NULL_PTR,&
       & RSB_IO_WANT_EXTRA_VERBOSE_INTERFACE,RSB_IO_WANT_VERBOSE_TUNING,&
       & rsb_lib_set_opt
      USE iso_c_binding
      IMPLICIT NONE
      INTEGER :: res = 0, passed = 0, failed = 0
      !TYPE(C_PTR),PARAMETER :: EO = RSB_NULL_EXIT_OPTIONS
      !TYPE(C_PTR),PARAMETER :: IO = RSB_NULL_INIT_OPTIONS
      ! Note: using C_NULL_PTR instead of the previous lines because of http://gcc.gnu.org/bugzilla/show_bug.cgi?id=59411
      TYPE(C_PTR),PARAMETER :: EO = C_NULL_PTR
      TYPE(C_PTR),PARAMETER :: IO = C_NULL_PTR
      INTEGER,TARGET::IONE=1
      res = rsb_lib_init(IO)
      res = rsb_lib_set_opt(RSB_IO_WANT_VERBOSE_TUNING,C_LOC(IONE))
      
      CALL blas_sparse_mod_example(res)
      IF (res.LT.0) failed = failed + 1
      IF (res.EQ.0) passed = passed + 1

      res = rsb_lib_exit(EO)
      
      PRINT *, "FAILED:", failed
      PRINT *, "PASSED:", passed
      IF (failed .GT. 0) THEN
       STOP 1
      END IF
      END PROGRAM
