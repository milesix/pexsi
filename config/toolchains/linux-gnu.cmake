# Typical Build Environment for GNU+Linux systems with GNU compilers
set( CMAKE_C_COMPILER       mpicc    )
set( CMAKE_CXX_COMPILER     mpicxx   )
set( CMAKE_Fortran_COMPILER mpif90   )
set( PEXSI_ENABLE_OPENMP       ON)


set( BLAS_PREFIX            "/home/linlin/Software/OpenBLAS_install" )
set( LAPACK_PREFIX          "${BLAS_PREFIX}" ) # OpenBLAS has a LAPACK linker



#SuperLU_DIST
set(CMAKE_LINKER              mpicxx)
set(TPL_ENABLE_BLASLIB        OFF)
# set(TPL_BLAS_LIBRARIES        "-L${BLAS_PREFIX}/lib -lopenblas")
