Example
=======
\tableofcontents

**THIS IS OUT-OF-DATE and will be removed later**

\section sec_lap2d_seq Sequential example: 2D Laplacian

Description
-----------

**Note** The sequential %PEXSI code is out-of date. 3/26/2013

`lap2d_seq.csc` stores the matrix of a two dimensional Laplacian
operator with Dirichlet boundary condition on a domain \f$[0,10*\pi] *
[0,10*\pi]\f$, discretized using a five point stencil on a 20 * 20 grid.
Only the lower triangular part and the diagonal part of the matrix
is given as required by SelInv.

`identity_seq.csc` stores the identity matrix (as overlap matrix)
which has **exactly the same sparsity pattern** as `lap2d_seq.csc`.

These matrices can be constructed following the procedure in \ref
sec_generate_matrix.

Running the test
----------------

Assume the number of electrons (Ne) is 20, and the temperature (T) is
300K.  

    $ aprun -n 1 ${PEXSI_DIR}/examples/run_pexsi -temp 300 -mu0 0.16
    -numel 20 -numPole 80 -deltaE 10 -gap 0 -H lap2d_seq.csc -S
    identity_seq.csc -muiter 30

would generate a log file `logPEXSI0`.

    $ tail logPEXSI0 

    *********************************************************************
    Solve finished.
    *********************************************************************

    PEXSI has converged with 7 iterations
    mu                   = +1.71461149e-01 
    Total time for PEXSI = +1.06287694e+00

Compare with the true chemical potential 1.714611991618183e-01, the
error is in the order of 1e-8.

\section sec_lap2d_par Parallel example: 2D Laplacian

Description
-----------

`lap2d_par.matrix` stores the matrix of a two dimensional Laplacian
operator with Dirichlet boundary condition on a domain [0,10*pi] *
[0,10*pi], discretized using a five point stencil on a 20 * 20 grid.
This is the standard compressed sparse column (CSC) format that can
be directly read by run_ppexsi.

Running the test
----------------

Assume the number of electrons (Ne) is 20, and the temperature (T) is
300K.  

Run the parallel code sequentially:
    
    $ aprun -n 1 -temp 300 -mu0 0.16 -muMin -2 -muMax 2 -numel 20 
      -numPole 80 -deltaE 10 -gap 0 -H lap2d_par.matrix -npPerPole 1 
      -colperm MMD_AT_PLUS_A -muiter 30 -formatted 1

Check the log file (only `logPEXSI0` is generated)

    $ tail logPEXSI0 

    *********************************************************************
    Solve finished.
    *********************************************************************

    PEXSI has converged with 7 iterations
    mu                   = +3.90691982e-01
    
**Note** The following test results should be UPDATED. 3/26/2013

Run the parallel code with 16 processors and each processor work
with one pole at a time:

    $ aprun -n 16 ${PEXSI_DIR}/examples/run_ppexsi -temp 300 -mu0
    0.16 -muMin -2 -muMax 2 -numel 20 -numPole 80 -deltaE 10 -gap 0
    -H lap2d_par.csc -S identity_par.csc -npPerPole 1 -colperm
    MMD_AT_PLUS_A -muiter 30

Check the log file (16 log files `logPEXSI0`--`logPEXSI15` are
generated)

    $ tail logPEXSI0 

    *********************************************************************
    Solve finished.
    *********************************************************************


Run the parallel code with 16 processors working with 4 processors
working on each pole.

    $ aprun -n 16 ${PEXSI_DIR}/examples/run_ppexsi -temp 300 -mu0
    0.16 -muMin -2 -muMax 2 -numel 20 -numPole 80 -deltaE 10 -gap 0
    -H lap2d_par.csc -S identity_par.csc -npPerPole 4 -colperm
    MMD_AT_PLUS_A -muiter 30

Check the log file (16 log files `logPEXSI0`--`logPEXSI15` are
generated)

    $ tail logPEXSI0 

    *********************************************************************
    Solve finished.
    *********************************************************************

    PEXSI has converged with 7 iterations
    mu                   = +1.71461149e-01 
    Total time for PEXSI = +2.24112701e+00 

Note: The fact that the strong scaling is not preferable in this
case is mainly due to the small size of the system.

\section sec_generate_matrix Generate test matrices (csc format)

Here we show how to generate the matrices that can be used by PEXSI
using the utility subroutines in MATLAB, using the 2D Laplacian
matrix mentioned before as example.

Generate the matrix
-------------------

In MATLAB,

    >> N=20;
    >> h=10*pi/(N+1);
    >> e=ones(N,1);
    >> lap1d=spdiags([-e 2*e -e],-1:1,N,N);
    >> lap2d=1/h^2*(kron(lap1d,eye(N))+kron(eye(N),lap1d));

The first few eigenvalues are

    >> D=eig(lap2d);
    >> D(1:5)

    ans=

         1.996272772260532e-02
         4.968385213154768e-02
         4.968385213155013e-02
         7.940497654049716e-02
         9.848081628152548e-02

The chemical potentential for a given number of electrons (Ne, assume
double occupancy) and at given temperature (T) can be computed by the
subroutine `getocc`.  

    >> Ne=20;
    >> T=300;
    >> au2K=315774.67;
    >> [occ,mu]=getocc(D,Ne/2,au2K/T);
    >> mu

    mu =

         1.714611991618183e-01


Convert to the sequential code format
-------------------------------------

    >> [colptr,rowind,~]=WriteSparseMatrix(lap2d,'lap2d_seq.csc');
    >> WriteSparseMatrix2(speye(size(lap2d)),'identity_seq.csc',colptr,rowind);

Convert to the parallel code format
-----------------------------------

    >> [colptr,rowind,~]=WriteSparseMatrixLU(lap2d,'lap2d_par.csc');
    >> WriteSparseMatrixLU2(speye(size(lap2d)),'identity_par.csc',colptr,rowind);

> NOTE: This is not true
>
> Here is the reason \f$LDL^{T}\f$
>
> Here is a span of the code Use the `printf()` function.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
a one-line code block
asd asd  s s dd  dk s `PEXSI`
printf(int a, int b);
~~~~~~~~~~~~~~~~~~~~~

`%PEXSI` code does not refer to PEXSI.
   