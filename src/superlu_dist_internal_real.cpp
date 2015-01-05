/*
   Copyright (c) 2012 The Regents of the University of California,
   through Lawrence Berkeley National Laboratory.  

Author: Lin Lin and Mathias Jacquelin

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
/// @file superlu_dist_internal_real.cpp
/// @brief Implementation of internal structures for interfacing with SuperLU_Dist (version 3.0 and later) for real arithmetic
/// @date 2014-03-17
#include "pexsi/SuperLUGrid.hpp"
#include "pexsi/superlu_dist_internal.hpp"

#include "pexsi/pselinv.hpp"

#include <superlu_ddefs.h>
#include <numeric>

#include <Cnames.h>
extern "C"{ void
pdsymbfact(superlu_options_t *options, SuperMatrix *A, 
    ScalePermstruct_t *ScalePermstruct, gridinfo_t *grid,
    LUstruct_t *LUstruct, SuperLUStat_t *stat, int *numProcSymbFact,
    int *info, double *totalMemory, double *maxMemory );
}


// SuperLUGrid class
namespace PEXSI{


  class RealGridInfo{
    friend class RealGridData;
    friend class RealSuperLUData;
    protected:
    gridinfo_t          grid;
  };

  RealGridData::RealGridData(){
    info_ = new RealGridInfo;
  }

  RealGridData::~RealGridData(){
    delete info_;
  }



  void RealGridData::GridInit( MPI_Comm comm, Int nprow, Int npcol ){
    superlu_gridinit(comm, nprow, npcol, &info_->grid);
  }

  void RealGridData::GridExit(  ){
    superlu_gridexit(&info_->grid);
  }
}

// SuperLUData class
namespace PEXSI{

  class RealSuperLUData_internal{
    friend class RealSuperLUData;
    protected:
    /// @brief SuperLU matrix. 
    SuperMatrix         A;                        

    /// @brief SuperLU options. 
    ///
    /// Note
    /// ----
    ///
    /// It is important to have 
    ///
    /// options.RowPerm           = NOROWPERM;
    /// 
    /// to make sure that symmetric permutation is used.
    ///
    superlu_options_t   options;                  

    /// @brief Saves the permutation vectors.  Only perm_c (permutation of
    /// column as well as rows due to the symmetric permutation) will be used.
    ScalePermstruct_t   ScalePermstruct;          

    /// @brief SuperLU grid structure.
    gridinfo_t*         grid;

    /// @brief Saves the supernodal partition as well as the numerical
    /// values and structures of the L and U structure.
    LUstruct_t          LUstruct;

    /// @brief Used for solve for multivectors.
    SOLVEstruct_t       SOLVEstruct;

    /// @brief SuperLU statistics
    SuperLUStat_t       stat;

    /// @brief Number of processors used for parallel symbolic
    /// factorization and PARMETIS/PT-SCOTCH
    Int                 numProcSymbFact;

    /// @brief SuperLU information
    Int                 info;

    // The following are for consistency checks
    bool                isSuperMatrixAllocated;
    bool                isSuperMatrixFactorized;
    bool                isScalePermstructAllocated;
    bool                isLUstructAllocated;

    Int maxDomains;

  };


  RealSuperLUData::RealSuperLUData( const SuperLUGrid<Real>& g, const SuperLUOptions& opt ){
#ifndef _RELEASE_
    PushCallStack("RealSuperLUData::RealSuperLUData");
#endif
    ptrData = new RealSuperLUData_internal;
    if( ptrData == NULL ){
#ifdef USE_ABORT
      abort();
#endif
      throw std::runtime_error( "SuperLUMatrix cannot be allocated." );
    }	
    ptrData->isSuperMatrixAllocated     = false;
    ptrData->isScalePermstructAllocated = false;
    ptrData->isLUstructAllocated        = false;
    ptrData->numProcSymbFact            = opt.numProcSymbFact;

    // Options
    superlu_options_t& options = ptrData->options;

    set_default_options_dist(&options);

    // The default value of ColPerm uses the default value from SuperLUOptions
    options.Fact              = DOFACT;
    options.RowPerm           = NOROWPERM; // IMPORTANT for symmetric matrices
    options.IterRefine        = NOREFINE;
    options.ParSymbFact       = NO;
    options.Equil             = YES; 
    options.ReplaceTinyPivot  = YES; //TODO might be safe to turn on

    // For output information such as # of nonzeros in L and U
    // and the memory cost, set PrintStat = YES
    options.PrintStat         = NO;
    options.SolveInitialized  = NO;
    // Necessary to invoke static scheduling of SuperLU
    options.lookahead_etree   = YES;
    options.SymPattern        = YES;

    if ( opt.ColPerm == "NATURAL" ){
      options.ColPerm = NATURAL;
    } 
    else if( opt.ColPerm == "MMD_AT_PLUS_A" ){
      options.ColPerm = MMD_AT_PLUS_A;
    }
    else if( opt.ColPerm == "METIS_AT_PLUS_A" ){
      options.ColPerm = METIS_AT_PLUS_A;
    }
    else if( opt.ColPerm == "PARMETIS" ){
      options.ColPerm           = PARMETIS;
      options.ParSymbFact       = YES;
      //		options.ParSymbFact       = NO;
    }
    else{
      std::ostringstream msg;
      msg << opt.ColPerm << " is not a supported ColPerm type. Try (case sensitive) " << std::endl
        << "NATURAL | MMD_AT_PLUS_A | METIS_AT_PLUS_A | PARMETIS" << std::endl;
#ifdef USE_ABORT
      abort();
#endif
      throw std::runtime_error( msg.str().c_str() );
    }

    // Setup grids
    ptrData->grid = &(g.ptrData->info_->grid);


#ifndef _RELEASE_
    PopCallStack();
#endif



  }


  RealSuperLUData::~RealSuperLUData(){
#ifndef _RELEASE_
    PushCallStack("RealSuperLUData::~RealSuperLUData");
#endif
    SuperMatrix& A = ptrData->A;
    if( ptrData->isLUstructAllocated ){
      Destroy_LU(A.ncol, ptrData->grid, &ptrData->LUstruct);
      LUstructFree(&ptrData->LUstruct); 
    }
    if( ptrData->isScalePermstructAllocated ){
      ScalePermstructFree(&ptrData->ScalePermstruct);
    }
    if( ptrData->options.SolveInitialized ){
      // TODO real arithmetic
      dSolveFinalize(&ptrData->options, &ptrData->SOLVEstruct);
    }
    if( ptrData->isSuperMatrixAllocated ){
      this->DestroyAOnly();
    }

    delete ptrData;

#ifndef _RELEASE_
    PopCallStack();
#endif
  }


  Int RealSuperLUData::m (  ) const	
  {
    return ptrData->A.nrow;
  } 		// -----  end of method RealSuperLUData::m  ----- 


  Int RealSuperLUData::n (  ) const	
  {
    return ptrData->A.ncol;
  } 		// -----  end of method RealSuperLUData::n  ----- 

  void RealSuperLUData::DistSparseMatrixToSuperMatrixNRloc( DistSparseMatrix<Real>& sparseA , const SuperLUOptions & options)
  {
#ifndef _RELEASE_
    PushCallStack( "RealSuperLUData::DistSparseMatrixToSuperMatrixNRloc" );
#endif
    if( ptrData->isSuperMatrixAllocated == true ){
#ifdef USE_ABORT
      abort();
#endif
      throw std::logic_error( "SuperMatrix is already allocated." );
    }
    gridinfo_t* grid = ptrData->grid;

    Int mpirank = grid->iam;
    Int mpisize = grid->nprow * grid->npcol;

    int_t *colindLocal, *rowptrLocal;
    double *nzvalLocal;

    Int numRowLocalFirst = sparseA.size / mpisize;
    Int firstRow = mpirank * numRowLocalFirst;
    Int numRowLocal = -1;
    Int nnzLocal = -1;

    if(options.symmetric == 1 ){
      numRowLocal = sparseA.colptrLocal.m() - 1;
      nnzLocal = sparseA.nnzLocal;

      colindLocal = (int_t*)intMalloc_dist(sparseA.nnzLocal); 
      nzvalLocal  = (double*)doubleMalloc_dist(sparseA.nnzLocal);
      rowptrLocal = (int_t*)intMalloc_dist(numRowLocal+1);

      std::copy( sparseA.colptrLocal.Data(), sparseA.colptrLocal.Data() + sparseA.colptrLocal.m(),
          rowptrLocal );
      std::copy( sparseA.rowindLocal.Data(), sparseA.rowindLocal.Data() + sparseA.rowindLocal.m(),
          colindLocal );
      std::copy( sparseA.nzvalLocal.Data(), sparseA.nzvalLocal.Data() + sparseA.nzvalLocal.m(),
          nzvalLocal );

    }
    else{
      DistSparseMatrix<Real> sparseB;
      CSCToCSR(sparseA,sparseB);

      numRowLocal = sparseB.colptrLocal.m() - 1;
      nnzLocal = sparseB.nnzLocal;

      colindLocal = (int_t*)intMalloc_dist(sparseB.nnzLocal); 
      nzvalLocal  = (double*)doubleMalloc_dist(sparseB.nnzLocal);
      rowptrLocal = (int_t*)intMalloc_dist(numRowLocal+1);

      std::copy( sparseB.colptrLocal.Data(), sparseB.colptrLocal.Data() + sparseB.colptrLocal.m(),
          rowptrLocal );
      std::copy( sparseB.rowindLocal.Data(), sparseB.rowindLocal.Data() + sparseB.rowindLocal.m(),
          colindLocal );
      std::copy( sparseB.nzvalLocal.Data(), sparseB.nzvalLocal.Data() + sparseB.nzvalLocal.m(),
          (Complex*)nzvalLocal );
    }



    // Important to adjust from FORTRAN convention (1 based) to C convention (0 based) indices
    for(Int i = 0; i < sparseA.rowindLocal.m(); i++){
      colindLocal[i]--;
    }

    for(Int i = 0; i < sparseA.colptrLocal.m(); i++){
      rowptrLocal[i]--;
    }

    // Construct the distributed matrix according to the SuperLU_DIST format
    dCreate_CompRowLoc_Matrix_dist(&ptrData->A, sparseA.size, sparseA.size, sparseA.nnzLocal, 
        numRowLocal, firstRow,
        nzvalLocal, colindLocal, rowptrLocal,
        SLU_NR_loc, SLU_D, SLU_GE);

    ptrData->isSuperMatrixAllocated = true;

#ifndef _RELEASE_
    PopCallStack();
#endif

    return;

  } 		// -----  end of method RealSuperLUData::DistSparseMatrixToSuperMatrixNRloc ----- 

  void
    RealSuperLUData::DestroyAOnly	(  )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::DestroyAOnly");
#endif
      if( ptrData->isSuperMatrixAllocated == false ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "SuperMatrix has not been allocated." );
      }
      switch ( ptrData->A.Stype ){
        case SLU_NC:
          Destroy_CompCol_Matrix_dist(&ptrData->A);
          break;
        case SLU_NR_loc:
          Destroy_CompRowLoc_Matrix_dist(&ptrData->A);
          break;
        default:
          std::ostringstream msg;
          msg << "Type " << SLU_NR_loc << " is to be destroyed" << std::endl
            << "This is an unsupported SuperMatrix format to be destroyed." << std::endl;
#ifdef USE_ABORT
          abort();
#endif
          throw std::runtime_error( msg.str().c_str() );
      }
      ptrData->isSuperMatrixAllocated = false;
#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::DestroyAOnly  ----- 

  void
    RealSuperLUData::SymbolicFactorize	(  )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::SymbolicFactorize");
#endif
      if( ptrData->isScalePermstructAllocated ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "ScalePermstruct is already allocated." );
      }
      if( ptrData->isLUstructAllocated){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "LUstruct is already allocated." );
      }
      if( ptrData->options.RowPerm != NOROWPERM ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "For PEXSI there must be no row permutation." );
      }

      SuperMatrix&  A = ptrData->A;

      ScalePermstructInit(A.nrow, A.ncol, &ptrData->ScalePermstruct);
      LUstructInit(A.nrow, A.ncol, &ptrData->LUstruct);

      PStatInit(&ptrData->stat);
#if ( _DEBUGlevel_ >= 1 )
      statusOFS << "Before symbfact subroutine." << std::endl;
#endif

      double totalMemory, maxMemory;


      pdsymbfact(&ptrData->options, &A, &ptrData->ScalePermstruct, ptrData->grid, 
          &ptrData->LUstruct, &ptrData->stat, &ptrData->numProcSymbFact, &ptrData->info,
          &totalMemory, &maxMemory);
      PStatFree(&ptrData->stat);


      //assert(ptrData->ScalePermstruct.DiagScale == BOTH );


#if ( _DEBUGlevel_ >= 0 )
      statusOFS << "Memory cost of symbolic factorization (MB): " << std::endl;
      statusOFS << "Total: " << totalMemory << ", Average: " << 
        totalMemory / ( ptrData->grid->nprow * ptrData->grid->npcol )
        << ", Max: " << maxMemory << std::endl << std::endl;
#endif



      ptrData->isScalePermstructAllocated = true;
      ptrData->isLUstructAllocated        = true; 

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::SymbolicFactorize  ----- 

  void
    RealSuperLUData::Distribute	(  )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::Distribute");
#endif
      if( ptrData->isScalePermstructAllocated == false ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "ScalePermstruct has not been allocated by SymbolicFactorize." );
      }	
      if( ptrData->isLUstructAllocated == false ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "LUstruct has not been allocated by SymbolicFactorize." );
      }	
      if( ptrData->isSuperMatrixAllocated == false ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "SuperMatrix has not been allocated." );
      }	

      Int* perm_c = ptrData->ScalePermstruct.perm_c;
      NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;
      Int* colind = Astore->colind;
      Int  nnzLocal = Astore->nnz_loc;
      // Apply column permutation to the original distributed A
      for(Int j = 0; j < nnzLocal; j++)
        colind[j] = perm_c[colind[j]];
      // Distribute Pc*Pr*diag(R)*A*diag(C)*Pc' into L and U storage.  
      // NOTE: the row permutation Pc*Pr is applied internally in the
      // distribution routine. 
      float dist_mem_use = pddistribute(SamePattern_SameRowPerm, ptrData->A.nrow, 
          &ptrData->A, &ptrData->ScalePermstruct, NULL, &ptrData->LUstruct, ptrData->grid);

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::Distribute  ----- 

  void
    RealSuperLUData::NumericalFactorize	(  )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::NumericalFactorize");
#endif
      if( !ptrData->isLUstructAllocated ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::logic_error( "LUstruct has not been allocated." );
      }
      // Estimate the 1-norm
      char norm[1]; *norm = '1';
      double anorm = pdlangs( norm, &ptrData->A, ptrData->grid );

      PStatInit(&ptrData->stat);
      pdgstrf(&ptrData->options, ptrData->A.nrow, ptrData->A.ncol, 
          anorm, &ptrData->LUstruct, ptrData->grid, &ptrData->stat, &ptrData->info); 

#ifdef _PRINT_STATS_
      statusOFS<<"******************SUPERLU STATISTICS****************"<<std::endl;  
      statusOFS<<"Number of tiny pivots: "<<ptrData->stat.TinyPivots<<std::endl;  
      statusOFS<<"Number of look aheads: "<<ptrData->stat.num_look_aheads<<std::endl;  
      statusOFS<<"Number of FLOPS during factorization: "<<ptrData->stat.ops[FACT]<<std::endl;  
      statusOFS<<"****************************************************"<<std::endl;  
#endif


      PStatFree(&ptrData->stat);



      if( ptrData->info ){
        std::ostringstream msg;
        msg << "Numerical factorization error, info =  " << ptrData->info << std::endl;
#ifdef USE_ABORT
        abort();
#endif
        throw std::runtime_error( msg.str().c_str() );
      }

      //assert(ptrData->ScalePermstruct.DiagScale == BOTH );

      // Prepare for Solve.
      ptrData->options.Fact = FACTORED;

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::NumericalFactorize  ----- 

  void
    RealSuperLUData::ConvertNRlocToNC	( RealSuperLUData * aptrData )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::ConvertNRlocToNC");
#endif
      if( !ptrData->isSuperMatrixAllocated ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::runtime_error( "The local SuperMatrix has not been allocated." );
      }
      if( aptrData->ptrData->isSuperMatrixAllocated ){
#ifdef USE_ABORT
        abort();
#endif
        throw std::runtime_error( "The global SuperMatrix has been allocated." );
      }
      // TODO make sure the two grids are the same

      // TODO real arithmetic
      const Int NEED_VALUE = 1;
      pdCompRow_loc_to_CompCol_global(NEED_VALUE, &ptrData->A, ptrData->grid, 
          &aptrData->ptrData->A);

      ptrData->isSuperMatrixAllocated = true;

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::ConvertNRlocToNC  ----- 

  void
    RealSuperLUData::MultiplyGlobalMultiVector	( NumMat<Real>& xGlobal, NumMat<Real>& bGlobal )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::MultiplyGlobalMultiVector");
#endif
      char trans[1]; *trans = 'N';
      Int m = xGlobal.m();
      Int nrhs = xGlobal.n();
      bGlobal.Resize( m, nrhs );

      if( ptrData->A.Stype != SLU_NC ){
        std::ostringstream msg;
        msg << "MultiplyGlobalMultiVector requires SLU_NC matrix with type " << SLU_NC << std::endl
          << "The matrix is of type " << ptrData->A.Stype << std::endl
          << "Consider using ConvertNRlocToNC subroutine" << std::endl;
#ifdef USE_ABORT
        abort();
#endif
        throw std::runtime_error( msg.str().c_str() );
      }	
      dFillRHS_dist(trans, nrhs, (double*)xGlobal.Data(), m, 
          &ptrData->A, (double*) bGlobal.Data(), m);
#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::MultiplyGlobalMultiVector  ----- 


  void
    RealSuperLUData::DistributeGlobalMultiVector	( NumMat<Real>& xGlobal, NumMat<Real>& xLocal )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::DistributeGlobalMultiVector");
#endif
      SuperMatrix& A = ptrData->A;
      if( ptrData->A.Stype != SLU_NR_loc ){
        std::ostringstream msg;
        msg << "DistributeGlobalMultiVector requires SLU_NR_loc matrix with type " << SLU_NR_loc << std::endl
          << "The matrix is of type " << ptrData->A.Stype << std::endl;
#ifdef USE_ABORT
        abort();
#endif
        throw std::runtime_error( msg.str().c_str() );
      }	

      NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;

      Int numRowLocal = Astore->m_loc;
      Int firstRow    = Astore->fst_row;
      Int nrhs = xGlobal.n();

      xLocal.Resize( numRowLocal, nrhs );
      SetValue( xLocal, ZERO<Real>() );
      for( Int j = 0; j < nrhs; j++ ){
        std::copy( xGlobal.VecData(j)+firstRow, xGlobal.VecData(j)+firstRow+numRowLocal,
            xLocal.VecData(j) );
      }

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::DistributeGlobalMultiVector  ----- 


  void RealSuperLUData::GatherDistributedMultiVector	( NumMat<Real>& xGlobal, NumMat<Real>& xLocal )
  {
#ifndef _RELEASE_
    PushCallStack("RealSuperLUData::GatherDistributedMultiVector");
#endif
    SuperMatrix& A = ptrData->A;
    if( ptrData->A.Stype != SLU_NR_loc ){
      std::ostringstream msg;
      msg << "GatherDistributedMultiVector requires SLU_NR_loc matrix with type " << SLU_NR_loc << std::endl
        << "The matrix is of type " << ptrData->A.Stype << std::endl;
#ifdef USE_ABORT
      abort();
#endif
      throw std::runtime_error( msg.str().c_str() );
    }	

    NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;

    Int numRowLocal = Astore->m_loc;
    Int firstRow    = Astore->fst_row;
    Int nrhs = xGlobal.n();

    Int maxRows = 0;
    Int localRows = xLocal.m();

    MPI_Allreduce(&localRows,&maxRows,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);



    NumMat<Real> tmpLocal(xGlobal.m(),nrhs);
    SetValue( tmpLocal, ZERO<Real>() );
    for( Int j = 0; j < nrhs; j++ ){
      std::copy( xLocal.VecData(j), xLocal.VecData(j)+numRowLocal, tmpLocal.VecData(j)+firstRow );
    }

    MPI_Allreduce(tmpLocal.Data(),xGlobal.Data(),2*tmpLocal.m()*tmpLocal.n(),MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

#ifndef _RELEASE_
    PopCallStack();
#endif

    return ;
  } 		// -----  end of method RealSuperLUData::GatherDistributedMultiVector  ----- 


  void
    RealSuperLUData::SolveDistMultiVector	( NumMat<Real>& bLocal, DblNumVec& berr )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::SolveDistMultiVector");
#endif
      Int nrhs = bLocal.n();
      NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;
      Int numRowLocal = Astore->m_loc;
      Int firstRow    = Astore->fst_row;

      berr.Resize( nrhs );

      // TODO Real arithmetic

      PStatInit(&ptrData->stat);
      pdgssvx(&ptrData->options, &ptrData->A, &ptrData->ScalePermstruct, 
          (double*)bLocal.Data(), numRowLocal, nrhs, ptrData->grid,
          &ptrData->LUstruct, &ptrData->SOLVEstruct, berr.Data(), 
          &ptrData->stat, &ptrData->info);
      PStatFree(&ptrData->stat); 

      if ( ptrData->options.SolveInitialized ) {
        dSolveFinalize(&ptrData->options, &ptrData->SOLVEstruct);
      }

      if( ptrData->info ){
        std::ostringstream msg;
        msg << "Numerical solve error, info =  " << ptrData->info << std::endl;
#ifdef USE_ABORT
        abort();
#endif
        throw std::runtime_error( msg.str().c_str() );
      }




#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::SolveDistMultiVector  ----- 


  void
    RealSuperLUData::CheckErrorDistMultiVector	( NumMat<Real>& xLocal, NumMat<Real>& xTrueLocal )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::CheckErrorDistMultiVector");
#endif
      Int nrhs = xLocal.n();
      NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;
      Int numRowLocal = Astore->m_loc;

      pdinf_norm_error(ptrData->grid->iam, numRowLocal, nrhs, 
          (double*)xLocal.Data(), numRowLocal, 
          (double*)xTrueLocal.Data(), numRowLocal, ptrData->grid);

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::CheckErrorDistMultiVector  ----- 


  void
    RealSuperLUData::LUstructToPMatrix	( PMatrix<Real>& PMloc )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::LUstructToPMatrix");
#endif
      const LocalLU_t* Llu   = ptrData->LUstruct.Llu;
      const GridType* grid   = PMloc.Grid();
      const SuperNodeType* super = PMloc.SuperNode();
      Int numSuper = PMloc.NumSuper();

      // L part   
#ifndef _RELEASE_
      PushCallStack("L part");
#endif
#if ( _DEBUGlevel_ >= 1 )
      statusOFS << std::endl << "LUstructToPMatrix::L part" << std::endl;
#endif

      PMloc.ColBlockIdx().clear();
      PMloc.RowBlockIdx().clear();
      PMloc.ColBlockIdx().resize( PMloc.NumLocalBlockCol() );
      PMloc.RowBlockIdx().resize( PMloc.NumLocalBlockRow() );

      for( Int jb = 0; jb < PMloc.NumLocalBlockCol(); jb++ ){
        Int bnum = GBj( jb, grid );
        if( bnum >= numSuper ) continue;

        Int cnt = 0;                                // Count for the index in LUstruct
        Int cntval = 0;                             // Count for the nonzero values
        Int cntidx = 0;                             // Count for the nonzero block indexes
        const Int* index = Llu->Lrowind_bc_ptr[jb];
        if( index ){ 
          // Not an empty column, start a new column then.
          std::vector<LBlock<Real> >& Lcol = PMloc.L(jb);
          Lcol.resize( index[cnt++] );


          Int lda = index[cnt++];


          Int savCnt = cnt;
          IntNumVec LIndices(Lcol.size());
          for( Int iblk = 0; iblk < Lcol.size(); iblk++ ){
            Int blockIdx    = index[cnt++];
            Int numRow      = index[cnt++];
            cnt += numRow;
            LIndices[iblk] = blockIdx;
          }
          //      statusOFS<<"Unsorted blockidx for L are: "<<LIndices<<std::endl;
          //sort the array
          IntNumVec sortedIndices(Lcol.size());
          for(Int i = 0; i<sortedIndices.m(); ++i){ sortedIndices[i] = i;}
          ULComparator cmp2(LIndices);
          //sort the row indices (so as to speedup the index lookup
          std::sort(sortedIndices.Data(),sortedIndices.Data()+sortedIndices.m(),cmp2);

          //      statusOFS<<"Sorted indices for L are: "<<sortedIndices<<std::endl;
          //get the inverse perm
          IntNumVec tmp = sortedIndices;
          for(Int i = 0; i<sortedIndices.m(); ++i){ tmp[sortedIndices[i]] = i;}
          sortedIndices = tmp;

          cnt = savCnt;
          for( Int iblk = 0; iblk < Lcol.size(); iblk++ ){

            Int blockIdx    = index[cnt++];
            //determine where to put it
            LBlock<Real> & LB     = Lcol[sortedIndices[iblk]];
            LB.blockIdx    = blockIdx;

            PMloc.ColBlockIdx(jb).push_back(LB.blockIdx);
            Int LBi = LB.blockIdx / grid->numProcRow; 
            PMloc.RowBlockIdx( LBi ).push_back( bnum );


            LB.numRow      = index[cnt++];
            LB.numCol      = super->superPtr[bnum+1] - super->superPtr[bnum];
            LB.rows        = IntNumVec( LB.numRow, true, const_cast<Int*>(&index[cnt]) );

            IntNumVec rowsPerm(LB.numRow);
            IntNumVec rowsSorted(LB.numRow);
            for(Int i = 0; i<rowsPerm.m(); ++i){ rowsPerm[i] = i;}
            ULComparator cmp(LB.rows);
            //sort the row indices (so as to speedup the index lookup
            std::sort(rowsPerm.Data(),rowsPerm.Data()+rowsPerm.m(),cmp);

            for(Int i = 0; i<LB.rows.m(); ++i){ 
              rowsSorted[i] = LB.rows[rowsPerm[i]];
            }

            LB.rows = rowsSorted;

            LB.nzval.Resize( LB.numRow, LB.numCol );   
            SetValue( LB.nzval, ZERO<Real>() ); 
            cnt += LB.numRow;

            //sort the nzval
            for(Int j = 0; j<LB.numCol; ++j){
              for(Int i = 0; i<LB.numRow; ++i){
                LB.nzval(i,j) = ((Real*)(Llu->Lnzval_bc_ptr[jb]+cntval))[rowsPerm[i]+j*lda];
              }
            }

            cntval += LB.numRow;


#if ( _DEBUGlevel_ >= 1 )
            statusOFS 
              << "L part: bnum = " << bnum << ", numBlock = " << Lcol.size()
              << ", blockIdx = " << LB.blockIdx
              << ", numRow = " << LB.numRow 
              << ", numCol = " << LB.numCol << std::endl;
#endif 

          } // for(iblk)

#if ( _DEBUGlevel_ >= 2 )
          statusOFS<<"Real Sorted blockidx for L are: ";
          for( Int iblk = 0; iblk < Lcol.size(); iblk++ ){
            statusOFS<<Lcol[iblk].blockIdx<<" ";
          }
          statusOFS<<std::endl;
#endif
        }  // if(index)
      } // for(jb)
#ifndef _RELEASE_
      PopCallStack();
#endif

      // U part
#ifndef _RELEASE_
      PushCallStack("U part");
#endif
#if ( _DEBUGlevel_ >= 1 )
      statusOFS << std::endl << "LUstructToPMatrix::U part" << std::endl;
#endif
      for( Int ib = 0; ib < PMloc.NumLocalBlockRow(); ib++ ){
        Int bnum = GBi( ib, grid );
        if( bnum >= numSuper ) continue;

        Int cnt = 0;                                // Count for the index in LUstruct
        Int cntval = 0;                             // Count for the nonzero values
        Int cntidx = 0;                             // Count for the nonzero block indexes
        const Int*    index = Llu->Ufstnz_br_ptr[ib]; 
        const Real* pval  = reinterpret_cast<const Real*>(Llu->Unzval_br_ptr[ib]);
        if( index ){ 
          // Not an empty row
          // Compute the number of nonzero columns 
          std::vector<UBlock<Real> >& Urow = PMloc.U(ib);
          Urow.resize( index[cnt++] );
          cnt = BR_HEADER;


          std::vector<Int> cols;                    //Save the nonzero columns in the current block
          for(Int jblk = 0; jblk < Urow.size(); jblk++ ){
            cols.clear();
            UBlock<Real> & UB = Urow[jblk];
            UB.blockIdx = index[cnt];



            PMloc.RowBlockIdx(ib).push_back(UB.blockIdx);
            Int LBj = UB.blockIdx / grid->numProcCol; 
            PMloc.ColBlockIdx( LBj ).push_back( bnum );


            UB.numRow = super->superPtr[bnum+1] - super->superPtr[bnum];
            cnt += UB_DESCRIPTOR;
            for( Int j = FirstBlockCol( UB.blockIdx, super ); 
                j < FirstBlockCol( UB.blockIdx+1, super ); j++ ){
              Int firstRow = index[cnt++];
              if( firstRow != FirstBlockCol( bnum+1, super ) )
                cols.push_back(j);
            }
            // Rewind the index
            cnt -= super->superPtr[UB.blockIdx+1] - super->superPtr[UB.blockIdx];

            UB.numCol = cols.size();
            UB.cols   = IntNumVec( cols.size(), true, &cols[0] );
            UB.nzval.Resize( UB.numRow, UB.numCol );
            SetValue( UB.nzval, ZERO<Real>() );

            Int cntcol = 0;
            for( Int j = 0; 
                j < super->superPtr[UB.blockIdx+1] - super->superPtr[UB.blockIdx]; j++ ){
              Int firstRow = index[cnt++];
              if( firstRow != FirstBlockCol( bnum+1, super ) ){
                Int tnrow = FirstBlockCol( bnum+1, super ) - firstRow;
                lapack::Lacpy( 'A', tnrow, 1, &pval[cntval], tnrow,
                    &UB.nzval(firstRow - FirstBlockCol(bnum, super), cntcol),
                    UB.numRow );
                cntcol ++;
                cntval += tnrow;
              }
            } // for( j )

#if ( _DEBUGlevel_ >= 1 )
            statusOFS 
              << "U part: bnum = " << bnum << ", numBlock = " << Urow.size()
              << ", blockIdx = " << UB.blockIdx
              << ", numRow = " << UB.numRow 
              << ", numCol = " << UB.numCol << std::endl;
#endif 

          } // for (jblk)



        } // if( index )

      } // for(ib)
#ifndef _RELEASE_
      PopCallStack();
#endif

      for( Int ib = 0; ib < PMloc.NumLocalBlockRow(); ib++ ){
        std::vector<Int> & rowBlockIdx = PMloc.RowBlockIdx(ib);
        std::sort(rowBlockIdx.begin(),rowBlockIdx.end());
      }

      for( Int jb = 0; jb < PMloc.NumLocalBlockCol(); jb++ ){
        std::vector<Int> & colBlockIdx = PMloc.ColBlockIdx(jb);
        std::sort(colBlockIdx.begin(),colBlockIdx.end());
      }

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::LUstructToPMatrix  ----- 



  void
    RealSuperLUData::SymbolicToSuperNode	( SuperNodeType& super )
    {
#ifndef _RELEASE_
      PushCallStack("RealSuperLUData::SymbolicToSuperNode");
#endif
      Int n = ptrData->A.ncol;
      // Permutation vector
      Int *perm_c = ptrData->ScalePermstruct.perm_c;
      super.perm.Resize( n );
      std::copy( perm_c, perm_c + n, super.perm.Data() );

      // Construct the inverse permutation vector
      super.permInv.Resize( n );
      for( Int i = 0; i < n; i++ ){
        super.permInv(i) = i;
      }

      std::sort( super.permInv.Data(), super.permInv.Data() + n,
          IndexComp<IntNumVec&>(super.perm) );


      // Supernodal information

      Int *xsup    = ptrData -> LUstruct.Glu_persist -> xsup;
      Int *superno = ptrData -> LUstruct.Glu_persist -> supno;
      Int *etree = ptrData -> LUstruct.etree;

      Int numSuper = superno[ n-1 ] + 1;
      super.superPtr.Resize( numSuper + 1 );
      std::copy( xsup, xsup + numSuper + 1, super.superPtr.Data() );
      super.superIdx.Resize( n );
      std::copy( superno, superno + n, super.superIdx.Data() );
      super.etree.Resize( n );
      std::copy( etree, etree + n, super.etree.Data() );

#ifndef _RELEASE_
      PopCallStack();
#endif

      return ;
    } 		// -----  end of method RealSuperLUData::SymbolicToSuperNode  ----- 


}
