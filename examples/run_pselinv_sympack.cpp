/// @file ex32.cpp
/// @brief Test to use the NGCHOL library in PEXSI.
/// @author Lin Lin
/// @date 2014-07-04

#include <mpi.h>

#include <complex>
#include <string>
#include <sstream>

// Libraries from NGCHOL BEGIN
#include  "ppexsi.hpp"

#include "sympack.hpp"
#include "sympack/SupernodalMatrix.hpp"
#include "pexsi/sympack_interf.hpp"

//#define _MYCOMPLEX_

typedef double ISCALAR;
#ifdef _MYCOMPLEX_
typedef std::complex<double> SCALAR;
#else
typedef double SCALAR;
#endif




using namespace PEXSI;
using namespace std;

void Usage(){
  std::cout << "Usage" << std::endl << "run_pselinv_sympack -H <symmetric Hfile> -S [symmetric Sfile] -inf [H/S file format: CSC,HARWELL_BOEING] -r [nprow] -c [npcol] -rshift [real shift] -ishift [imaginary shift] -ToDist [doToDist] -Diag [doDiag]" << std::endl;
}


int main(int argc, char **argv) 
{
  int mpisize;
  int mpirank;

  MPI_Init(&argc,&argv);

  //Create a communicator with npcol*nprow processors
  MPI_Comm world_comm;
  MPI_Comm_dup(MPI_COMM_WORLD, &world_comm);

  MPI_Comm_size(world_comm, &mpisize);
  MPI_Comm_rank(world_comm, &mpirank);

#if defined(SPROFILE) || defined(PMPI)
  SYMPACK::symPACK_set_main_args(argc,argv);
  //  SYMPACK_SPROFILE_INIT(argc, argv);
#endif

  stringstream  ss;
  ss << "logTest" << mpirank;
  statusOFS.open( ss.str().c_str() );

  if(argc<3){
    if( mpirank == 0 ) {
      Usage();
    }
  }

  try{

    //Temporarily required
    //MPI_Comm_size(world_comm, &SYMPACK::np);
    //MPI_Comm_rank(world_comm, &SYMPACK::iam);

    std::map<std::string,std::string> options;
    OptionsCreate(argc, argv, options);

    Int nprow = 1, npcol = mpisize;

    if( options.find("-r") != options.end() ){
      if( options.find("-c") != options.end() ){
        nprow= atoi(options["-r"].c_str());
        npcol= atoi(options["-c"].c_str());
      }
      else{
        throw std::runtime_error( "When using -r option, -c also needs to be provided." );
      }
    }
    else if( options.find("-c") != options.end() ){
      if( options.find("-r") != options.end() ){
        nprow= atoi(options["-r"].c_str());
        npcol= atoi(options["-c"].c_str());
      }
      else{
        throw std::runtime_error( "When using -c option, -r also needs to be provided." );
      }
    }
    if(nprow*npcol > mpisize){
      throw std::runtime_error("The number of used processors can't be larger than the total number of available processors." );
    } 

    std::string Hfile;
    if( options.find("-H") != options.end() ){ 
      Hfile = options["-H"];
    }
    else{
      throw std::logic_error("Hfile must be provided.");
    }

    std::string format("CSC");
    if( options.find("-inf") != options.end() ){ 
      format = options["-inf"];
    }
    int doToDist = 0;
    if( options.find("-ToDist") != options.end() ){ 
      doToDist= atoi(options["-ToDist"].c_str());
    }

    int doDiag = 0;
    if( options.find("-Diag") != options.end() ){ 
      doDiag = atoi(options["-Diag"].c_str());
    }

    Int maxPipelineDepth = -1;
    if( options.find("-P") != options.end() ){ 
      maxPipelineDepth = atoi(options["-P"].c_str());
    }
    else{
      statusOFS << "-P option is not given. " 
        << "Do not limit SelInv pipelining depth." 
        << std::endl << std::endl;
    }

    Real rshift = 0.0, ishift = 0.0;
    if( options.find("-rshift") != options.end() ){ 
      rshift = atof(options["-rshift"].c_str());
    }
    if( options.find("-ishift") != options.end() ){ 
      ishift = atof(options["-ishift"].c_str());
    }


    std::string ColPerm;
    if( options.find("-colperm") != options.end() ){ 
      ColPerm = options["-colperm"];
    }
    else{
      statusOFS << "-colperm option is not given. " 
        << "Use MMD" 
        << std::endl << std::endl;
      ColPerm = "MMD";
    }



    //Initialize symPACK logfile
    std::stringstream suffix;
    suffix<<mpirank;
    SYMPACK::logfileptr = new SYMPACK::LogFile("status",suffix.str().c_str());
    SYMPACK::logfileptr->OFS()<<"********* LOGFILE OF P"<<mpirank<<" *********"<<endl;
    SYMPACK::logfileptr->OFS()<<"**********************************"<<endl;


    mpisize = nprow*npcol;

    MPI_Comm workcomm;
    MPI_Comm_split(world_comm,mpirank<mpisize,mpirank,&workcomm);

    SYMPACK::symPACKOptions optionsFact;
    optionsFact.relax.SetMaxSize(200);
    optionsFact.factorization = SYMPACK::FANBOTH;
    optionsFact.decomposition = SYMPACK::LDL;
    optionsFact.ordering = SYMPACK::MMD;
    optionsFact.scheduler = SYMPACK::DL;
    optionsFact.mappingTypeStr = "ROW2D";
    optionsFact.load_balance_str = "SUBCUBE-FO";
    optionsFact.MPIcomm = workcomm;

    if(ColPerm=="MMD"){
      optionsFact.ordering = SYMPACK::MMD;
    }
    else if(ColPerm=="AMD"){
      optionsFact.ordering = SYMPACK::AMD;
    }
    else if(ColPerm=="METIS"){
      optionsFact.ordering = SYMPACK::METIS;
    }
    else if(ColPerm=="SCOTCH"){
      optionsFact.ordering = SYMPACK::SCOTCH;
    }
    else if(ColPerm=="PARMETIS"){
      optionsFact.ordering = SYMPACK::PARMETIS;
    }
    else if(ColPerm=="PTSCOTCH"){
      optionsFact.ordering = SYMPACK::PTSCOTCH;
    }

    //Initialize UPCXX for symPACK
    upcxx::init(&argc, &argv);

    if(mpirank<mpisize){
      Real timeSta, timeEnd;
      Real timeTotalFactorizationSta, timeTotalFactorizationEnd;
      Real timeTotalSelInvSta, timeTotalSelInvEnd;
      //MPI_Comm_size(workcomm,&SYMPACK::np);
      //MPI_Comm_rank(workcomm,&SYMPACK::iam);

#ifdef _MYCOMPLEX_
      SCALAR zshift = SCALAR(rshift, ishift);
#else
      SCALAR zshift = SCALAR(rshift);
#endif

      SYMPACK::DistSparseMatrix<SCALAR> AMat(workcomm);
      SYMPACK::SupernodalMatrix<SCALAR>*  symPACKMat = NULL;
      {
        SYMPACK::DistSparseMatrix<ISCALAR> HMat(workcomm);
        SYMPACK::ReadMatrix<ISCALAR,ISCALAR>(Hfile, format, HMat);

        //Build AMat
        SYMPACK::DistSparseMatrix<ISCALAR> SMat(workcomm);
        if( options.find("-S") != options.end() ){ 
          std::string Sfile;
          Sfile = options["-S"];
          SYMPACK::ReadMatrix<ISCALAR,ISCALAR>(Sfile, format, SMat);
        }
        else{

          statusOFS << "-S option is not given. " 
            << "Treat the overlap matrix as an identity matrix." 
            << std::endl << std::endl;
          SMat.size=0;
        }



        const SYMPACK::DistSparseMatrixGraph & Hgraph = HMat.GetLocalGraph();
        // Get the diagonal indices for H and save it n diagIdxLocal_
        std::vector<Int>  diagIdxLocal;
        { 

          Int numColLocal      = Hgraph.LocalVertexCount();
          Int firstCol         = Hgraph.LocalFirstVertex();

          diagIdxLocal.clear();
          diagIdxLocal.reserve( HMat.size );
          for( SYMPACK::Idx j = 0; j < numColLocal; j++ ){
            SYMPACK::Idx jcol = firstCol + j + 1;
            for( SYMPACK::Ptr i = Hgraph.colptr[j]-1; 
                i < Hgraph.colptr[j+1]-1; i++ ){
              SYMPACK::Idx irow = Hgraph.rowind[i];
              if( irow == jcol ){
                diagIdxLocal.push_back( i );
              }
            }
          } // for (j)
        }

        GetTime( timeSta );

        AMat.size          = HMat.size;
        AMat.nnz           = HMat.nnz;
        AMat.SetLocalGraph(HMat.GetLocalGraph());
        AMat.nzvalLocal.resize( HMat.nzvalLocal.size() );

        SCALAR *ptr0 = AMat.nzvalLocal.data();
        Real *ptr1 = HMat.nzvalLocal.data();
        Real *ptr2 = SMat.nzvalLocal.data();


        if( SMat.size != 0 ){
          // S is not an identity matrix
          for( Int i = 0; i < HMat.nzvalLocal.size(); i++ ){
            AMat.nzvalLocal[i] = HMat.nzvalLocal[i] - zshift * SMat.nzvalLocal[i];
          }
        }
        else{
          // S is an identity matrix
          for( Int i = 0; i < Hgraph.nnz; i++ ){
            AMat.nzvalLocal[i] = HMat.nzvalLocal[i];
          }

          for( Int i = 0; i < diagIdxLocal.size(); i++ ){
            AMat.nzvalLocal[ diagIdxLocal[i] ] -= zshift;
          }
        } // if (SMat.size != 0 )

        if( mpirank == 0 ){
          cout << "nonzero in A (DistSparseMatrix format) = " << AMat.nnz << endl;
        }


        GetTime( timeEnd );
        if( mpirank == 0 )
          cout << "Time for constructing the matrix A is " << timeEnd - timeSta << endl;


        GetTime( timeTotalFactorizationSta );
        GetTime( timeSta );
        symPACKMat = new SYMPACK::SupernodalMatrix<SCALAR>();
        //      optionsFact.commEnv = new SYMPACK::CommEnvironment(workcomm);
        symPACKMat->Init(optionsFact);
        symPACKMat->SymbolicFactorization(AMat);
        GetTime( timeEnd );
        if( mpirank == 0 )
          cout << "Time for symbolic factorization is " << timeEnd - timeSta << " sec" << endl; 
        GetTime( timeSta );
        symPACKMat->DistributeMatrix(AMat);
        GetTime( timeEnd );
        if( mpirank == 0 )
          cout << "Time for distribution is " << timeEnd - timeSta << " sec" << endl; 
        GetTime( timeSta );
        symPACKMat->Factorize();
        GetTime( timeEnd );

        if( mpirank == 0 )
          cout << "Time for factorization is " << timeEnd - timeSta << " sec" << endl; 

        GetTime( timeTotalFactorizationEnd );
        if( mpirank == 0 )
          cout << "Time for total factorization is " << timeTotalFactorizationEnd - timeTotalFactorizationSta<< " sec" << endl; 

      }

      GetTime( timeTotalSelInvSta );
      GridType *gPtr = new GridType( workcomm, nprow, npcol );
      SuperNodeType *superPtr = new SuperNodeType();

      // deprecated options
      SuperLUOptions luOpt;
      luOpt.ColPerm = "METIS_AT_PLUS_A";
      luOpt.numProcSymbFact = 1;
      luOpt.Symmetric = 1;

      PSelInvOptions selInvOpt;
      selInvOpt.maxPipelineDepth = -1;

      GetTime( timeSta );
      symPACKMatrixToSuperNode( *symPACKMat, *superPtr );


      PMatrix<SCALAR> * pMat = PMatrix<SCALAR>::Create(gPtr,superPtr, &selInvOpt, &luOpt);
      symPACKMatrixToPMatrix( *symPACKMat, *pMat );
      GetTime( timeEnd );

      if( mpirank == 0 )
        cout << "Time for converting symPACK matrix to PMatrix is " << timeEnd  - timeSta << endl;

      delete symPACKMat;
      //      delete optionsFact.commEnv;

      MPI_Barrier(workcomm);

      // Preparation for the selected inversion
      GetTime( timeSta );
      pMat->ConstructCommunicationPattern();
      GetTime( timeEnd );
      if( mpirank == 0 )
        cout << "Time for constructing the communication pattern is " << timeEnd  - timeSta << endl;

      GetTime( timeSta );
      pMat->PreSelInv();
      GetTime( timeEnd );
      if( mpirank == 0 )
        cout << "Time for pre-selected inversion is " << timeEnd  - timeSta << endl;
      statusOFS << "Time for pre-selected inversion is " << timeEnd  - timeSta << endl;

      GetTime( timeSta );
      pMat->SelInv();
      GetTime( timeEnd );
      GetTime( timeTotalSelInvEnd );
      if( mpirank == 0 )
        cout << "Time for numerical selected inversion is " << timeEnd  - timeSta << endl;
      statusOFS << "Time for numerical selected inversion is " << timeEnd  - timeSta << endl;


      if( mpirank == 0 )
        cout << "Time for total selected inversion is " << timeTotalSelInvEnd  - timeTotalSelInvSta << endl;


      if( doDiag ){
        NumVec<SCALAR> diag;

        GetTime( timeSta );
        pMat->GetDiagonal( diag );
        GetTime( timeEnd );

        if( mpirank == 0 )
          cout << "Time for getting the diagonal is " << timeEnd  - timeSta << endl;


        if( mpirank == 0 ){
          statusOFS << std::endl << "Diagonal (pipeline) of inverse in natural order: " << std::endl << diag << std::endl;
          ofstream ofs("diag");
          if( !ofs.good() ) 
            ErrorHandling("file cannot be opened.");
          serialize( diag, ofs, NO_MASK );
          ofs.close();
        }
      }

      if(doToDist){
        //Expand AMat to unsymmetric storage
        GetTime( timeSta );
        AMat.ExpandSymmetric();
        AMat.SortGraph();
        AMat.GetLocalGraph().SetBaseval(1);
        GetTime( timeEnd );

        if( mpirank == 0 )
          cout << "Matrix A has been expanded in " << timeEnd  - timeSta << endl;

        DistSparseMatrix<SCALAR> AMatPattern;
        AMatPattern.size = AMat.size;
        AMatPattern.nnz = AMat.nnz;
        AMatPattern.colptrLocal.Resize(AMat.GetLocalGraph().colptr.size());
        std::copy(AMat.GetLocalGraph().colptr.begin(), AMat.GetLocalGraph().colptr.end(), &AMatPattern.colptrLocal[0]);
        AMatPattern.rowindLocal.Resize(AMat.GetLocalGraph().rowind.size());
        std::copy(AMat.GetLocalGraph().rowind.begin(), AMat.GetLocalGraph().rowind.end(), &AMatPattern.rowindLocal[0]);
        AMatPattern.nnzLocal = AMat.GetLocalGraph().rowind.size();

        // Convert to DistSparseMatrix in the 2nd format and get the diagonal
        DistSparseMatrix<SCALAR> Ainv;

        GetTime( timeSta );
        pMat->PMatrixToDistSparseMatrix( AMatPattern, Ainv );
        GetTime( timeEnd );


        if( mpirank == 0 )
          cout << "Time for converting PMatrix to DistSparseMatrix (2nd format) is " << timeEnd  - timeSta << endl;

        SCALAR traceLocal = ZERO<SCALAR>();
        traceLocal = blas::Dotu( Ainv.nnzLocal, Ainv.nzvalLocal.Data(), 1,
            AMat.nzvalLocal.data(), 1 );


        SCALAR trace = ZERO<SCALAR>();
        mpi::Allreduce( &traceLocal, &trace, 1, MPI_SUM, workcomm );

        if( mpirank == 0 ){
          cout << "A.size = "  << AMat.size << endl;
          cout << std::endl << "Tr[Ainv * AMat] = " <<  trace << std::endl;
          statusOFS << std::endl << "Tr[Ainv * AMat] = " << std::endl << trace << std::endl;

          cout << std::endl << "|N - Tr[Ainv * AMat]| = " << std::abs( SCALAR(AMat.size) - trace ) << std::endl;
          statusOFS << std::endl << "|N - Tr[Ainv * AMat]| = " << std::abs( SCALAR(AMat.size) - trace ) << std::endl;
        }
      }



      delete pMat;


      delete superPtr;
      delete gPtr;
    }
    delete SYMPACK::logfileptr;
    statusOFS.close();

  }
  catch( std::exception& e )
  {
    std::cerr << "Processor " << mpirank << " caught exception with message: "
      << e.what() << std::endl;
  }



  //This will also finalize MPI
  upcxx::finalize();

  return 0;
}
