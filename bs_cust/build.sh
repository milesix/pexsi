#!/bin/bash

# MPI paths (openmpi) 
# export MPI_HOME=/usr/lib/x86_64-linux-gnu/openmpi/
# export MPI_INCLUDE_PATH=/usr/lib/x86_64-linux-gnu/openmpi/include/
# export MPI_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/openmpi/lib/
# export MPIEXEC=/usr/bin/mpiexec

# MPI paths (mpich) 
export MPI_INCLUDE_PATH="/usr/include/x86_64-linux-gnu/mpich"
export MPI_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu"


# Pull in environment variables
# MPICC=$(MPI_HOME)/bin/mpicc
# MPICXX=$(MPI_HOME)/bin/mpic++
# MPIFORT=$(MPI_HOME)/bin/mpifort
# MPIRUN=$(MPIRUN)

HDIR=$(realpath .)
mkdir -p ../pexsi_install/v2.1_gnu
PEXSI_INSTALL_DIR=$(realpath ../pexsi_install/v2.1_gnu)

BDIR=$HDIR/build_gnu
rm -rf ${BDIR}

# Set MPI environment variables
export LD_LIBRARY_PATH=${MPI_LIBRARY_PATH}:${LD_LIBRARY_PATH}
export PATH=${MPI_HOME}/bin:${PATH}

# Run CMake with MPI options
cmake -H${HDIR} -B${BDIR} \
  -DCMAKE_EXE_LINKER_FLAGS="-L${MPI_LIBRARY_PATH} -lmpi" \
  -DCMAKE_TOOLCHAIN_FILE=$HDIR/perlmutter_cpu_gnu.cmake \
  -DCMAKE_INSTALL_PREFIX=$PEXSI_INSTALL_DIR \
  -DMPI_CXX_COMPILER=$(which mpicxx) \
  -DMPI_Fortran_COMPILER=$(which mpifort) \
  -DMPI_INCLUDE_PATH=${MPI_INCLUDE_PATH} \
  -DMPI_LIBRARY=${MPI_LIBRARY_PATH}/libmpi.so

# Check if CMake configuration was successful
if [ $? -ne 0 ]; then
  echo "CMake configuration failed. Please check the CMake options and paths."
  exit 1
fi

# Build the project
cd ${BDIR}
make -j 12

# Check if the build was successful
if [ $? -ne 0 ]; then
  echo "Build failed. Please check the build process for errors."
  exit 1
fi

# Install the project
make install

# Check if the installation was successful
if [ $? -ne 0 ]; then
  echo "Installation failed. Please check the installation process for errors."
  exit 1
fi

cd ..

