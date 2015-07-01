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
/// @file interface.cpp
/// @brief Interface subroutines of PPEXSI that can be called by both C and FORTRAN.
///
/// This file will eventually merge with interface.cpp.
/// @date Original:      2013-02-03
/// @date Revision:      2014-01-01 
/// @date Revision:      2014-03-07  Second generation interface.
#include "c_pexsi_interface.h"
#include "ppexsi.hpp"
#include "pexsi/blas.hpp"

// FIXME
// Error handling used in the C interface that is different from the
// throw/catch system.
#define iC(fun)  { int ierr=fun; if(ierr!=0) exit(1); }
#define iA(expr) { if((expr)==0) { std::cerr<<"wrong "<<__LINE__<<" in " <<__FILE__<<std::endl; std::cerr.flush(); exit(1); } }

using namespace PEXSI;

extern "C"
void ReadDistSparseMatrixFormattedHeadInterface (
		char*    filename,
		int*     size,
		int*     nnz,
		int*     nnzLocal,
		int*     numColLocal,
		MPI_Comm comm )
{
  Int mpirank;  MPI_Comm_rank(comm, &mpirank);
  Int mpisize;  MPI_Comm_size(comm, &mpisize);
	std::ifstream fin;
	if( mpirank == 0 ){
		fin.open(filename);
		if( !fin.good() ){
			#ifdef USE_ABORT
abort();
#endif
throw std::logic_error( "File cannot be openeded!" );
		}
		Int dummy;
		fin >> *size >> dummy;
		fin >> *nnz >> dummy;
	}
	
	MPI_Bcast( &(*size), 1, MPI_INT, 0, comm);
	MPI_Bcast( &(*nnz),  1, MPI_INT, 0, comm);

	IntNumVec  colptr(*size+1);
	if( mpirank == 0 ){
		Int* ptr = colptr.Data();
		for( Int i = 0; i < *size+1; i++ )
			fin >> *(ptr++);
	}

	MPI_Bcast(colptr.Data(), *size+1, MPI_INT, 0, comm);

	// Compute the number of columns on each processor
	IntNumVec numColLocalVec(mpisize);
	Int numColFirst;
	numColFirst = *size / mpisize;
  SetValue( numColLocalVec, numColFirst );
  numColLocalVec[mpisize-1] = *size - numColFirst * (mpisize-1);  
	// Modify the last entry	

	*numColLocal = numColLocalVec[mpirank];

	*nnzLocal = colptr[mpirank * numColFirst + (*numColLocal)] - 
		colptr[mpirank * numColFirst];
	
	// Close the file
	if( mpirank == 0 ){
    fin.close();
	}

	return;
}  
// -----  end of function ReadDistSparseMatrixFormattedHeadInterface


extern "C"
void ReadDistSparseMatrixFormattedInterface(
		char*     filename,
		int       size,
		int       nnz,
		int       nnzLocal,
		int       numColLocal,
		int*      colptrLocal,
		int*      rowindLocal,
		double*   nzvalLocal,
		MPI_Comm  comm )
{
	DistSparseMatrix<Real> A;
	ReadDistSparseMatrixFormatted( filename, A, comm );
	iA( size == A.size );
	iA( nnz  == A.nnz  );
	iA( nnzLocal == A.nnzLocal );
	iA( numColLocal + 1 == A.colptrLocal.m() );
	
	blas::Copy( numColLocal+1, A.colptrLocal.Data(), 1,
			colptrLocal, 1 );

	blas::Copy( nnzLocal, A.rowindLocal.Data(), 1,
			rowindLocal, 1 );

	blas::Copy( nnzLocal, A.nzvalLocal.Data(), 1,
			nzvalLocal, 1 );

	return;
}  
// -----  end of function ReadDistSparseMatrixFormattedInterface  


extern "C"
void ReadDistSparseMatrixHeadInterface (
		char*    filename,
		int*     size,
		int*     nnz,
		int*     nnzLocal,
		int*     numColLocal,
		MPI_Comm comm )
{
  Int mpirank;  MPI_Comm_rank(comm, &mpirank);
  Int mpisize;  MPI_Comm_size(comm, &mpisize);
	std::ifstream fin;
	if( mpirank == 0 ){
		fin.open(filename);
		if( !fin.good() ){
			#ifdef USE_ABORT
abort();
#endif
throw std::logic_error( "File cannot be openeded!" );
		}
		fin.read((char*)size, sizeof(int));
		fin.read((char*)nnz,  sizeof(int));
	}
	
	MPI_Bcast( &(*size), 1, MPI_INT, 0, comm);
	MPI_Bcast( &(*nnz),  1, MPI_INT, 0, comm);

	IntNumVec  colptr(*size+1);
	if( mpirank == 0 ){
		Int tmp;
		fin.read((char*)&tmp, sizeof(int));  

		if( tmp != (*size)+1 ){
			#ifdef USE_ABORT
abort();
#endif
throw std::logic_error( "colptr is not of the right size." );
		}

		Int* ptr = colptr.Data();
		fin.read((char*)ptr, (*size+1) * sizeof(int));  
	}

	MPI_Bcast(colptr.Data(), *size+1, MPI_INT, 0, comm);

	// Compute the number of columns on each processor
	IntNumVec numColLocalVec(mpisize);
	Int numColFirst;
	numColFirst = *size / mpisize;
  SetValue( numColLocalVec, numColFirst );
  numColLocalVec[mpisize-1] = *size - numColFirst * (mpisize-1);  
	// Modify the last entry	

	*numColLocal = numColLocalVec[mpirank];

	*nnzLocal = colptr[mpirank * numColFirst + (*numColLocal)] - 
		colptr[mpirank * numColFirst];
	
	// Close the file
	if( mpirank == 0 ){
    fin.close();
	}

	return;
}  
// -----  end of function ReadDistSparseMatrixHeadInterface

extern "C"
void ParaReadDistSparseMatrixInterface(
		char*     filename,
		int       size,
		int       nnz,
		int       nnzLocal,
		int       numColLocal,
		int*      colptrLocal,
		int*      rowindLocal,
		double*   nzvalLocal,
		MPI_Comm  comm )
{
	DistSparseMatrix<Real> A;
	ParaReadDistSparseMatrix( filename, A, comm );
	iA( size == A.size );
	iA( nnz  == A.nnz  );
	iA( nnzLocal == A.nnzLocal );
	iA( numColLocal + 1 == A.colptrLocal.m() );
	
	blas::Copy( numColLocal+1, A.colptrLocal.Data(), 1,
			colptrLocal, 1 );

	blas::Copy( nnzLocal, A.rowindLocal.Data(), 1,
			rowindLocal, 1 );

	blas::Copy( nnzLocal, A.nzvalLocal.Data(), 1,
			nzvalLocal, 1 );

	return;
}  
// -----  end of function ReadDistSparseMatrixFormattedInterface  


// *********************************************************************
// Second interface
// *********************************************************************

extern "C"
void PPEXSISetDefaultOptions(
    PPEXSIOptions*   options ){
  options->temperature           = 0.0019;   // 300K 
  options->gap                   = 0.0;      // no gap 
  options->deltaE                = 10.0; 
  options->numPole               = 40;
  options->isInertiaCount        = 1;
  options->maxPEXSIIter          = 3;
  options->muMin0                = -10.0; 
  options->muMax0                = +10.0; 
  options->mu0                   =  0.0;  // For more control by the user
  options->muInertiaTolerance    = 0.05;
  options->muInertiaExpansion    = 0.3;      
  options->muPEXSISafeGuard      = 0.05;
  options->numElectronPEXSITolerance = 0.01;
  options->matrixType            = 0;
  options->isSymbolicFactorize   = 1;
  options->ordering              = 0;
  options->rowOrdering           = 0;
  options->npSymbFact            = 1;
  options->symmetric             = 1;
  options->verbosity             = 1;
}   // -----  end of function PPEXSISetDefaultOptions  ----- 


extern "C"
PPEXSIPlan PPEXSIPlanInitialize(
    MPI_Comm      comm,
    int           numProcRow,
    int           numProcCol,
    int           outputFileIndex, 
    int*          info ){

  Int mpirank, mpisize;
  MPI_Comm_rank( comm, &mpirank );
  MPI_Comm_size( comm, &mpisize );

  *info = 0;
  PPEXSIData *ptrData;

  try{
    ptrData = new PPEXSIData( comm, numProcRow, numProcCol, outputFileIndex );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << mpirank << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

  return reinterpret_cast<PPEXSIPlan>(ptrData);
}   // -----  end of function PPEXSIPlanInitialize  ----- 


extern "C"
void PPEXSILoadRealSymmetricHSMatrix(
    PPEXSIPlan    plan,
    PPEXSIOptions options,
    int           nrows,                        
    int           nnz,                          
    int           nnzLocal,                     
    int           numColLocal,                  
    int*          colptrLocal,                  
    int*          rowindLocal,                  
    double*       HnzvalLocal,                  
    int           isSIdentity,                  
    double*       SnzvalLocal, 
    int*          info ){

  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    reinterpret_cast<PPEXSIData*>(plan)->
      LoadRealSymmetricMatrix(
          nrows,                        
          nnz,                          
          nnzLocal,                     
          numColLocal,                  
          colptrLocal,                  
          rowindLocal,                  
          HnzvalLocal,                  
          isSIdentity,                  
          SnzvalLocal,
          options.verbosity );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

  return;
}   // -----  end of function PPEXSILoadRealSymmetricHSMatrix  ----- 


extern "C"
void PPEXSILoadRealUnsymmetricHSMatrix(
    PPEXSIPlan    plan,
    PPEXSIOptions options,
    int           nrows,                        
    int           nnz,                          
    int           nnzLocal,                     
    int           numColLocal,                  
    int*          colptrLocal,                  
    int*          rowindLocal,                  
    double*       HnzvalLocal,                  
    int           isSIdentity,                  
    double*       SnzvalLocal, 
    int*          info ){

  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    reinterpret_cast<PPEXSIData*>(plan)->
      LoadRealUnsymmetricMatrix(
          nrows,                        
          nnz,                          
          nnzLocal,                     
          numColLocal,                  
          colptrLocal,                  
          rowindLocal,                  
          HnzvalLocal,                  
          isSIdentity,                  
          SnzvalLocal,
          options.verbosity );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

  return;
}   // -----  end of function PPEXSILoadRealUnsymmetricHSMatrix  ----- 





extern "C"
void PPEXSISymbolicFactorizeRealSymmetricMatrix(
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    int*              info ) {
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    std::string colPerm;
    switch (options.ordering){
      case 0:
        colPerm = "PARMETIS";
        break;
      case 1:
        colPerm = "METIS_AT_PLUS_A";
        break;
      case 2:
        colPerm = "MMD_AT_PLUS_A";
        break;
      default:
        throw std::logic_error("Unsupported ordering strategy.");
    }

    reinterpret_cast<PPEXSIData*>(plan)->
      SymbolicFactorizeRealSymmetricMatrix(
          colPerm,
          options.npSymbFact,
          options.verbosity );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISymbolicFactorizeRealSymmetricMatrix  ----- 

extern "C"
void PPEXSISymbolicFactorizeRealUnsymmetricMatrix(
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    int*              info ) {
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    std::string colPerm;
    switch (options.ordering){
      case 0:
        colPerm = "PARMETIS";
        break;
      case 1:
        colPerm = "METIS_AT_PLUS_A";
        break;
      case 2:
        colPerm = "MMD_AT_PLUS_A";
        break;
      default:
        throw std::logic_error("Unsupported ordering strategy.");
    }


    std::string rowPerm;
    switch (options.rowOrdering){
      case 0:
        rowPerm = "NOROWPERM";
        break;
      case 1:
        rowPerm = "LargeDiag";
        break;
      default:
        throw std::logic_error("Unsupported row ordering strategy.");
    }



    reinterpret_cast<PPEXSIData*>(plan)->
      SymbolicFactorizeRealUnsymmetricMatrix(
          colPerm,
          rowPerm,
          options.npSymbFact,
          options.verbosity );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISymbolicFactorizeRealUnsymmetricMatrix  ----- 



extern "C"
void PPEXSISymbolicFactorizeComplexSymmetricMatrix(
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    int*              info ) {
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    std::string colPerm;
    switch (options.ordering){
      case 0:
        colPerm = "PARMETIS";
        break;
      case 1:
        colPerm = "METIS_AT_PLUS_A";
        break;
      case 2:
        colPerm = "MMD_AT_PLUS_A";
        break;
      default:
        throw std::logic_error("Unsupported ordering strategy.");
    }

    reinterpret_cast<PPEXSIData*>(plan)->
      SymbolicFactorizeComplexSymmetricMatrix(
          colPerm,
          options.npSymbFact,
          options.verbosity );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISymbolicFactorizeRealSymmetricMatrix  ----- 

extern "C"
void PPEXSISymbolicFactorizeComplexUnsymmetricMatrix(
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    int*              info ) {
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    std::string colPerm;
    switch (options.ordering){
      case 0:
        colPerm = "PARMETIS";
        break;
      case 1:
        colPerm = "METIS_AT_PLUS_A";
        break;
      case 2:
        colPerm = "MMD_AT_PLUS_A";
        break;
      default:
        throw std::logic_error("Unsupported ordering strategy.");
    }

    std::string rowPerm;
    switch (options.rowOrdering){
      case 0:
        rowPerm = "NOROWPERM";
        break;
      case 1:
        rowPerm = "LargeDiag";
        break;
      default:
        throw std::logic_error("Unsupported row ordering strategy.");
    }


    reinterpret_cast<PPEXSIData*>(plan)->
      SymbolicFactorizeComplexUnsymmetricMatrix(
          colPerm,
          rowPerm,
          options.npSymbFact,
          options.verbosity );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISymbolicFactorizeRealUnsymmetricMatrix  ----- 



extern "C"
void PPEXSIInertiaCountRealSymmetricMatrix(
    /* Input parameters */
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    int               numShift,
    double*           shiftList,
    /* Output parameters */
    double*           inertiaList,
    int*              info ) {

  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    std::vector<Real>    shiftVec(numShift);
    std::vector<Real>    inertiaVec(numShift);
    for( Int i = 0; i < numShift; i++ ){
      shiftVec[i] = shiftList[i];
    }

    reinterpret_cast<PPEXSIData*>(plan)->
      CalculateNegativeInertiaReal(
          shiftVec,
          inertiaVec,
          options.verbosity );

    for( Int i = 0; i < numShift; i++ ){
      inertiaList[i] = inertiaVec[i];
    }
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSIInertiaCountRealSymmetricMatrix  ----- 

extern "C"
void PPEXSIInertiaCountRealUnsymmetricMatrix(
    /* Input parameters */
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    int               numShift,
    double*           shiftList,
    /* Output parameters */
    double*           inertiaList,
    int*              info ) {

  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  *info = 0;

  try{
    std::vector<Real>    shiftVec(numShift);
    std::vector<Real>    inertiaVec(numShift);
    for( Int i = 0; i < numShift; i++ ){
      shiftVec[i] = shiftList[i];
    }

    reinterpret_cast<PPEXSIData*>(plan)->
      CalculateNegativeInertiaReal(
          shiftVec,
          inertiaVec,
          options.verbosity );

    for( Int i = 0; i < numShift; i++ ){
      inertiaList[i] = inertiaVec[i];
    }
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSIInertiaCountRealUnsymmetricMatrix  ----- 


extern "C"
void PPEXSICalculateFermiOperatorReal(
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    double            mu,
    double            numElectronExact,
    double*           numElectronPEXSI,
    double*           numElectronDrvMuPEXSI,
    int*              info )
{
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  try{
    reinterpret_cast<PPEXSIData*>(plan)->CalculateFermiOperatorReal(
        options.numPole,
        options.temperature,
        options.gap,
        options.deltaE,
        mu,
        numElectronExact,
        options.numElectronPEXSITolerance,
        options.verbosity,
        *numElectronPEXSI,
        *numElectronDrvMuPEXSI );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSICalculateFermiOperatorReal  ----- 



extern "C"
void PPEXSISelInvRealSymmetricMatrix (
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    double*           AnzvalLocal,                  
    double*           AinvnzvalLocal,
    int*              info )
{
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  try{
    reinterpret_cast<PPEXSIData*>(plan)->SelInvRealSymmetricMatrix(
        AnzvalLocal,
        options.verbosity,
        AinvnzvalLocal );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISelInvRealSymmetricMatrix  ----- 

extern "C"
void PPEXSISelInvRealUnsymmetricMatrix (
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    double*           AnzvalLocal,                  
    double*           AinvnzvalLocal,
    int*              info )
{
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  try{
    reinterpret_cast<PPEXSIData*>(plan)->SelInvRealUnsymmetricMatrix(
        AnzvalLocal,
        options.verbosity,
        AinvnzvalLocal );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISelInvRealUnsymmetricMatrix  ----- 

extern "C"
void PPEXSISelInvComplexSymmetricMatrix (
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    double*           AnzvalLocal,                  
    double*           AinvnzvalLocal,
    int*              info )
{
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  try{
    reinterpret_cast<PPEXSIData*>(plan)->SelInvComplexSymmetricMatrix(
        AnzvalLocal,
        options.verbosity,
        AinvnzvalLocal );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISelInvComplexSymmetricMatrix  ----- 

extern "C"
void PPEXSISelInvComplexUnsymmetricMatrix (
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    double*           AnzvalLocal,                  
    double*           AinvnzvalLocal,
    int*              info )
{
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();

  try{
    reinterpret_cast<PPEXSIData*>(plan)->SelInvComplexUnsymmetricMatrix(
        AnzvalLocal,
        options.verbosity,
        AinvnzvalLocal );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}

	return ;
}		// -----  end of function PPEXSISelInvComplexUnsymmetricMatrix  ----- 


extern "C"
void PPEXSIDFTDriver(
    /* Input parameters */
    PPEXSIPlan        plan,
    PPEXSIOptions     options,
    double            numElectronExact,
    /* Output parameters */
		double*           muPEXSI,                   
		double*           numElectronPEXSI,         
    double*           muMinInertia,              
		double*           muMaxInertia,             
		int*              numTotalInertiaIter,   
		int*              numTotalPEXSIIter,   
    int*              info ){
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();
  
  try{
    reinterpret_cast<PPEXSIData*>(plan)->DFTDriver(
        numElectronExact,
        options.temperature,
        options.gap,
        options.deltaE,
        options.numPole,
        options.isInertiaCount,
        options.maxPEXSIIter,
        options.muMin0,
        options.muMax0,
        options.mu0,
        options.muInertiaTolerance,
        options.muInertiaExpansion,
        options.muPEXSISafeGuard,
        options.numElectronPEXSITolerance,
        options.matrixType,
        options.isSymbolicFactorize,
        options.ordering,
        options.npSymbFact,
        options.verbosity,
        *muPEXSI,
        *numElectronPEXSI,
        *muMinInertia,
        *muMaxInertia,
        *numTotalInertiaIter,
        *numTotalPEXSIIter );
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}
  return;
}   // -----  end of function PPEXSIDFTDriver  ----- 



extern "C"
void PPEXSIRetrieveRealSymmetricDFTMatrix(
    PPEXSIPlan        plan,
		double*      DMnzvalLocal,
		double*     EDMnzvalLocal,
		double*     FDMnzvalLocal,
    double*     totalEnergyH,
    double*     totalEnergyS,
    double*     totalFreeEnergy,
    int*              info ){
  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();
  PPEXSIData* ptrData = reinterpret_cast<PPEXSIData*>(plan);
  
  try{
    Int nnzLocal = ptrData->RhoRealMat().nnzLocal;

    blas::Copy( nnzLocal, ptrData->RhoRealMat().nzvalLocal.Data(), 1,
        DMnzvalLocal, 1 );

    blas::Copy( nnzLocal, ptrData->EnergyDensityRealMat().nzvalLocal.Data(), 1,
        EDMnzvalLocal, 1 );

    blas::Copy( nnzLocal, ptrData->FreeEnergyDensityRealMat().nzvalLocal.Data(), 1,
        FDMnzvalLocal, 1 );

    *totalEnergyH = ptrData->TotalEnergyH();

    *totalEnergyS = ptrData->TotalEnergyS();

    *totalFreeEnergy = ptrData->TotalFreeEnergy();
  }
	catch( std::exception& e ) {
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}
  return;
}   // -----  end of function PPEXSIRetrieveRealSymmetricDFTMatrix  ----- 



extern "C"
void PPEXSIPlanFinalize( 
    PPEXSIPlan    plan,
    int*          info ){

  *info = 0;
  const GridType* gridPole = 
    reinterpret_cast<PPEXSIData*>(plan)->GridPole();
  
  try{
    delete reinterpret_cast<PPEXSIData*>(plan);
  }
	catch( std::exception& e )
	{
		statusOFS << std::endl << "ERROR!!! Proc " << gridPole->mpirank 
      << " caught exception with message: "
			<< std::endl << e.what() << std::endl;
		*info = 1;
#ifndef _RELEASE_
		DumpCallStack();
#endif
	}
  return;
}   // -----  end of function PPEXSIPlanFinalize  ----- 

// ********************************************************************* 
// FORTRAN interface
// 
// NOTE: 
// 
// 1. Most of the interface routines do not explicitly depend on the MPI
// communicators, and therefore can be called directly through the
// ISO_C_BINDING feature as in f_interface.f90.
//
// 2. For routines use MPI communicators, the communicators from FORTRAN
// must be transferred to C communicators using f2c_comm, and the
// FORTRAN version is given below.
//
// 3. In ISO_C_BINDING, passing by value is allowed and it is not
// required that all arguments by given in the form of pointers.
//
// 4. The following routines need not be defined in the C header file.
// *********************************************************************

/// @brief Internal subroutine to convert FORTRAN communicator to C
extern "C" 
MPI_Comm f2c_comm(MPI_Fint* Fcomm)
{
	return MPI_Comm_f2c((*Fcomm));
}  // -----  end of function f2c_comm ----- 


/// @brief FORTRAN interface for @ref ReadDistSparseMatrixFormattedHeadInterface.
extern "C"
void f_read_distsparsematrix_formatted_head (
		char*    filename,
		int*     size,
		int*     nnz,
		int*     nnzLocal,
		int*     numColLocal,
		MPI_Fint* Fcomm )
{
  ReadDistSparseMatrixFormattedHeadInterface(
			filename,
			size,
			nnz,
			nnzLocal,
			numColLocal,
			f2c_comm( Fcomm ) );

	return;
}  // -----  end of function f_read_distsparsematrix_formatted_head  


/// @brief FORTRAN interface for @ref ReadDistSparseMatrixFormattedInterface.
extern "C"
void f_read_distsparsematrix_formatted (
		char*    filename,
		int      size,
		int      nnz,
		int      nnzLocal,
		int      numColLocal,
		int*     colptrLocal,
		int*     rowindLocal,
		double*  nzvalLocal,
    MPI_Fint* Fcomm )
{
	ReadDistSparseMatrixFormattedInterface(
			filename,
			size,
			nnz,
			nnzLocal,
			numColLocal,
			colptrLocal,
			rowindLocal,
			nzvalLocal,
			f2c_comm( Fcomm ) );
	return;
} // -----  end of function f_read_distsparsematrix_formatted  ----- 

/// @brief FORTRAN interface for @ref ReadDistSparseMatrixHeadInterface.
extern "C"
void f_read_distsparsematrix_head (
		char*    filename,
		int*     size,
		int*     nnz,
		int*     nnzLocal,
		int*     numColLocal,
		MPI_Fint* Fcomm )
{
  ReadDistSparseMatrixHeadInterface(
			filename,
			size,
			nnz,
			nnzLocal,
			numColLocal,
			f2c_comm( Fcomm ) );

	return;
}  // -----  end of function f_read_distsparsematrix_head  


/// @brief FORTRAN interface for @ref ParaReadDistSparseMatrixInterface.
extern "C"
void f_para_read_distsparsematrix (
		char*    filename,
		int      size,
		int      nnz,
		int      nnzLocal,
		int      numColLocal,
		int*     colptrLocal,
		int*     rowindLocal,
		double*  nzvalLocal,
		MPI_Fint* Fcomm )
{
	ParaReadDistSparseMatrixInterface(
			filename,
			size,
			nnz,
			nnzLocal,
			numColLocal,
			colptrLocal,
			rowindLocal,
			nzvalLocal,
			f2c_comm( Fcomm ) );
	return;
} // -----  end of function f_para_read_distsparsematrix  ----- 


/// @brief FORTRAN interface for @ref PPEXSIPlanInitialize
extern "C"
PPEXSIPlan f_ppexsi_plan_initialize (
    MPI_Fint*     Fcomm,
    int           numProcRow,
    int           numProcCol,
    int           outputFileIndex, 
    int*          info ){
  return PPEXSIPlanInitialize(
      f2c_comm( Fcomm ),
      numProcRow,
      numProcCol,
      outputFileIndex,
      info );
}		// -----  end of function f_ppexsi_plan_initialize  ----- 


