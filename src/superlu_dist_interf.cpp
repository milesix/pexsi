/// @file superlu_dist_interf.cpp
/// @brief Interface to SuperLU_Dist.
/// @author Lin Lin
/// @version 0.1
/// @date 2012-11-15
#include "superlu_dist_interf.hpp"
#ifdef _USE_COMPLEX_
#include "superlu_zdefs.h"
#include "Cnames.h"
extern "C"{
void
pzsymbfact(superlu_options_t *options, SuperMatrix *A, 
		ScalePermstruct_t *ScalePermstruct, gridinfo_t *grid,
		LUstruct_t *LUstruct, SuperLUStat_t *stat, int *info);
}
#else
#include "superlu_ddefs.h"
#include "Cnames.h"
extern "C"{ void
pdsymbfact(superlu_options_t *options, SuperMatrix *A, 
		ScalePermstruct_t *ScalePermstruct, gridinfo_t *grid,
		LUstruct_t *LUstruct, SuperLUStat_t *stat, int *info);
}
#endif

// SuperLUGrid class
namespace PEXSI{

struct SuperLUGrid::GridData{
	gridinfo_t          grid;
};


SuperLUGrid::SuperLUGrid	( MPI_Comm comm, Int nprow, Int npcol )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUGrid::SuperLUGrid");
#endif
	ptrData = new GridData;
	if( ptrData == NULL )
		throw std::runtime_error( "SuperLUGrid cannot be allocated." );

	superlu_gridinit(comm, nprow, npcol, &ptrData->grid);


#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUGrid::SuperLUGrid  ----- 


SuperLUGrid::~SuperLUGrid	(  )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUGrid::~SuperLUGrid");
#endif
	superlu_gridexit(&ptrData->grid);

	delete ptrData;

#ifndef _RELEASE_
	PopCallStack();
#endif
	return ;
} 		// -----  end of method SuperLUGrid::~SuperLUGrid  ----- 

} // namespace PEXSI

// SuperLUMatrix class
namespace PEXSI{

struct SuperLUMatrix::SuperLUData{
	SuperMatrix         A;                        ///< SuperLU matrix. 
	superlu_options_t   options;                  ///< SuperLU options.
	ScalePermstruct_t   ScalePermstruct;          ///< Permutation vectors. 
	gridinfo_t          *grid;
	LUstruct_t          LUstruct;
	SOLVEstruct_t       SOLVEstruct;
	SuperLUStat_t       stat;
	Int                 info;

	bool                isSuperMatrixAllocated;
	bool                isSuperMatrixFactorized;
	bool                isScalePermstructAllocated;
	bool                isLUstructAllocated;
};

SuperLUMatrix::SuperLUMatrix	( SuperLUGrid& g )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::SuperLUMatrix");
#endif
	ptrData = new SuperLUData;
	if( ptrData == NULL )
		throw std::runtime_error( "SuperLUMatrix cannot be allocated." );
	
	ptrData->isSuperMatrixAllocated     = false;
	ptrData->isScalePermstructAllocated = false;
	ptrData->isLUstructAllocated        = false;

	// Options
	superlu_options_t& options = ptrData->options;
	
	set_default_options_dist(&options);
	options.Fact              = DOFACT;
	options.RowPerm           = NOROWPERM; // IMPORTANT for symmetric matrices
	options.IterRefine        = NOREFINE;
	options.ParSymbFact       = NO;
	options.Equil             = NO; 
	options.ReplaceTinyPivot  = NO;
	// options.ColPerm           = MMD_AT_PLUS_A;
	options.ColPerm           = METIS_AT_PLUS_A;
	options.PrintStat         = YES;
	options.SolveInitialized  = NO;

	// Setup grids
  ptrData->grid = &(g.ptrData->grid);

#ifndef _RELEASE_
	PopCallStack();
#endif
} 		// -----  end of method SuperLUMatrix::SuperLUMatrix  ----- 

SuperLUMatrix::~SuperLUMatrix	(  )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::~SuperLUMatrix");
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
		zSolveFinalize(&ptrData->options, &ptrData->SOLVEstruct);
	}
	if( ptrData->isSuperMatrixAllocated ){
		this->DestroyAOnly();
	}
	
	delete ptrData;

#ifndef _RELEASE_
	PopCallStack();
#endif
} 		// -----  end of method SuperLUMatrix::~SuperLUMatrix  ----- 


Int
SuperLUMatrix::m (  ) const	
{
	return ptrData->A.nrow;
} 		// -----  end of method SuperLUMatrix::m  ----- 


Int
SuperLUMatrix::n (  ) const	
{
	return ptrData->A.ncol;
} 		// -----  end of method SuperLUMatrix::n  ----- 

void SuperLUMatrix::DistSparseMatrixToSuperMatrixNRloc( DistSparseMatrix<Scalar>& sparseA ){
#ifndef _RELEASE_
	PushCallStack( "SuperLUMatrix::DistSparseMatrixToSuperMatrixNRloc" );
#endif
	if( ptrData->isSuperMatrixAllocated == true ){
		throw std::logic_error( "SuperMatrix is already allocated." );
	}
	gridinfo_t* grid = ptrData->grid;

	Int mpirank = grid->iam;
	Int mpisize = grid->nprow * grid->npcol;

	Int numRowLocal = sparseA.colptrLocal.m() - 1;
	Int numRowLocalFirst = sparseA.size / mpisize;
	Int firstRow = mpirank * numRowLocalFirst;

  int_t *colindLocal, *rowptrLocal;
	doublecomplex *nzvalLocal;
	rowptrLocal = (int_t*)intMalloc_dist(numRowLocal+1);
	colindLocal = (int_t*)intMalloc_dist(sparseA.nnzLocal); 
	nzvalLocal  = (doublecomplex*)doublecomplexMalloc_dist(sparseA.nnzLocal);
  
	std::copy( sparseA.colptrLocal.Data(), sparseA.colptrLocal.Data() + sparseA.colptrLocal.m(),
			rowptrLocal );
	std::copy( sparseA.rowindLocal.Data(), sparseA.rowindLocal.Data() + sparseA.rowindLocal.m(),
			colindLocal );
	std::copy( sparseA.nzvalLocal.Data(), sparseA.nzvalLocal.Data() + sparseA.nzvalLocal.m(),
			(Complex*)nzvalLocal );

//	std::cout << "Processor " << mpirank << " rowptrLocal[end] = " << 
//		rowptrLocal[numRowLocal] << std::endl;


	// Important to adjust from FORTRAN convention (1 based) to C convention (0 based) indices
	for(Int i = 0; i < sparseA.rowindLocal.m(); i++){
		colindLocal[i]--;
	}

	for(Int i = 0; i < sparseA.colptrLocal.m(); i++){
		rowptrLocal[i]--;
	}

//	std::cout << "Processor " << mpirank << " colindLocal[end] = " << 
//		colindLocal[A.nnzLocal-1] << std::endl;

	// Construct the distributed matrix according to the SuperLU_DIST format
	zCreate_CompRowLoc_Matrix_dist(&ptrData->A, sparseA.size, sparseA.size, sparseA.nnzLocal, 
			numRowLocal, firstRow,
			nzvalLocal, colindLocal, rowptrLocal,
			SLU_NR_loc, SLU_Z, SLU_GE);

	ptrData->isSuperMatrixAllocated = true;

#ifndef _RELEASE_
	PopCallStack();
#endif

	return;

} 		// -----  end of method SuperLUMatrix::DistSparseMatrixToSuperMatrixNRloc ----- 


void
SuperLUMatrix::DestroyAOnly	(  )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::DestroyAOnly");
#endif
	if( ptrData->isSuperMatrixAllocated == false ){
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
			throw std::runtime_error( msg.str().c_str() );
	}
	ptrData->isSuperMatrixAllocated = false;
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::DestroyAOnly  ----- 

void
SuperLUMatrix::SymbolicFactorize	(  )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::SymbolicFactorize");
#endif
	if( ptrData->isScalePermstructAllocated ){
		throw std::logic_error( "ScalePermstruct is already allocated." );
	}
	if( ptrData->isLUstructAllocated){
		throw std::logic_error( "LUstruct is already allocated." );
	}
	if( ptrData->options.RowPerm != NOROWPERM ){
		throw std::logic_error( "For PEXSI there must be no row permutation." );
	}
	
	SuperMatrix&  A = ptrData->A;

	ScalePermstructInit(A.nrow, A.ncol, &ptrData->ScalePermstruct);
	LUstructInit(A.nrow, A.ncol, &ptrData->LUstruct);

	PStatInit(&ptrData->stat);
	pzsymbfact(&ptrData->options, &A, &ptrData->ScalePermstruct, ptrData->grid, 
			&ptrData->LUstruct, &ptrData->stat, &ptrData->info);
	PStatFree(&ptrData->stat);

	ptrData->isScalePermstructAllocated = true;
  ptrData->isLUstructAllocated        = true; 

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::SymbolicFactorize  ----- 


void
SuperLUMatrix::Distribute	(  )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::Distribute");
#endif
	if( ptrData->isScalePermstructAllocated == false ){
		throw std::logic_error( "ScalePermstruct has not been allocated by SymbolicFactorize." );
	}	
	if( ptrData->isLUstructAllocated == false ){
		throw std::logic_error( "LUstruct has not been allocated by SymbolicFactorize." );
	}	
	if( ptrData->isSuperMatrixAllocated == false ){
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
	float dist_mem_use = pzdistribute(SamePattern_SameRowPerm, ptrData->A.nrow, 
			&ptrData->A, &ptrData->ScalePermstruct, NULL, &ptrData->LUstruct, ptrData->grid);

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::Distribute  ----- 


void
SuperLUMatrix::NumericalFactorize	(  )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::NumericalFactorize");
#endif
	if( !ptrData->isLUstructAllocated ){
		throw std::logic_error( "LUstruct has not been allocated." );
	}
	// Estimate the 1-norm
	char norm[1]; *norm = '1';
	double anorm = pzlangs( norm, &ptrData->A, ptrData->grid );
  
	PStatInit(&ptrData->stat);
	pzgstrf(&ptrData->options, ptrData->A.nrow, ptrData->A.ncol, 
			anorm, &ptrData->LUstruct, ptrData->grid, &ptrData->stat, &ptrData->info); 
	PStatFree(&ptrData->stat);
	if( ptrData->info ){
		std::ostringstream msg;
		msg << "Numerical factorization error, info =  " << ptrData->info << std::endl;
		throw std::runtime_error( msg.str().c_str() );
	}
	
	// Prepare for Solve.
	ptrData->options.Fact = FACTORED;

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::NumericalFactorize  ----- 


void
SuperLUMatrix::ConvertNRlocToNC	( SuperLUMatrix& AGlobal )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::ConvertNRlocToNC");
#endif
  if( !ptrData->isSuperMatrixAllocated ){
		throw std::runtime_error( "The local SuperMatrix has not been allocated." );
	}
  if( AGlobal.ptrData->isSuperMatrixAllocated ){
		throw std::runtime_error( "The global SuperMatrix has been allocated." );
	}
	// TODO make sure the two grids are the same
  
	// TODO real arithmetic
	const Int NEED_VALUE = 1;
	pzCompRow_loc_to_CompCol_global(NEED_VALUE, &ptrData->A, ptrData->grid, 
			&AGlobal.ptrData->A);

	AGlobal.ptrData->isSuperMatrixAllocated = true;

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::ConvertNRlocToNC  ----- 
void
SuperLUMatrix::MultiplyGlobalMultiVector	( NumMat<Scalar>& xGlobal, NumMat<Scalar>& bGlobal )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::MultiplyGlobalMultiVector");
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
		throw std::runtime_error( msg.str().c_str() );
	}	
  zFillRHS_dist(trans, nrhs, (doublecomplex*)xGlobal.Data(), m, 
			&ptrData->A, (doublecomplex*) bGlobal.Data(), m);
#ifndef _RELEASE_
	PopCallStack();
#endif
 
	return ;
} 		// -----  end of method SuperLUMatrix::MultiplyGlobalMultiVector  ----- 


void
SuperLUMatrix::DistributeGlobalMultiVector	( NumMat<Scalar>& xGlobal, NumMat<Scalar>& xLocal )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::DistributeGlobalMultiVector");
#endif
	SuperMatrix& A = ptrData->A;
  if( ptrData->A.Stype != SLU_NR_loc ){
		std::ostringstream msg;
		msg << "DistributeGlobalMultiVector requires SLU_NR_loc matrix with type " << SLU_NR_loc << std::endl
			<< "The matrix is of type " << ptrData->A.Stype << std::endl;
		throw std::runtime_error( msg.str().c_str() );
	}	

	NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;
	
	Int numRowLocal = Astore->m_loc;
	Int firstRow    = Astore->fst_row;
	
	xLocal.Resize( numRowLocal, xGlobal.n() );
	std::copy( xGlobal.Data()+firstRow, xGlobal.Data()+firstRow+numRowLocal,
			xLocal.Data() );

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::DistributeGlobalMultiVector  ----- 


void
SuperLUMatrix::SolveDistMultiVector	( NumMat<Scalar>& bLocal, DblNumVec& berr )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::SolveDistMultiVector");
#endif
	Int nrhs = bLocal.n();
	NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;
	Int numRowLocal = Astore->m_loc;
	Int firstRow    = Astore->fst_row;
	
	berr.Resize( nrhs );

	// TODO Real arithmetic

	PStatInit(&ptrData->stat);
	pzgssvx(&ptrData->options, &ptrData->A, &ptrData->ScalePermstruct, 
			(doublecomplex*)bLocal.Data(), numRowLocal, nrhs, ptrData->grid,
			&ptrData->LUstruct, &ptrData->SOLVEstruct, berr.Data(), 
			&ptrData->stat, &ptrData->info);
	PStatFree(&ptrData->stat); 

	if( ptrData->info ){
		std::ostringstream msg;
		msg << "Numerical solve error, info =  " << ptrData->info << std::endl;
		throw std::runtime_error( msg.str().c_str() );
	}


#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::SolveDistMultiVector  ----- 


void
SuperLUMatrix::CheckErrorDistMultiVector	( NumMat<Scalar>& xLocal, NumMat<Scalar>& xTrueLocal )
{
#ifndef _RELEASE_
	PushCallStack("SuperLUMatrix::CheckErrorDistMultiVector");
#endif
	Int nrhs = xLocal.n();
	NRformat_loc *Astore = (NRformat_loc *) ptrData->A.Store;
	Int numRowLocal = Astore->m_loc;

	pzinf_norm_error(ptrData->grid->iam, numRowLocal, nrhs, 
			(doublecomplex*)xLocal.Data(), numRowLocal, 
			(doublecomplex*)xTrueLocal.Data(), numRowLocal, ptrData->grid);

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method SuperLUMatrix::CheckErrorDistMultiVector  ----- 

} // namespace PEXSI