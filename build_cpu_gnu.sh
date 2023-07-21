#!/bin/bash
export MPI_HOME=/usr/lib/x86_64-linux-gnu/openmpi/
export MPI_INCLUDE_PATH=/usr/lib/x86_64-linux-gnu/openmpi/include/
export MPI_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/openmpi/lib/

# module load espresso
HDIR=$(realpath .)
mkdir -p ../pexsi_install/v2.1_gnu
PEXSI_INSTALL_DIR=$(realpath ../pexsi_install/v2.1_gnu)
# export LIBSCI_LIBS=-L$CRAY_LIBSCI_PREFIX_DIR/lib

BDIR=$HDIR/build_gnu
rm -rf ${BDIR}

cmake -H${HDIR} -B${BDIR} \
  -DCMAKE_TOOLCHAIN_FILE=$HDIR/perlmutter_cpu_gnu.cmake \
  -DCMAKE_INSTALL_PREFIX=$PEXSI_INSTALL_DIR 
cd ${BDIR}
make -j 12
make install
cd ..
