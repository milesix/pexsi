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
/// @file new_ppexsi.cpp
/// @brief New implementation of the parallel %PEXSI.
/// @date 2014-03-09  Revise for the new interface.

// FIXME
#define _DEBUGlevel_ 0

#include "new_ppexsi.hpp"
#include "utility.hpp"

namespace PEXSI{

PPEXSINewData::PPEXSINewData	(
    MPI_Comm   comm,
    Int        numProcRow, 
    Int        numProcCol, 
    Int        outputFileIndex ){
#ifndef _RELEASE_
	PushCallStack("PPEXSINewData::PPEXSINewData");
#endif

  Int mpirank, mpisize;
  MPI_Comm_rank( comm, &mpirank );
  MPI_Comm_size( comm, &mpisize );

  Int npPerPole = numProcRow * numProcCol;
  if( mpisize % npPerPole != 0 ){
    std::ostringstream msg;
    msg 
      << "mpisize    = " << mpisize << std::endl
      << "npPerPole = " << npPerPole << std::endl
      << "mpisize is not divisible by npPerPole!" << std::endl;
    throw std::runtime_error( msg.str().c_str() );
  }

  gridPole_     = new GridType( comm, mpisize / npPerPole, npPerPole );
	gridSuperLU_  = new SuperLUGrid( gridPole_->rowComm, 
      numProcRow, numProcCol );
	gridSelInv_   = new GridType( gridPole_->rowComm, 
      numProcRow, numProcCol );

  // Start the log file. Append to previous log files
  std::stringstream ss;
	ss << "logPEXSI" << outputFileIndex;
	statusOFS.open( ss.str().c_str(), std::ios_base::app );

  // Initialize the saved variables
  muPEXSISave_ = 0.0;
  
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method PPEXSINewData::PPEXSINewData  ----- 


PPEXSINewData::~PPEXSINewData	(  )
{
#ifndef _RELEASE_
	PushCallStack("PPEXSINewData::~PPEXSINewData");
#endif
  if( gridPole_    != NULL ){
    delete gridPole_;
  }

	if( gridSuperLU_ != NULL ){
		delete gridSuperLU_;
	}
	
	if( gridSelInv_ != NULL ){
		delete gridSelInv_;
	}

  // Close the log file
  statusOFS.close();

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method PPEXSINewData::~PPEXSINewData  ----- 


void
PPEXSINewData::LoadRealSymmetricMatrix	(
    Int           nrows,                        
    Int           nnz,                          
    Int           nnzLocal,                     
    Int           numColLocal,                  
    Int*          colptrLocal,                  
    Int*          rowindLocal,                  
    Real*         HnzvalLocal,                  
    Int           isSIdentity,                  
    Real*         SnzvalLocal )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::LoadRealSymmetricMatrix");
#endif
  std::vector<char> sstr;
  Int sizeStm;
  if( MYROW( gridPole_ ) == 0 ){
    std::stringstream sstm;

    HRealMat_.size        = nrows;
    HRealMat_.nnz         = nnz;
    HRealMat_.nnzLocal    = nnzLocal;
    // The first row processor does not need extra copies of the index /
    // value of the matrix. 
    HRealMat_.colptrLocal = IntNumVec( numColLocal+1, false, colptrLocal );
    HRealMat_.rowindLocal = IntNumVec( nnzLocal,      false, rowindLocal );
    // H value
    HRealMat_.nzvalLocal  = DblNumVec( nnzLocal,      false, HnzvalLocal );
    HRealMat_.comm        = gridPole_->rowComm;

    // Serialization will copy the values regardless of the ownership
    serialize( HRealMat_, sstm, NO_MASK );

    // S value
    if( isSIdentity ){
      SRealMat_.size = 0;
      SRealMat_.nnz  = 0;
      SRealMat_.nnzLocal = 0;
      SRealMat_.comm = HRealMat_.comm; 
    }
    else{
      CopyPattern( HRealMat_, SRealMat_ );
      SRealMat_.comm = HRealMat_.comm; 
      SRealMat_.nzvalLocal  = DblNumVec( nnzLocal,      false, SnzvalLocal );
      serialize( SRealMat_.nzvalLocal, sstm, NO_MASK );
    }

    sstr.resize( Size( sstm ) );
    sstm.read( &sstr[0], sstr.size() ); 	
    sizeStm = sstr.size();
  }

  MPI_Bcast( &sizeStm, 1, MPI_INT, 0, gridPole_->colComm );

#if ( _DEBUGlevel_ >= 0 )
  statusOFS << "sizeStm = " << sizeStm << std::endl;
#endif

  if( MYROW( gridPole_ ) != 0 ) sstr.resize( sizeStm );

  MPI_Bcast( (void*)&sstr[0], sizeStm, MPI_BYTE, 0, gridPole_->colComm );

  if( MYROW( gridPole_ ) != 0 ){
    std::stringstream sstm;
    sstm.write( &sstr[0], sizeStm );
    deserialize( HRealMat_, sstm, NO_MASK );
    // Communicator
    HRealMat_.comm = gridPole_->rowComm;
    if( isSIdentity ){
      SRealMat_.size = 0;    // Means S is an identity matrix
      SRealMat_.nnz  = 0;
      SRealMat_.nnzLocal = 0;
      SRealMat_.comm = HRealMat_.comm;
    }
    else{
      CopyPattern( HRealMat_, SRealMat_ );
      SRealMat_.comm = HRealMat_.comm;
      deserialize( SRealMat_.nzvalLocal, sstm, NO_MASK );
    }
  }
  sstr.clear();


#if ( _DEBUGlevel_ >= 0 )
  statusOFS << "H.size     = " << HRealMat_.size     << std::endl;
  statusOFS << "H.nnzLocal = " << HRealMat_.nnzLocal << std::endl;
  statusOFS << "S.size     = " << SRealMat_.size     << std::endl;
  statusOFS << "S.nnzLocal = " << SRealMat_.nnzLocal << std::endl;
#endif


  // Record the index for the diagonal elements to handle the case if S
  // is identity.
	{
		Int numColLocal      = HRealMat_.colptrLocal.m() - 1;
		Int numColLocalFirst = HRealMat_.size / gridSelInv_->mpisize;
		Int firstCol         = gridSelInv_->mpirank * numColLocalFirst;
		
		diagIdxLocal_.clear();

		for( Int j = 0; j < numColLocal; j++ ){
			Int jcol = firstCol + j + 1;
			for( Int i = HRealMat_.colptrLocal(j)-1; 
				 	 i < HRealMat_.colptrLocal(j+1)-1; i++ ){
				Int irow = HRealMat_.rowindLocal(i);
				if( irow == jcol ){
					diagIdxLocal_.push_back( i );
				}
			}
		} // for (j)
	}


#ifndef _RELEASE_
  PopCallStack();
#endif

  return ;
}    	// -----  end of method PPEXSINewData::LoadRealSymmetricMatrix  ----- 



// Main subroutine for the electronic structure calculation
void PPEXSINewData::CalculateFermiOperatorReal(
    Int   numPole, 
    Real  temperature,
    Real  gap,
    Real  deltaE,
    Real  mu,
    Real  numElectronExact,
    Real  numElectronTolerance,
    const DistSparseMatrix<Real>&  HMat,
    const DistSparseMatrix<Real>&  SMat,
    std::string         ColPerm,
    Int                 numProcSymbFact,
    Real& numElectron,
    Real& numElectronDrvMu ){

#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateFermiOperatorReal");
#endif

  // *********************************************************************
  // Check the input parameters
  // *********************************************************************
  if( numPole % 2 != 0 ){
    throw std::logic_error( "Must be even number of poles!" );
  }

  // TODO Check H and S have the same pattern

  // TODO Check H and S agree with gridPole_

  // *********************************************************************
  // Initialize
  // *********************************************************************
  // rename for convenience
  DistSparseMatrix<Real>& rhoMat       = rhoRealMat_;     
  DistSparseMatrix<Real>& rhoDrvMuMat  = rhoDrvMuRealMat_;
  DistSparseMatrix<Real>& rhoDrvTMat   = rhoDrvTRealMat_;
  DistSparseMatrix<Real>& hmzMat       = freeEnergyDensityRealMat_;
  DistSparseMatrix<Real>& frcMat       = energyDensityRealMat_;

  DistSparseMatrix<Complex>& AMat      = shiftComplexMat_;
  DistSparseMatrix<Complex>& AinvMat   = shiftInvComplexMat_;

  // 
  bool isFreeEnergyDensityMatrix = true;
  bool isEnergyDensityMatrix     = true;
  bool isDerivativeTMatrix       = false;

  // Copy the pattern
  CopyPattern( HMat, AMat );
  CopyPattern( HMat, rhoMat );
  CopyPattern( HMat, rhoDrvMuMat );
  if( isFreeEnergyDensityMatrix )
    CopyPattern( HMat, hmzMat );
  if( isEnergyDensityMatrix )
    CopyPattern( HMat, frcMat );
  if( isDerivativeTMatrix )
    CopyPattern( HMat, rhoDrvTMat );

  SetValue( AMat.nzvalLocal, Z_ZERO );          
  // Symbolic factorization does not need value

#if ( _DEBUGlevel_ >= 0 )
  statusOFS << "AMat.nnzLocal = " << AMat.nnzLocal << std::endl;
  statusOFS << "AMat.nnz      = " << AMat.Nnz()    << std::endl;
#endif

  SuperLUOptions   luOpt;

  luOpt.ColPerm = ColPerm;
  luOpt.numProcSymbFact = numProcSymbFact;
  // TODO Introduce maxPipelineDepth as an adjustable parameter when needed.
  luOpt.maxPipelineDepth = -1;

  // FIXME Update to new superlumatrix formulation using the template format. 
  // FIXME Symbolic factorization to be put outside the routine
  SuperLUMatrix    luMat( *gridSuperLU_, luOpt );  // SuperLU matrix.

  // *********************************************************************
  // Symbolic factorization.  
  // Each numPoleGroup perform independently
  // *********************************************************************
  Real timeSta, timeEnd;
  GetTime( timeSta );
  luMat.DistSparseMatrixToSuperMatrixNRloc( AMat );
  GetTime( timeEnd );
#if ( _DEBUGlevel_ >= 0 )
  statusOFS << "AMat is converted to SuperMatrix." << std::endl;
  statusOFS << "Time for SuperMatrix conversion is " <<
    timeEnd - timeSta << " [s]" << std::endl << std::endl;
#endif
  GetTime( timeSta );
  luMat.SymbolicFactorize();
  GetTime( timeEnd );
#if ( _DEBUGlevel_ >= 0 )
  statusOFS << "Symbolic factorization is finished." << std::endl;
  statusOFS << "Time for symbolic factorization is " <<
    timeEnd - timeSta << " [s]" << std::endl << std::endl;
#endif
  luMat.SymbolicToSuperNode( super_ );
  luMat.DestroyAOnly();


  // Compute the number of nonzeros from PMatrix
  {
    PMatrix PMloc( gridSelInv_, &super_ , &luOpt); // A^{-1} in PMatrix format
    luMat.LUstructToPMatrix( PMloc );
#if ( _DEBUGlevel_ >= 0 )
    Int nnzLocal = PMloc.NnzLocal();
    statusOFS << "Number of local nonzeros (L+U) = " << nnzLocal << std::endl;
    LongInt nnz  = PMloc.Nnz();
    statusOFS << "Number of nonzeros (L+U)       = " << nnz << std::endl;
#endif
  }

#if ( _DEBUGlevel_ >= 1 )
  statusOFS << "perm: "    << std::endl << super_.perm     << std::endl;
  statusOFS << "permInv: " << std::endl << super_.permInv  << std::endl;
  statusOFS << "superIdx:" << std::endl << super_.superIdx << std::endl;
  statusOFS << "superPtr:" << std::endl << super_.superPtr << std::endl; 
#endif


  // Reinitialize the variables
  SetValue( rhoMat.nzvalLocal, 0.0 );
  SetValue( rhoDrvMuMat.nzvalLocal, 0.0 );
  if( isFreeEnergyDensityMatrix )
    SetValue( hmzMat.nzvalLocal, 0.0 );
  if( isEnergyDensityMatrix )
    SetValue( frcMat.nzvalLocal, 0.0 );
  if( isDerivativeTMatrix )
    SetValue( rhoDrvTMat.nzvalLocal, 0.0 );

  // Refine the pole expansion  
  // numPoleInput is the number of poles to be given to other parts of
  // the pole expansion, which is larger than or equal to numPole.
  Int numPoleInput;
  // poleIdx is a vector of size numPole.  Only poles with index in
  // poleIdx are used for actual computation. The rest of the poles are
  // discarded according to tolerance criterion
  //
  //   numElectronTolerance / numElectronExact / numPole
  //
  // FIXME The heuristics should be refined to give error estimate to
  // other quantities such as the energy.
  // FIXME The heuristics part should also be given in a separate
  // routine, and the input of this file does not need mu, gap etc.
  std::vector<Int>  poleIdx(numPole);
  {
    // Setup a grid from (mu - deltaE, mu + deltaE), and measure
    // the error bound in the L^infty sense on this grid.
    //
    // fdGrid:      Exact Fermi-Dirac function evaluated on xGrid
    // fdPoleGrid:  Fermi-Dirac function using pole expansion
    // evaluated on the grid.
    Int numX = 10000;
    std::vector<Real>    xGrid( numX );
    std::vector<Real>    fdGrid( numX );

    Real x0 = mu - deltaE;
    Real x1 = mu + deltaE;
    Real h  = (x1 - x0) / (numX - 1);
    Real ez;
    for( Int i = 0; i < numX; i++ ){
      xGrid[i]  = x0 + i * h;
      if( xGrid[i] - mu >= 0 ){
        ez = std::exp(- (xGrid[i] - mu) / temperature );
        fdGrid[i] = 2.0 * ez / (1.0 + ez);
      }
      else{
        ez = std::exp((xGrid[i] - mu) / temperature );
        fdGrid[i] = 2.0 / (1.0 + ez);
      }
    }


    numPoleInput = numPole;
    Real tol;
    Int  numPoleSignificant;

    Int poleIter = 0;
    do{
      // If the number of significant poles is less than numPole,
      // increase numPoleInput by 2 at a time and redo the
      // computation.
      if( poleIter > 0 )
        numPoleInput += 2;

      zshift_.resize( numPoleInput );
      zweightRho_.resize( numPoleInput );
      GetPoleDensity( &zshift_[0], &zweightRho_[0],
          numPoleInput, temperature, gap, deltaE, mu ); 

      std::vector<Real>  maxMagPole(numPoleInput);
      for( Int l = 0; l < numPoleInput; l++ ){
        maxMagPole[l] = 0.0;
      }

      // Compute the approximation due to pole expansion, as well as
      // the maximum magnitude of each pole
      Complex cpxmag;
      Real    mag;
      numPoleSignificant = 0;
      tol = numElectronTolerance / numElectronExact / numPoleInput;
      for( Int l = 0; l < numPoleInput; l++ ){
        for( Int i = 0; i < numX; i++ ){
          cpxmag = zweightRho_[l] / ( xGrid[i] - zshift_[l] );
          mag    = cpxmag.imag();
          maxMagPole[l] = ( maxMagPole[l] >= mag ) ?  maxMagPole[l] : mag;
        }
        if( maxMagPole[l] > tol ){
          numPoleSignificant++;
        }	
      } // for (l)

      // Pick the most significant numPole poles and update poleIdx
      // Sort in DESCENDING order
      std::vector<Int>  sortIdx( numPoleInput );
      for( Int i = 0; i < sortIdx.size(); i++ ){
        sortIdx[i]      = i;
      }
      std::sort( sortIdx.begin(), sortIdx.end(), 
          IndexComp<std::vector<Real>& >( maxMagPole ) ) ;
      std::reverse( sortIdx.begin(), sortIdx.end() );

      for( Int l = 0; l < numPole; l++ ){
        poleIdx[l]      = sortIdx[l];
      }


      // Update poleIter
      poleIter++; 
    } while( numPoleSignificant < numPole );


    // Compute the relative error 
    std::vector<Real>  fdPoleGrid( numX );
    Real errRel, errorRelMax;
    Real errAbs, errorAbsMax;
    Real errorTotal;
    Real errEPS = 1e-1;

    errorRelMax = 0.0;
    errorAbsMax = 0.0; 
    for( Int i = 0; i < numX; i++ ){
      fdPoleGrid[i] = 0.0;
      for( Int lidx = 0; lidx < numPoleInput; lidx++ ){
        Int l = lidx;
        Complex cpxmag = zweightRho_[l] / ( xGrid[i] - zshift_[l] );
        fdPoleGrid[i] += cpxmag.imag();
      }
      errAbs = std::abs( fdPoleGrid[i] - fdGrid[i] );
      errorAbsMax = ( errorAbsMax >= errAbs ) ? errorAbsMax : errAbs;

      if( std::abs(fdGrid[i]) > errEPS ){
        errRel = std::abs( fdPoleGrid[i] - fdGrid[i] ) / ( std::abs( fdGrid[i] ) );
        errorRelMax = ( errorRelMax >= errRel ) ? errorRelMax : errRel;
      }
    }

    errorTotal = errorRelMax * numElectronExact + errorAbsMax * HMat.size;

#if ( _DEBUGlevel_ >= 0 )
    statusOFS << "Pole expansion indicates that the error "
      << "of numElectron is bounded by "
      << errorTotal << std::endl;
    statusOFS << "Required accuracy: numElectronTolerance is "
      << numElectronTolerance << std::endl << std::endl;

    if( errorTotal > numElectronTolerance ){
      statusOFS << "WARNING!!! " 
        << "Pole expansion may not be accurate enough to reach numElectronTolerance. " << std::endl
        << "Try to increase numPole or increase numElectronTolerance." << std::endl << std::endl;

    }
    statusOFS << "numPoleInput =" << numPoleInput << std::endl;
    statusOFS << "numPoleSignificant = " << numPoleSignificant << std::endl;
#endif
  }

  // Initialize the number of electrons
  numElectron  = 0.0;

  //Initialize the pole expansion
  zweightRhoDrvMu_.resize( numPoleInput );

  GetPoleDensityDrvMu( &zshift_[0], &zweightRhoDrvMu_[0],
      numPoleInput, temperature, gap, deltaE, mu ); 

  if( isFreeEnergyDensityMatrix ){
    std::vector<Complex>  zshiftTmp( numPoleInput );
    zweightHelmholtz_.resize( numPoleInput );
    GetPoleHelmholtz( &zshiftTmp[0], &zweightHelmholtz_[0], 
        numPoleInput, temperature, gap, deltaE, mu ); 
  }

  if( isEnergyDensityMatrix ){
    std::vector<Complex>  zshiftTmp( numPoleInput );
    zweightForce_.resize( numPoleInput );
    GetPoleForce( &zshiftTmp[0], &zweightForce_[0],
        numPoleInput, temperature, gap, deltaE, mu ); 
  }

  if( isDerivativeTMatrix ){
    std::vector<Complex>  zshiftTmp( numPoleInput );
    zweightRhoDrvT_.resize( numPoleInput );
    GetPoleDensityDrvT( &zshiftTmp[0], &zweightRhoDrvT_[0],
        numPoleInput, temperature, gap, deltaE, mu ); 
  }

#if ( _DEBUGlevel_ >= 1 )
  statusOFS << "zshift" << std::endl << zshift_ << std::endl;
  statusOFS << "zweightRho" << std::endl << zweightRho_ << std::endl;
#endif

  // for each pole, perform LDLT factoriation and selected inversion
  Real timePoleSta, timePoleEnd;

  Int numPoleComputed = 0;
  for(Int lidx = 0; lidx < numPole; lidx++){
    if( MYROW( gridPole_ ) == PROW( lidx, gridPole_ ) ){

      Int l = poleIdx[lidx];

      GetTime( timePoleSta );
#if ( _DEBUGlevel_ >= 0 )
      statusOFS << "Pole " << lidx << " processing..." << std::endl;
      statusOFS << "zshift           = " << zshift_[l] << std::endl;
      statusOFS	<< "zweightRho       = " << zweightRho_[l] << std::endl;
      statusOFS	<< "zweightRhoDrvMu  = " << zweightRhoDrvMu_[l] << std::endl;
      if( isFreeEnergyDensityMatrix )
        statusOFS << "zweightHelmholtz = " << zweightHelmholtz_[l] << std::endl;
      if( isEnergyDensityMatrix )
        statusOFS << "zweightForce     = " << zweightForce_[l] << std::endl;
      if( isDerivativeTMatrix )
        statusOFS << "zweightRhoDrvT   = " << zweightRhoDrvT_[l] << std::endl;
#endif
      {
        numPoleComputed++;

        if( SMat.size != 0 ){
          // S is not an identity matrix
          for( Int i = 0; i < HMat.nnzLocal; i++ ){
            AMat.nzvalLocal(i) = HMat.nzvalLocal(i) - zshift_[l] * SMat.nzvalLocal(i);
          }
        }
        else{
          // S is an identity matrix
          for( Int i = 0; i < HMat.nnzLocal; i++ ){
            AMat.nzvalLocal(i) = HMat.nzvalLocal(i);
          }

          for( Int i = 0; i < diagIdxLocal_.size(); i++ ){
            AMat.nzvalLocal( diagIdxLocal_[i] ) -= zshift_[l];
          }
        } // if (SMat.size != 0 )


        // *********************************************************************
        // Factorization
        // *********************************************************************
        // Important: the distribution in pzsymbfact is going to mess up the
        // A matrix.  Recompute the matrix A here.
#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "Before DistSparseMatrixToSuperMatrixNRloc." << std::endl;
#endif
        luMat.DistSparseMatrixToSuperMatrixNRloc( AMat );
#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "After DistSparseMatrixToSuperMatrixNRloc." << std::endl;
#endif

        Real timeTotalFactorizationSta, timeTotalFactorizationEnd;

        GetTime( timeTotalFactorizationSta );

        // Data redistribution
#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "Before Distribute." << std::endl;
#endif
        luMat.Distribute();
#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "After Distribute." << std::endl;
#endif

        // Numerical factorization
#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "Before NumericalFactorize." << std::endl;
#endif
        luMat.NumericalFactorize();
#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "After NumericalFactorize." << std::endl;
#endif
        luMat.DestroyAOnly();

        GetTime( timeTotalFactorizationEnd );

#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "Time for total factorization is " << timeTotalFactorizationEnd - timeTotalFactorizationSta<< " [s]" << std::endl; 
#endif

        // *********************************************************************
        // Selected inversion
        // *********************************************************************
        Real timeTotalSelInvSta, timeTotalSelInvEnd;
        GetTime( timeTotalSelInvSta );

        PMatrix PMloc( gridSelInv_, &super_, &luOpt ); // A^{-1} in PMatrix format

        luMat.LUstructToPMatrix( PMloc );

        // P2p communication version
        PMloc.ConstructCommunicationPattern();

        // Collective communication version
        //          PMloc.ConstructCommunicationPattern_Collectives();

        PMloc.PreSelInv();

        // Main subroutine for selected inversion
        //
        // P2p communication version
        PMloc.SelInv();

        // Collective communication version
        //          PMloc.SelInv_Collectives();

        GetTime( timeTotalSelInvEnd );

#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "Time for total selected inversion is " <<
          timeTotalSelInvEnd  - timeTotalSelInvSta << " [s]" << std::endl;
#endif

        // *********************************************************************
        // Postprocessing
        // *********************************************************************

        Real timePostProcessingSta, timePostProcessingEnd;

        GetTime( timePostProcessingSta );

        PMloc.PMatrixToDistSparseMatrix( AMat, AinvMat );

#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "rhoMat.nnzLocal = " << rhoMat.nnzLocal << std::endl;
        statusOFS << "AinvMat.nnzLocal = " << AinvMat.nnzLocal << std::endl;
#endif


        // Update the density matrix. The following lines are equivalent to
        //
        //				for( Int i = 0; i < rhoMat.nnzLocal; i++ ){
        //					rhoMat.nzvalLocal(i) += 
        //						zweightRho_[l].real() * AinvMat.nzvalLocal(i).imag() + 
        //						zweightRho_[l].imag() * AinvMat.nzvalLocal(i).real();
        //				}
        // 
        // But done more cache-efficiently with blas.
        Real* AinvMatRealPtr = (Real*)AinvMat.nzvalLocal.Data();
        Real* AinvMatImagPtr = AinvMatRealPtr + 1;
        blas::Axpy( rhoMat.nnzLocal, zweightRho_[l].real(), AinvMatImagPtr, 2, 
            rhoMat.nzvalLocal.Data(), 1 );
        blas::Axpy( rhoMat.nnzLocal, zweightRho_[l].imag(), AinvMatRealPtr, 2,
            rhoMat.nzvalLocal.Data(), 1 );

        // Derivative of the Fermi-Dirac with respect to mu
        blas::Axpy( rhoDrvMuMat.nnzLocal, zweightRhoDrvMu_[l].real(), AinvMatImagPtr, 2, 
            rhoDrvMuMat.nzvalLocal.Data(), 1 );
        blas::Axpy( rhoDrvMuMat.nnzLocal, zweightRhoDrvMu_[l].imag(), AinvMatRealPtr, 2,
            rhoDrvMuMat.nzvalLocal.Data(), 1 );

        // Free energy density matrix
        if( isFreeEnergyDensityMatrix ){
          blas::Axpy( hmzMat.nnzLocal, zweightHelmholtz_[l].real(), AinvMatImagPtr, 2,
              hmzMat.nzvalLocal.Data(), 1 );
          blas::Axpy( hmzMat.nnzLocal, zweightHelmholtz_[l].imag(), AinvMatRealPtr, 2,
              hmzMat.nzvalLocal.Data(), 1 );
        }

        // Energy density matrix
        if( isEnergyDensityMatrix ){
          blas::Axpy( frcMat.nnzLocal, zweightForce_[l].real(), AinvMatImagPtr, 2,
              frcMat.nzvalLocal.Data(), 1 );
          blas::Axpy( frcMat.nnzLocal, zweightForce_[l].imag(), AinvMatRealPtr, 2, 
              frcMat.nzvalLocal.Data(), 1 );
        }

        // Derivative of the Fermi-Dirac with respect to T
        if( isDerivativeTMatrix ){
          blas::Axpy( rhoDrvTMat.nnzLocal, zweightRhoDrvT_[l].real(), AinvMatImagPtr, 2, 
              rhoDrvTMat.nzvalLocal.Data(), 1 );
          blas::Axpy( rhoDrvTMat.nnzLocal, zweightRhoDrvT_[l].imag(), AinvMatRealPtr, 2,
              rhoDrvTMat.nzvalLocal.Data(), 1 );
        }

        // Update the free energy density matrix and energy density matrix similarly
        GetTime( timePostProcessingEnd );

#if ( _DEBUGlevel_ >= 0 )
        statusOFS << "Time for postprocessing is " <<
          timePostProcessingEnd - timePostProcessingSta << " [s]" << std::endl;
#endif

      }
      GetTime( timePoleEnd );

#if ( _DEBUGlevel_ >= 0 )
      statusOFS << "Time for pole " << lidx << " is " <<
        timePoleEnd - timePoleSta << " [s]" << std::endl << std::endl;
#endif

    } // if I am in charge of this pole
#if ( _DEBUGlevel_ >= 1 )
    // Output the number of electrons at each step for debugging,
    // if there is no parallelization among poles.
    // This debug mode is currently only available if SMat is not
    // implicitly given by  an identity matrix 
    if( gridPole_->numProcRow == 1 && SMat.size != 0 ){
      Real numElecLocal = blas::Dot( SMat.nnzLocal, SMat.nzvalLocal.Data(),
          1, rhoMat.nzvalLocal.Data(), 1 );

      Real numElec;
      mpi::Allreduce( &numElecLocal, &numElec, 1, MPI_SUM, gridPole_->comm ); 

      statusOFS << std::endl << "No parallelization of poles, output the number of electrons up to this pole." << std::endl;
      statusOFS << "numElecLocal = " << numElecLocal << std::endl;
      statusOFS << "numElecTotal = " << numElec << std::endl << std::endl;
    }
#endif
  } // for(lidx)

  // Reduce the density matrix across the processor rows in gridPole_
  {
    DblNumVec nzvalRhoMatLocal = rhoMat.nzvalLocal;
    SetValue( rhoMat.nzvalLocal, 0.0 );

    mpi::Allreduce( nzvalRhoMatLocal.Data(), rhoMat.nzvalLocal.Data(),
        rhoMat.nnzLocal, MPI_SUM, gridPole_->colComm );
  }

  // Reduce the derivative of density matrix with respect to mu across
  // the processor rows in gridPole_ 
  {
    DblNumVec nzvalRhoDrvMuMatLocal = rhoDrvMuMat.nzvalLocal;
    SetValue( rhoDrvMuMat.nzvalLocal, 0.0 );

    mpi::Allreduce( nzvalRhoDrvMuMatLocal.Data(), rhoDrvMuMat.nzvalLocal.Data(),
        rhoDrvMuMat.nnzLocal, MPI_SUM, gridPole_->colComm );
  }

  // Reduce the free energy density matrix across the processor rows in gridPole_ 
  if( isFreeEnergyDensityMatrix ){
    DblNumVec nzvalHmzMatLocal = hmzMat.nzvalLocal;
    SetValue( hmzMat.nzvalLocal, 0.0 );

    mpi::Allreduce( nzvalHmzMatLocal.Data(), hmzMat.nzvalLocal.Data(),
        hmzMat.nnzLocal, MPI_SUM, gridPole_->colComm );
  }

  // Reduce the energy density matrix across the processor rows in gridPole_ 
  if( isEnergyDensityMatrix ){
    DblNumVec nzvalFrcMatLocal = frcMat.nzvalLocal;
    SetValue( frcMat.nzvalLocal, 0.0 );

    mpi::Allreduce( nzvalFrcMatLocal.Data(), frcMat.nzvalLocal.Data(),
        frcMat.nnzLocal, MPI_SUM, gridPole_->colComm );
  }

  // Reduce the derivative of density matrix with respect to T across
  // the processor rows in gridPole_ 
  if( isDerivativeTMatrix ){
    DblNumVec nzvalRhoDrvTMatLocal = rhoDrvTMat.nzvalLocal;
    SetValue( rhoDrvTMat.nzvalLocal, 0.0 );

    mpi::Allreduce( nzvalRhoDrvTMatLocal.Data(), rhoDrvTMat.nzvalLocal.Data(),
        rhoDrvTMat.nnzLocal, MPI_SUM, gridPole_->colComm );
  }

  // All processors groups compute the number of electrons

  numElectron      = CalculateNumElectron( SMat );
  numElectronDrvMu = CalculateNumElectronDrvMu( SMat );

#ifndef _RELEASE_
  PopCallStack();
#endif

  return ;
}    // -----  end of method PPEXSINewData::CalculateFermiOperatorReal  ----- 



Real
PPEXSINewData::CalculateNumElectron	( const DistSparseMatrix<Real>& SMat )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateNumElectron");
#endif
  Real numElecLocal = 0.0, numElec = 0.0;
  DistSparseMatrix<Real>& rhoMat       = rhoRealMat_;     


  // TODO Check SMat and rhoMat has the same sparsity if SMat is not
  // implicitly given by an identity matrix.

  if( SMat.size != 0 ){
    // S is not an identity matrix
    numElecLocal = blas::Dot( SMat.nnzLocal, SMat.nzvalLocal.Data(),
        1, rhoMat.nzvalLocal.Data(), 1 );
  }
  else{
    // S is an identity matrix
    DblNumVec& nzval = rhoMat.nzvalLocal;
    for( Int i = 0; i < diagIdxLocal_.size(); i++ ){
      numElecLocal += nzval(diagIdxLocal_[i]);
    }

  } // if ( SMat.size != 0 )
#if ( _DEBUGlevel_ >= 0 )
  statusOFS << std::endl << "numElecLocal = " << numElecLocal << std::endl;
#endif

  mpi::Allreduce( &numElecLocal, &numElec, 1, MPI_SUM, rhoMat.comm ); 

#ifndef _RELEASE_
  PopCallStack();
#endif

  return numElec;
} 		// -----  end of method PPEXSINewData::CalculateNumElectron  ----- 

Real
PPEXSINewData::CalculateNumElectronDrvMu	( const DistSparseMatrix<Real>& SMat )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateNumElectronDrvMu");
#endif
  Real numElecDrvLocal = 0.0, numElecDrv = 0.0;
  DistSparseMatrix<Real>& rhoMat       = rhoRealMat_;     
  DistSparseMatrix<Real>& rhoDrvMuMat  = rhoDrvMuRealMat_;     

  // TODO Check SMat and rhoDrvMuMat has the same sparsity


  if( SMat.size != 0 ){
    // S is not an identity matrix
    numElecDrvLocal = blas::Dot( SMat.nnzLocal, SMat.nzvalLocal.Data(),
        1, rhoDrvMuMat.nzvalLocal.Data(), 1 );
  }
  else{
    // S is an identity matrix
    DblNumVec& nzval = rhoDrvMuMat.nzvalLocal;
    for( Int i = 0; i < diagIdxLocal_.size(); i++ ){
      numElecDrvLocal += nzval(diagIdxLocal_[i]);
    }
  }

#if ( _DEBUGlevel_ >= 0 )
  statusOFS << std::endl << "numElecDrvLocal = " << numElecDrvLocal << std::endl;
#endif

  mpi::Allreduce( &numElecDrvLocal, &numElecDrv, 1, MPI_SUM, rhoDrvMuMat.comm ); 

#ifndef _RELEASE_
  PopCallStack();
#endif

  return numElecDrv;
} 		// -----  end of method PPEXSINewData::CalculateNumElectronDrvMu  ----- 

Real
PPEXSINewData::CalculateNumElectronDrvT	( const DistSparseMatrix<Real>& SMat )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateNumElectronDrvT");
#endif
  Real numElecDrvLocal = 0.0, numElecDrv = 0.0;
  DistSparseMatrix<Real>& rhoMat       = rhoRealMat_;     
  DistSparseMatrix<Real>& rhoDrvTMat   = rhoDrvTRealMat_;     

  // TODO Check SMat and rhoDrvTMat has the same sparsity

  if( SMat.size != 0 ){
    // S is not an identity matrix
    numElecDrvLocal = blas::Dot( SMat.nnzLocal, SMat.nzvalLocal.Data(),
        1, rhoDrvTMat.nzvalLocal.Data(), 1 );
  }
  else{
    // S is an identity matrix
    DblNumVec& nzval = rhoDrvTMat.nzvalLocal;
    for( Int i = 0; i < diagIdxLocal_.size(); i++ ){
      numElecDrvLocal += nzval(diagIdxLocal_[i]);
    }
  }
#if ( _DEBUGlevel_ >= 0 )
  statusOFS << std::endl << "numElecDrvLocal = " << numElecDrvLocal << std::endl;
#endif

  mpi::Allreduce( &numElecDrvLocal, &numElecDrv, 1, MPI_SUM, rhoDrvTMat.comm ); 

#ifndef _RELEASE_
  PopCallStack();
#endif

  return numElecDrv;
} 		// -----  end of method PPEXSINewData::CalculateNumElectronDrvT  ----- 


Real
PPEXSINewData::CalculateTotalEnergy	( const DistSparseMatrix<Real>& HMat )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateTotalEnergy");
#endif

  Real totalEnergyLocal = 0.0, totalEnergy = 0.0;
  DistSparseMatrix<Real>& rhoMat           = rhoRealMat_;     

  // TODO Check HMat and rhoMat has the same sparsity

  totalEnergyLocal = blas::Dot( HMat.nnzLocal, HMat.nzvalLocal.Data(),
      1, rhoMat.nzvalLocal.Data(), 1 );
#if ( _DEBUGlevel_ >= 0 )
  statusOFS << std::endl << "TotalEnergyLocal = " << totalEnergyLocal << std::endl;
#endif

  mpi::Allreduce( &totalEnergyLocal, &totalEnergy, 1, MPI_SUM, rhoMat.comm ); 

#ifndef _RELEASE_
  PopCallStack();
#endif

  return totalEnergy;
} 		// -----  end of method PPEXSINewData::CalculateTotalEnergy  ----- 

Real 
PPEXSINewData::CalculateFreeEnergy	( const DistSparseMatrix<Real>& SMat )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateFreeEnergy");
#endif

  Real totalFreeEnergyLocal = 0.0, totalFreeEnergy = 0.0;
  DistSparseMatrix<Real>& hmzMat                   = freeEnergyDensityRealMat_;     

  // TODO Check SMat and hmzMat has the same sparsity

  if( SMat.size != 0 ){
    // S is not an identity matrix
    totalFreeEnergyLocal = blas::Dot( SMat.nnzLocal, SMat.nzvalLocal.Data(),
        1, hmzMat.nzvalLocal.Data(), 1 );
  }
  else{
    // S is an identity matrix
    DblNumVec& nzval = hmzMat.nzvalLocal;
    for( Int i = 0; i < diagIdxLocal_.size(); i++ ){
      totalFreeEnergyLocal += nzval(diagIdxLocal_[i]);
    }
  } // if ( SMat.size != 0 )

#if ( _DEBUGlevel_ >= 0 )
  statusOFS << std::endl << "TotalFreeEnergyLocal = " << totalFreeEnergyLocal << std::endl;
#endif

  mpi::Allreduce( &totalFreeEnergyLocal, &totalFreeEnergy, 1, MPI_SUM, 
      hmzMat.comm ); 

#ifndef _RELEASE_
  PopCallStack();
#endif

  return totalFreeEnergy;
} 		// -----  end of method PPEXSINewData::CalculateFreeEnergy  ----- 


Real
PPEXSINewData::CalculateForce	( 
    const DistSparseMatrix<Real>& HDerivativeMat,  
    const DistSparseMatrix<Real>& SDerivativeMat )
{
#ifndef _RELEASE_
  PushCallStack("PPEXSINewData::CalculateForce");
#endif

  Real totalForceLocal = 0.0, totalForce = 0.0;

  DistSparseMatrix<Real>& rhoMat           = rhoRealMat_;     
  DistSparseMatrix<Real>& frcMat           = energyDensityRealMat_;     

  // TODO Check HDerivativeMat, SDerivativeMat, rhoMat and
  // frcMat has the same sparsity pattern

  totalForceLocal = - blas::Dot( HDerivativeMat.nnzLocal,
      HDerivativeMat.nzvalLocal.Data(), 1, rhoMat.nzvalLocal.Data(), 1
      );

  if( SDerivativeMat.size != 0 ){
    // If S is not an identity matrix, compute the Pulay force
    totalForceLocal += blas::Dot( SDerivativeMat.nnzLocal,
        SDerivativeMat.nzvalLocal.Data(), 1,
        frcMat.nzvalLocal.Data(), 1 );
  }

#if ( _DEBUGlevel_ >= 0 )
  statusOFS << std::endl << "TotalForceLocal = " << totalForceLocal << std::endl;
#endif

  mpi::Allreduce( &totalForceLocal, &totalForce, 1, MPI_SUM, rhoMat.comm );

#ifndef _RELEASE_
  PopCallStack();
#endif

  return totalForce;
} 		// -----  end of method PPEXSINewData::CalculateForce  ----- 



void
PPEXSINewData::DFTDriver (
    Real       numElectronExact,
    Real       temperature,
    Real       gap,
    Real       deltaE,
    Int        numPole, 
    Int        isInertiaCount,
    Int        maxPEXSIIter,
    Real       muMin0,
    Real       muMax0,
    Real       muInertiaTolerance,
    Real       muInertiaExpansion,
    Real       muPEXSISafeGuard,
    Real       numElectronPEXSITolerance,
    Int        matrixType,
    Int        ordering,
    Int        numProcSymbFact,
    Int        verbosity,
    Real&      muPEXSI,                   
    Real&      numElectronPEXSI,         
    Real&      muMinInertia,              
    Real&      muMaxInertia,             
    Int&       numTotalInertiaIter,   
    Int&       numTotalPEXSIIter )
{
#ifndef _RELEASE_
	PushCallStack("PPEXSINewData::DFTDriver");
#endif
  numTotalInertiaIter = 0;
  numTotalPEXSIIter   = 0;

	Real timeSta, timeEnd;
  Real timeInertiaSta, timeInertiaEnd;
  Real timePEXSISta, timePEXSIEnd;
  Real timeTotalSta, timeTotalEnd;
  Real timeInertia = 0.0;
  Real timePEXSI   = 0.0;

  // Initial setup
  muMinInertia = muMin0;
  muMaxInertia = muMax0;

  Real muPEXSI0 = muPEXSISave_;
  Real numElectronDrvMuPEXSI;

  std::string colPerm;
  switch (ordering){
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


  // Main loop: inertia count + PEXSI

  const Int maxTotalInertiaIter = 10;
  bool  isBadBound;
  bool  isConverged;
   
  GetTime( timeTotalSta );
  while( numTotalInertiaIter < maxTotalInertiaIter ){

    // Perform inertia count
    GetTime( timeInertiaSta );
    if( isInertiaCount == 1 ){

      // Number of shifts is exactly determined by the number of
      // independent groups to minimize the cost
      // However, the minimum number of shifts is 10 to accelerate
      // convergence.
      Int numShift = std::max( gridPole_->numProcRow, 10 );
      std::vector<Real>  shiftVec( numShift );
      std::vector<Real>  inertiaVec( numShift );   // Zero temperature
      std::vector<Real>  inertiaFTVec( numShift ); // Finite temperature
      Int maxInertiaIter = std::max( 1, (Int)std::ceil( 
            std::log( (muMax0 - muMin0) / muInertiaTolerance ) /
            std::log( numShift ) ) ); 

      if( verbosity >= 1 ){
        statusOFS << std::endl
          << "From" << std::endl
          << "(muMin0, muMax0)   = " << "(" << muMinInertia << ", " << muMaxInertia
          << ")" << std::endl
          << "muInertiaTolerance = " << muInertiaTolerance << std::endl
          << "numShift           = " << numShift << std::endl
          << "we have " << std::endl
          << "maxInertiaIter     = " << maxInertiaIter << std::endl << std::endl;
      }

      for( Int iter = 1; iter <= maxInertiaIter; iter++ ){
        numTotalInertiaIter++;

        for( Int l = 0; l < numShift; l++ ){
          shiftVec[l] = muMinInertia + l * (muMaxInertia - muMinInertia) / (numShift-1);
        }


        GetTime( timeSta );
        CalculateNegativeInertiaReal(
            shiftVec,
            inertiaVec,
            HRealMat_,
            SRealMat_,
            colPerm,
            numProcSymbFact );

        GetTime( timeEnd );

        // Inertia is multiplied by 2.0 to reflect the doubly occupied
        // orbitals.
        for( Int l = 0; l < numShift; l++ ){
          inertiaVec[l] *= 2.0;
        }

        // Using linear interpolation procedure to compute the finite
        // temperature cumulative DOS
        {
          Int numInp = 1000;   // Number of interpolation points
          Real shiftExpand = 20 * temperature;   // Expand the interval for interpolation
          std::vector<Real>  shiftInpVec( numInp );
          std::vector<Real>  inertiaInpVec( numInp ); 
          std::vector<Real>  fdDrvVec( numInp );
          for( Int l = 0; l < numShift; l++ ){
            Real shiftInp0 = shiftVec[l] - shiftExpand;
            Real shiftInp1 = shiftVec[l] + shiftExpand; 
            Real h = (shiftInp1 - shiftInp0) / (numInp-1);
            for( Int i = 0; i < numInp; i++ ){
              shiftInpVec[i] = shiftInp0 + h * i;
              // fdDrvMu(x) = beta * exp(beta*x)/(1+exp(beta*z))^2
              // Note: compared to fdDrvMu used in pole.cpp, the factor 2.0 is not
              // present, because it is included in inertiaVec. 
              fdDrvVec[i]    = 1.0 / ( 2.0 * temperature * (
                    1.0 + std::cosh( ( shiftInpVec[i] - shiftVec[l] ) / temperature ) ) );
            }
            LinearInterpolation( shiftVec, inertiaVec, 
                shiftInpVec, inertiaInpVec );

            inertiaFTVec[l] = 0.0;
            for( Int i = 0; i < numInp; i++ ){
              inertiaFTVec[l] += fdDrvVec[i] * inertiaInpVec[i] * h;
            }

            if( verbosity >= 1 ){
              statusOFS << std::setiosflags(std::ios::left) 
                << std::setw(LENGTH_VAR_NAME) << "Shift      = "
                << std::setw(LENGTH_VAR_DATA) << shiftVec[l]
                << std::setw(LENGTH_VAR_NAME) << "Inertia    = "
                << std::setw(LENGTH_VAR_DATA) << inertiaVec[l]
                << std::setw(LENGTH_VAR_NAME) << "InertiaFT  = "
                << std::setw(LENGTH_VAR_DATA) << inertiaFTVec[l]
                << std::endl;
            }
          } // for (l)
        }

        if( verbosity >= 1 ){
          statusOFS << std::endl << "Time for iteration " 
            << iter << " of the inertia count is " 
            << timeEnd - timeSta << std::endl;
        }

        // If the chemical potential does not fall into the
        // (muMinInertia, muMaxInertia) bracket, expand the interval.

        const Real EPS = 1e-1;

        isBadBound = false;

        if( inertiaFTVec[0] > numElectronExact ||
            inertiaVec[0] > numElectronExact - EPS ){
          isBadBound = true;
          muMaxInertia = muMinInertia;
          muMinInertia = muMinInertia - muInertiaExpansion;
        }

        if( inertiaFTVec[numShift-1] < numElectronExact ||
            inertiaVec[numShift-1] < numElectronExact + EPS ){
          isBadBound = true;
          muMinInertia = muMaxInertia;
          muMaxInertia = muMaxInertia + muInertiaExpansion;
        }

        if( isBadBound == true ){
          if( verbosity >= 1 ){
            statusOFS << std::endl << std::endl
              << "The solution is not in the provided interval." << std::endl
              << "(muMin, muMax) = ( " << shiftVec[0] << " , " << shiftVec[numShift-1] << " ) " << std::endl
              << "(Ne(muMin), Ne(muMax)) = ( " << inertiaFTVec[0] << " , " << inertiaFTVec[numShift-1] 
              << " ) " << std::endl
              << "NeExact = " << numElectronExact << std::endl
              << "Refine the interval to " << std::endl
              << "(muMin, muMax) = ( " << muMinInertia << " , " << muMaxInertia << " ) " << std::endl;
          }
          // Exit the loop
          break;
        }

        // Update muMin, muMax

        // First find the smallest interval
        Int idxMin = 0, idxMax = numShift-1;
        for( Int i = 0; i < numShift; i++ ){
          if( ( inertiaFTVec[i] < numElectronExact ) &&
              ( inertiaVec[i]   < numElectronExact - EPS ) ){
            idxMin = ( idxMin < i ) ? i : idxMin;
          }
          if( ( inertiaFTVec[i] > numElectronExact ) &&
              ( inertiaVec[i]   > numElectronExact + EPS ) ){
            idxMax = ( idxMax > i ) ? i : idxMax;
          }
        }

        Int idxMid = iround( Real(idxMin + idxMax)/2.0 );

        if( verbosity >= 1 ){
          statusOFS << "idxMin = " << idxMin << ", inertiaVec = " << inertiaVec[idxMin] << std::endl;
          statusOFS << "idxMax = " << idxMax << ", inertiaVec = " << inertiaVec[idxMax] << std::endl;
          statusOFS << "idxMid = " << idxMid << ", inertiaVec = " << inertiaVec[idxMid] << std::endl;
        }


        muMinInertia = shiftVec[idxMin];
        muMaxInertia = shiftVec[idxMax];
        muPEXSI0     = MonotoneRootFinding( shiftVec, inertiaFTVec, numElectronExact );

        // Check convergence. Stop the inertia count after convergence.
        if( ( std::abs( inertiaVec[idxMax] - numElectronExact ) < EPS ) ||
            ( ( shiftVec[idxMax] - shiftVec[idxMin] ) < muInertiaTolerance ) ){
          isInertiaCount = 0;
          // FIXME SOON!
          isConverged = true;
        }
      } // for (iter)
    }
    GetTime( timeInertiaEnd );
    timeInertia += timeInertiaEnd - timeInertiaSta;

    // Immediately continue the inertia counting procedure
    if( isBadBound == true ){
      continue;
    }

    // PEXSI iteration

    CalculateFermiOperatorReal(
        numPole,
        temperature,
        gap,
        deltaE,
        muPEXSI0,
        numElectronExact,
        numElectronPEXSITolerance,
        HRealMat_,
        SRealMat_,
        colPerm,
        numProcSymbFact,
        numElectronPEXSI,
        numElectronDrvMuPEXSI );

    if( verbosity >= 1 ){
      statusOFS << std::endl;
      Print( statusOFS, "mu                          = ", muPEXSI0 );
      Print( statusOFS, "Computed number of electron = ", numElectronPEXSI );
      Print( statusOFS, "Exact number of electron    = ", numElectronExact );
      Print( statusOFS, "d Ne / d mu                 = ", numElectronDrvMuPEXSI );
    }

    if( isConverged == true ){
      break;
    }
    break;
  }

  GetTime( timeTotalEnd );
  
  if( numTotalInertiaIter >= maxTotalInertiaIter ){
    std::ostringstream msg;
    msg  << std::endl
      << maxTotalInertiaIter 
      << " inertia counts have been proceeded without convergence." << std::endl
      << "Try to revise the initial interval for the chemical potential, "
      << "or increase muInertiaTolerance. " << std::endl;
    throw std::runtime_error( msg.str().c_str() );
  }

  if( verbosity == 1 ){
    statusOFS << std::endl
      << "PEXSI has converged!" << std::endl
      << "Total number of inertia counts       = " << numTotalInertiaIter << std::endl
      << "Total number of PEXSI iterations     = " << numTotalPEXSIIter << std::endl 
      << "Total time for inertia counts        = " << timeInertia << " [s]" << std::endl 
      << "Total time for PEXSI iterations      = " << timePEXSI   << " [s]" << std::endl
      << "Total time for the DFT driver        = " << timeTotalEnd - timeTotalSta   << " [s]" << std::endl
      << std::endl;
  }

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method PPEXSINewData::DFTDriver  ----- 



} //  namespace PEXSI
