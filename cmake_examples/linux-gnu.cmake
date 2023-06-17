
#PEXSI
# Typical Build Environment for GNU+Linux systems with GNU compilers
set( CMAKE_C_COMPILER       mpicc    )
set( CMAKE_CXX_COMPILER     mpicxx   )
set( CMAKE_Fortran_COMPILER mpif90   )
set(PEXSI_ENABLE_OPENMP       ON)

set(PEXSI_DIR                 "${CMAKE_SOURCE_DIR}")
set( SuperLU_DIST_PREFIX    "${CMAKE_SOURCE_DIR}/external/SuperLU_DIST_8.1.0" )
set( ParMETIS_PREFIX        "${CMAKE_SOURCE_DIR}/external/parmetis_4.0.3" )
set( BLAS_PREFIX            "/share/home/wanghongyu/spack/opt/spack/linux-centos7-haswell/gcc-4.8.5/openblas-0.3.21-25je2lbzvjfhz5aujeakwbd3jz2ldog4/" )
set( LAPACK_PREFIX          "${BLAS_PREFIX}" ) # OpenBLAS has a LAPACK linker
# set( )

#ParMETIS
set(ParMETIS_DIR              "${CMAKE_SOURCE_DIR}/external/parmetis_4.0.3")
set(GKLIB_PATH                "${ParMETIS_DIR}/metis/GKlib")
set(METIS_PATH                "${ParMETIS_DIR}/metis")


#SuperLU_DIST
set(TPL_PARMETIS_INCLUDE_DIRS )
set(TPL_PARMETIS_INCLUDE_DIRS "${ParMETIS_DIR}/include;${ParMETIS_DIR}/metis/include")
set(TPL_PARMETIS_LIBRARIES    "${CMAKE_INSTALL_PREFIX}/lib/libparmetis.a;${BUILDDIR}/libmetis/libmetis.a")
set(TPL_ENABLE_PARMETISLIB    ON)
set(CMAKE_LINKER              mpicxx)
set(enable_openmp             ON)
set(TPL_ENABLE_BLASLIB        OFF)
# set(TPL_BLAS_LIBRARIES        "-L${BLAS_PREFIX}/lib -lopenblas")
