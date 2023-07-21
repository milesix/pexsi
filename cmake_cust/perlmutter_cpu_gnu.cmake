# PEXSI
# Typical Build Environment for GNU+Linux systems with GNU compilers

# Specify the MPI Fortran compiler
SET(CMAKE_Fortran_COMPILER "mpifort" CACHE STRING "MPI Fortran compiler")

# Specify the MPI C compiler
SET(CMAKE_C_COMPILER "mpicc" CACHE STRING "MPI C compiler")

# Specify the MPI C++ compiler
SET(CMAKE_CXX_COMPILER "mpicxx" CACHE STRING "MPI C++ compiler")

# Set OpenMP support (if required)
set(PEXSI_ENABLE_OPENMP    OFF)

# Specify the linker
set(CMAKE_LINKER           "gcc")

# Set the path to the LAPACK library
set(LAPACK_LIBRARIES "/usr/lib/x86_64-linux-gnu/lapack/liblapack.so.3" CACHE STRING "LAPACK library chosen")


# Set the path to the BLAS library
set(BLAS_LIBRARIES "/usr/lib/x86_64-linux-gnu/libblas.so.3" CACHE STRING "BLAS library chosen")

# Set the compiler flags for Fortran
SET(CMAKE_Fortran_FLAGS "-O3 -funroll-loops -funsafe-math-optimizations -fallow-argument-mismatch" CACHE STRING "Fortran flags")

# Set the compiler flags for C
SET(CMAKE_C_FLAGS " -O3 -ffast-math" CACHE STRING "C flags")

# Set the compiler flags for C++
SET(CMAKE_CXX_FLAGS " -O3 -ffast-math" CACHE STRING "C++ flags")

# Set the PEXSI directory
set(PEXSI_DIR              "$ENV{HDIR}" CACHE STRING "PEXSI directory")

# Set the SuperLU_DIST prefix
set( SuperLU_DIST_PREFIX    "${PEXSI_DIR}/external/SuperLU_DIST_8.1.0" )

# Set the ParMETIS prefix
set(ParMETIS_PREFIX        "${PEXSI_DIR}/external/parmetis_4.0.3" )

# Enable Fortran support (if required)
set(PEXSI_ENABLE_FORTRAN   ON )

# Set ParMETIS directory
set(ParMETIS_DIR              "${ParMETIS_PREFIX}")

# Set GKlib path
set(GKLIB_PATH                "${ParMETIS_DIR}/metis/GKlib")

# Set METIS path
set(METIS_PATH                "${ParMETIS_DIR}/metis")

# Set TPL ParMETIS include directories
set(TPL_PARMETIS_INCLUDE_DIRS "${ParMETIS_DIR}/include;${ParMETIS_DIR}/metis/include")

# Set TPL ParMETIS libraries
set(TPL_PARMETIS_LIBRARIES    "${CMAKE_INSTALL_PREFIX}/lib/libparmetis.a;${BUILDDIR}/libmetis/libmetis.a")

# Enable ParMETIS library
set(TPL_ENABLE_PARMETISLIB    ON)

# Set OpenMP support (if required)
set(enable_openmp             OFF)

# Enable/Disable BLAS library usage
set(TPL_ENABLE_BLASLIB        OFF)

