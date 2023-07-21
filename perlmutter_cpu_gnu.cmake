#PEXSI
# Typical Build Environment for GNU+Linux systems with GNU compilers
SET(CMAKE_Fortran_COMPILER "ftn" CACHE STRING "MPI Fortran compiler")
SET(CMAKE_C_COMPILER "cc" CACHE STRING "MPI C compiler")
SET(CMAKE_CXX_COMPILER "CC" CACHE STRING "MPI C++ compiler")
set(PEXSI_ENABLE_OPENMP    OFF)
set(CMAKE_LINKER           "CC")
set(LAPACK_LIBRARIES "$ENV{LIBSCI_LIBS}" CACHE STRING "lapack library chosen")
# Setting the BLAS library seems to be important. Otherwise with GNU compiler it will load /usr/lib64/libblas.so.3 which is drastically slower
set(BLAS_LIBRARIES "$ENV{LIBSCI_LIBS}" CACHE STRING "blas library chosen") 

SET(CMAKE_Fortran_FLAGS "-O3 -funroll-loops -funsafe-math-optimizations -fallow-argument-mismatch" CACHE STRING "Fortran flags")
SET(CMAKE_C_FLAGS " -O3 -ffast-math" CACHE STRING "C flags")
SET(CMAKE_CXX_FLAGS " -O3 -ffast-math" CACHE STRING "C++ flags")


set( PEXSI_DIR              "$ENV{HDIR}")
set( SuperLU_DIST_PREFIX    "${PEXSI_DIR}/external/SuperLU_DIST_8.1.0" )
set( ParMETIS_PREFIX        "${PEXSI_DIR}/external/parmetis_4.0.3" )
set( PEXSI_ENABLE_FORTRAN   ON )
# set( )

#ParMETIS
set(ParMETIS_DIR              "${ParMETIS_PREFIX}")
set(GKLIB_PATH                "${ParMETIS_DIR}/metis/GKlib")
set(METIS_PATH                "${ParMETIS_DIR}/metis")


#SuperLU_DIST
set(TPL_PARMETIS_INCLUDE_DIRS )
set(TPL_PARMETIS_INCLUDE_DIRS "${ParMETIS_DIR}/include;${ParMETIS_DIR}/metis/include")
set(TPL_PARMETIS_LIBRARIES    "${CMAKE_INSTALL_PREFIX}/lib/libparmetis.a;${BUILDDIR}/libmetis/libmetis.a")
set(TPL_ENABLE_PARMETISLIB    ON)

set(enable_openmp             OFF)
set(TPL_ENABLE_BLASLIB        OFF)
