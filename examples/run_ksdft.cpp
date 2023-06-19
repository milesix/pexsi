/*
   Copyright (c) 2012 The Regents of the University of California,
   through Lawrence Berkeley National Laboratory.  

Author: Lin Lin

This file is part of PEXSI. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

(1) Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
(2) Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
(3) Neither the name of the University of California, Lawrence Berkeley
National Laboratory, U.S. Dept. of Energy nor the names of its contributors may
be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

You are under no obligation whatsoever to provide any bug fixes, patches, or
upgrades to the features, functionality or performance of the source code
("Enhancements") to anyone; however, if you choose to make your Enhancements
available either publicly, or directly to Lawrence Berkeley National
Laboratory, without imposing a separate written license agreement for such
Enhancements, then you hereby grant the following license: a non-exclusive,
royalty-free perpetual license to install, use, modify, prepare derivative
works, incorporate into other computer software, distribute, and sublicense
such enhancements or derivative works thereof, in binary and source code form.
 */
/// @file run_ksdft.cpp
/// @brief Similar to driver_ksdft_2 but test the CPP interface
/// directly.
///
/// This can be used to test the correctness of new CPP implementation
/// without going through the interfaces.
///
/// @date 2015-12-07. Derived from run_ppexsi.cpp
/// @date 2023-06-17. Update to DFTDriver2 and allow command line input
/// to run multiple tests.
#include "ppexsi.hpp"
#include "c_pexsi_interface.h"

using namespace PEXSI;
using namespace std;


int main(int argc, char **argv) 
{
 
  
  MPI_Init(&argc, &argv);
  int mpirank, mpisize;
  MPI_Comm_rank( MPI_COMM_WORLD, &mpirank );
  MPI_Comm_size( MPI_COMM_WORLD, &mpisize );

  // Status file

  std::ofstream wrapperOFS;
  string ofsName = "logwrapper" + std::to_string(mpirank);
  wrapperOFS.open( ofsName );

  if( mpirank == 0 )
    std::cout << "Wrapper output file name: " << ofsName << endl;

  wrapperOFS << "This is a wrapper for DFTDriver2. " << endl 
    << "It can be used to test the performance of PEXSI for dumped out H, S matrices." << endl << endl;
  wrapperOFS << "Usage(options in brakets optional input parameters):" << endl << endl
    << "run_ksdft "
    << "-H <Hfile> [-S [Sfile]]"
    << "-T [isText {0, 1}] "
    << "-ne [numElectronExact] "
    << "[-r [nprow]] [-c [npcol]] "
    << "[-npole [numPole]] "
    << "[-npoint [nPoints]] "
    << "[-mumin [muMin0]] [-mumax [muMax0]] " 
    << "[-ord [ordering {0,1,2,3}]] "
    << "[-npsym [npSymbFact]] "
    << "[-de [deltaE]] "
    << "[-temp [temperature]] "
    << "[-tolint [muInertiaTolerance]] "
    << "[-tolne [numElectronPEXSITolerance]] "
    << "[-verb [verbosity] {0,1,2}] "
    << endl << endl;

  wrapperOFS << "User input: " << endl;
  for (int i = 0; i < argc; i++) {
    wrapperOFS << argv[i] << " ";
  }
  wrapperOFS << endl << endl;

  try{
 
    // *********************************************************************
    // Read input parameter via parser
    // *********************************************************************

    std::map<std::string,std::string> parser;
    std::string argVal;
    OptionsCreate(argc, argv, parser); // this is for parser

    PPEXSIOptions options;
    PPEXSISetDefaultOptions( &options ); // default PEXSI options

    int nprow = 1;
    argVal = parser["-r"];
    if( argVal.empty()  )
      wrapperOFS << "Use default value for -r " << nprow << endl;
    else
      nprow = std::stoi(argVal);
  
    int npcol = 1;
    argVal = parser["-c"];
    if( argVal.empty()  )
      wrapperOFS << "Use default value for -c " << npcol << endl;
    else
      npcol = std::stoi(argVal);
     
    std::string Hfile;
    argVal = parser["-H"];
    if( argVal.empty() )
      ErrorHandling("Hfile must be provided.");
    else
      Hfile = argVal;

    int isSIdentity = 0;
    std::string Sfile;
    argVal = parser["-S"];
    if( argVal.empty() ){
      wrapperOFS << "Use default value for -S: " 
        << "Treat the overlap matrix as an identity matrix." << endl;
      isSIdentity = 1;
    }
    else
      Sfile = argVal;

    int isText;
    argVal = parser["-T"];
    if( argVal.empty() )
      ErrorHandling( "-T needs to be provided. " );
    else
      isText = std::stoi(argVal);

    int numElectronExact;
    argVal = parser["-ne"];
    if( argVal.empty() )
      ErrorHandling( "-ne needs to be provided." );
    else
      numElectronExact = std::stoi(argVal);

    argVal = parser["-npole"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -npole " << options.numPole << endl;
    else
      options.numPole = std::stoi(argVal);

    argVal = parser["-npoint"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -npoint " << options.nPoints << endl;
    else
      options.nPoints = std::stoi(argVal);

    argVal = parser["-mumin"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -mumin " << options.muMin0 << endl;
    else
      options.muMin0 = std::stod(argVal);
    
    argVal = parser["-mumax"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -mumax " << options.muMax0 << endl;
    else
      options.muMax0 = std::stod(argVal);


    argVal = parser["-npsym"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -npsym " << options.npSymbFact << endl;
    else
      options.npSymbFact  = std::stoi(argVal);

    argVal = parser["-ord"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -ord " << options.ordering 
        << " (0 means PARMETIS)" << endl;
    else
      options.ordering = std::stoi(argVal);

    argVal = parser["-de"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -de " << options.deltaE << endl;
    else
      options.deltaE = std::stod(argVal);

    argVal = parser["-temp"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -temp " << options.temperature << endl;
    else
      options.temperature = std::stod(argVal);

    argVal = parser["-tolint"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -tolint " << options.muInertiaTolerance << endl;
    else
      options.muInertiaTolerance = std::stod(argVal);

    argVal = parser["-tolne"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -tolne " << options.numElectronPEXSITolerance << endl;
    else
      options.numElectronPEXSITolerance = std::stod(argVal);

    argVal = parser["-verb"];
    if( argVal.empty() )
      wrapperOFS << "Use default value for -verb " << options.verbosity << endl;
    else
      options.verbosity= std::stoi(argVal);


    // Check mpisize
    int npPerPole = nprow * npcol;
    if( mpisize % npPerPole  != 0 )
      ErrorHandling("mpisize must be an integer multiple of nprow * npcol." );
    if( mpisize % options.nPoints != 0 )
      ErrorHandling("mpisize must be an integer multiple of nPoints." );


    // *********************************************************************
    // Read in matrix
    // *********************************************************************
    int           isProcRead;
    MPI_Comm      readComm;
    int           numColLocal; 
    DistSparseMatrix<Real> HMat;
    DistSparseMatrix<Real> SMat;
    
    // Only the first pole group reads matrix
    if( mpirank < npPerPole  )
      isProcRead = 1;
    else
      isProcRead = 0;

    MPI_Comm_split( MPI_COMM_WORLD, isProcRead, mpirank, &readComm );

    if( isProcRead == 1 ){
      wrapperOFS << "Proc " << mpirank << " is reading file " << Hfile << endl;
      if( isText == 1 ){
        ReadDistSparseMatrixFormatted( Hfile.c_str(), 
            HMat, readComm ); 
      }
      else{
        ReadDistSparseMatrix( Hfile.c_str(), HMat, readComm ); 
      }

      numColLocal = HMat.colptrLocal.m() - 1;

      wrapperOFS << "Finish reading H matrix." << endl;

      if( isSIdentity == 0 ){
        wrapperOFS << "Proc " << mpirank << " is reading file " << Sfile << endl;
        if( isText == 1 ){
          ReadDistSparseMatrixFormatted( Sfile.c_str(), SMat, readComm ); 
        }
        else{
          ReadDistSparseMatrix( Sfile.c_str(), SMat, readComm ); 
        }
        wrapperOFS << "Finish reading S matrix." << endl;

        if( options.verbosity >= 1 ){
          wrapperOFS << "nrows       = " << HMat.size << endl;
          wrapperOFS << "nnz         = " << HMat.nnz  << endl;
          wrapperOFS << "nnzLocal    = " << HMat.nnzLocal << endl;
          wrapperOFS << "numColLocal = " << numColLocal << endl;
        }
      }

    } // Read the matrix

    MPI_Comm_free( &readComm );

    // *********************************************************************
    // Initialize PEXSI
    // *********************************************************************

    /* Set the outputFileIndex to be the pole index */
    /* The first processor for each pole outputs information */

    int outputFileIndex;
    if( mpirank % (nprow * npcol) == 0 ){
      outputFileIndex = mpirank / (nprow * npcol);
    }
    else{
      outputFileIndex = -1;
    }

    wrapperOFS << "Initializing PEXSI." << endl;
    PPEXSIData pexsi( MPI_COMM_WORLD, nprow, npcol, outputFileIndex );

    pexsi.LoadRealMatrix(
        HMat.size,                        
        HMat.nnz,                          
        HMat.nnzLocal,                     
        numColLocal,                  
        HMat.colptrLocal.Data(),
        HMat.rowindLocal.Data(),
        HMat.nzvalLocal.Data(),
        isSIdentity,
        SMat.nzvalLocal.Data(),
        options.solver,
        options.verbosity );
    
    wrapperOFS << "Finish loading matrix into PEXSI." << endl;

    // *********************************************************************
    // Solve
    // *********************************************************************
    Real    muPEXSI;
    Real    numElectronPEXSI;
    Real    muMinInertia = options.muMin0;
    Real    muMaxInertia = options.muMax0;
    int     numInertiaIter;

    // PEXSI Solve
    // FIXME Should just use PPEXSIOptions
    pexsi.DFTDriver2(
        numElectronExact,
        options.temperature,
        options.gap,
        options.deltaE,
        options.numPole,
        options.muInertiaTolerance,
        options.numElectronPEXSITolerance,
        options.matrixType,
        options.isSymbolicFactorize,
        options.solver,
        options.symmetricStorage,
        options.ordering,
        options.npSymbFact,
        options.verbosity,
        muPEXSI,           // updated
        numElectronPEXSI,  // updated
        muMinInertia,      // updated
        muMaxInertia,      // updated
        numInertiaIter,    // updated
        options.method,
        options.nPoints,
        options.spin);

    // Retrieve data
    wrapperOFS << "Output from DFT calculation" << endl;

    Print(wrapperOFS, "muPEXSI               = ", muPEXSI);
    Print(wrapperOFS, "numElectronPEXSI      = ", numElectronPEXSI);
    Print(wrapperOFS, "muMinInertia          = ", muMinInertia);
    Print(wrapperOFS, "muMinInertia          = ", muMaxInertia);
    Print(wrapperOFS, "numInertiaIter        = ", numInertiaIter);
    Print(wrapperOFS, "Total energy (H*DM)   = ", pexsi.TotalEnergyH());
    Print(wrapperOFS, "Total energy (S*EDM)  = ", pexsi.TotalEnergyS());
    Print(wrapperOFS, "Total free            = ", pexsi.TotalFreeEnergy());
    
    wrapperOFS << "Output from DFT calculation" << endl;

  }
  catch( std::exception& e )
  {
    // std::cerr << "Error message in " << ofsName << endl;
    std::cerr  << endl << " ERROR!!! Proc " << mpirank << " caught exception with message: " << endl
      << e.what() << endl;
  }

  wrapperOFS.close();
  MPI_Finalize();

  return 0;
}
