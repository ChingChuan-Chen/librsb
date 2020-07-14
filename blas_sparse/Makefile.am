
subdir=blas_sparse

EXTRA_DIST= \
	blas_enum.h  blas_sparse.h  blas_sparse_proto.h blas_enum.F90

all: blas_enum.F90

blas_enum.F90: blas_enum.h
	( echo '! This file has been auto-generated from blas_enum.h.'; cat blas_enum.h  | grep '^\s*blas' | sed 's/[,};/].*//g;s/\s*//g;s/^/        INTEGER,PARAMETER :: /' ) > $@

