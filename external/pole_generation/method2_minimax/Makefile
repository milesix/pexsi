# NOTE: This Makefile does NOT support auto-dependency for the .h files.
# If the header files are changed, do "make clean" first.

FFTW_LIB         = -lfftw3_mpi -lfftw3 -lm
LAPACK_LIB       = -llapack
BLAS_LIB         = -lblas
LIBS         =  ${FFTW_LIB}  ${LAPACK_LIB} ${BLAS_LIB} 

CC           = gcc
CFLAGS       = -O3 -std=c99

minimax: minimax.c
	${CC} ${CFLAGS} $^ -o $@ ${LIBS}
