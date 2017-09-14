PEXSI: Pole EXpansion and Selected Inversion 
============================================

For full documentation, including instructions for installation please see

http://www.pexsi.org

PEXSI requires an external parallel :math:`LU` factorization or
:math:`LDL^T` factorization routine, and an external parallel matrix
reordering routine to reduce the fill-in of the factorization routine.

Starting from v1.0, PEXSI requires both symPACK and SuperLU_DIST.
symPACK is the default option for the :math:`LDL^T` factorization of
symmetric matrices, and use SuperLU_DIST as the default option for the
:math:`LU` factorization of unsymmetric matrices.  SuperLU_DIST can
also be used for symmetric matrices, by means of treating the matrix as
a general matrix but use symmetric reordering.

Starting from v1.0, PEXSI uses the PT-Scotch as the default package
for matrix reordering.  The ParMETIS package can also be used.

The installation procedure and dependencies of every version of the PEXSI
package may be slightly different. Please follow the documentation of the version
of the PEXSI package you are working with.
(provided in the :ref:`Download Page <pageDownload>` )


Build PT-Scotch
=============================

PT-Scotch can be downloaded from (latest version 6.0.0)
https://gforge.inria.fr/frs/download.php/31831/scotch_6.0.0.tar.gz

Follow the installation step to install PT-Scotch.
**In INSTALL.TXT, pay special attention to the following
sections in order to compile PT-Scotch correctly.**

    2.3) Integer size issues

    2.5) Threads issues


PT-Scotch is also METIS-Compatible.  See the following section in
INSTALL.TXT for more information.

    2.9) MeTiS compatibility library

In `src/` directory, you need
:: 
    make ptscotch 
    
to compile PT-Scotch.


**NOTE**: Just typing `make` will generate the Scotch library but not PT-Scotch.
Then all libraries will be given in `lib/` directory.**


Build symPACK
=============================


symPACK is a sparse symmetric matrix direct linear solver.
More information can be found at http://www.sympack.org/.

To use symPACK, first, download the package as follows
::
    git clone https://github.com/symPACK/symPACK.git  /path/to/sympack

Several environment variables can be set before configuring the build:
::
    `SCOTCH_DIR` = Installation directory for **SCOTCH** and **PT-SCOTCH**

Then, create a build directory, enter that directory and type:
::
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/path/to/install/sympack
    ...OPTIONS... /path/to/sympack


The `...OPTIONS...` can be one of the following:
::
    * `-DENABLE_METIS=ON|OFF`   to make **MeTiS** ordering available in **symPACK** (`METIS_DIR` must be set in the environment)

    * `-DENABLE_PARMETIS=ON|OFF`   to make **ParMETIS** ordering available in **symPACK** (`PARMETIS_DIR` must be set in the environment, `METIS_DIR` is required as well)

    * `-DENABLE_SCOTCH=ON|OFF`   to make **SCOTCH** / **PT-SCOTCH** orderings available in **symPACK** (`SCOTCH_DIR` must be set in the environment)



Some platforms have preconfigured toolchain files which can be used by adding the following option to the `cmake` command:
::
    -DCMAKE_TOOLCHAIN_FILE=/path/to/sympack/toolchains/edison.cmake     
    (To build on NERSC Edison for instance)


A sample toolchain file can be found in `/path/to/sympack/toolchains/build_config.cmake` and customized for the target platform.


The `cmake` command will configure the build process, which can now start by typing:
::
    make
    make install

Additionally, a standalone driver for **symPACK** can be built by typing `make examples`


Build SuperLU_DIST
======================


Download SuperLU_DIST (latest version 5.1.3) from

http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_dist_5.1.3.tar.gz

Follow the installation step to install SuperLU_DIST.

Our experience shows that on some machines it may be better
to build SuperLU_DIST with -O2 option than the more aggresive
optimization options provided by vendors.

 - In SuperLU_DIST v5.1.3, some functions conflict when both real
   and complex arithmetic factorization is needed. This can be temporarily
   solved by adding  `-Wl,--allow-multiple-definition` in the linking
   option.

 - In SuperLU_DIST v5.1.3, there could be some excessive outputs.
   This can be removed by going to the SRC/ directory of superlu, and
   comment out the line starting with `printf(".. dQuery_Space` in
   dmemory_dist.c. Do the same thing for the line starting with
   `printf(".. zQuery_Space..)` in zmemory_dist.c.

 - Please note that the number of processors for symbolic
   factorization cannot be too large when PARMETIS is used together with
   SuperLU. The exact number of processors for symbolic factorization is
   unfortunately a **magic parameter**. See :ref:`FAQ page <pageFAQ>`.



(Optional) Build ParMETIS
===============

Download ParMETIS (latest version 4.0.3) from

http://glaros.dtc.umn.edu/gkhome/fetch/sw/parmetis/parmetis-4.0.3.tar.gz

Follow the installation step to install ParMETIS.

**After untar the ParMETIS package, in Install.txt**

    Edit the file metis/include/metis.h and specify the width (32 or
    64 bits) of the elementary data type used in ParMetis (and
    METIS). This is controled by the IDXTYPEWIDTH constant.

    For now, on a 32 bit architecture you can only specify a width
    of 32, whereas for a 64 bit architecture you can specify a width
    of either 32 or 64 bits.

**In our experience for most cases, the following setup work
fine.**::

    #define IDXTYPEWIDTH 32


Build PEXSI
===========


Edit make.inc
=============================

Configuration of PEXSI is controlled by a single `make.inc` file.
Examples of the `make.inc` file are given under the `config/` directory.

Find `make.inc` with the most similar architecture, and copy to the main
PEXSI directory (using Edison for example, the latest Intel computer
at NERSC, a CRAY X30 machine).  `${PEXSI_DIR}` stands for the main
directory of PEXSI. ::

    cd ${PEXSI_DIR}
    cp config/make.inc.CRAY_XC30.intel make.inc

Edit the variables in make.inc.  ::
   
    PEXSI_DIR     = Main directory for PEXSI
    DSUPERLU_DIR  = Main directory for SuperLU_DIST
    PARMETIS_DIR  = Main directory for ParMETIS 
    PTSCOTCH_DIR  = Main directory for PT-Scotch

Edit the compiler options, for instance ::

    CC           = cc
    CXX          = CC
    FC           = ftn
    LOADER       = CC


The `USE_SYMPACK` option can be set to use the symPACK solver in
PEXSI. It is set to 0 by default. When set to 1, the `SYMPACK_DIR` variable
must be pointing to symPACK's installation directory.


**note**

- Starting from PEXSI v0.8.0, `-std=c++11` is required in `CXXFLAGS`. 

- Starting from PEXSI v0.9.2, `-std=c99` is required in `CFLAGS` to be
  compatible with SuperLU_DIST starting from v4.3.

- For **FORTRAN** users, `CPP_LIB=-lstdc++ -lmpi -lmpi_cxx` is often needed.
  Check this if there is link error.

- PEXSI can be compiled using `debug` or `release` mode in
  by the variable `COMPILE_MODE` in `make.inc`.  This variable mainly controls the
  compiling flag `-DRELEASE`.  The `debug` mode introduces tracing of call
  stacks at all levels of functions, and may significantly slow down the
  code.  For production runs, use `release` mode.

- The `USE_PROFILE` option is for internal test purpose. Usually set this to 0.


Build the PEXSI library
=============================

The installation procedure and dependencies of every version of the PEXSI
package may be different. Please follow the documentation of the version
of the PEXSI package you are working with 
(provided in the :ref:`Download Page <pageDownload>` )

If make.inc is configured correctly,::
    
    make 
    make install

Should build the PEXSI library under the `build` directory ready to be
used in an external package.  If the FORTRAN interface is needed, type::

    make finstall

If examples are needed (not necessary if you use PEXSI in an external
package), type ::

    make examples

which will generate C examples in `examples/` directory and FORTRAN examples in
`fortran/` directory, respectively.::

    make all

will make the library and the examples.

For more information on the examples, see :ref:`Tutorial Page <pageTutorial>`.

Tests
======

After example files are compiled, go to the `examples/` directory, and::

    examples$ mpirun -n 1 ./driver_pselinv_complex_(suffix)

should return the diagonal of the matrix
:math:`(A + i I)^{-1}`
saved on the 0-th processor, where :math:`A` is the five-point
discretization of a Laplacian operator on a 2D domain.  The result can
be compared with `examples/driver_pselinv_complex.out` to check the
correctness of the result. For more examples see :ref:`Tutorial Page <pageTutorial>`.

