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
/// @file mpi_interf.cpp
/// @brief Interface with MPI to facilitate communication.
/// @date 2012-11-03
#include "pexsi/mpi_interf.hpp"

#ifdef USE_TAU
  #include "pexsi/TAU.h"
#elif defined (PROFILE) || defined(PMPI)
  #define TAU
  #include "pexsi/timer.h"
#endif


namespace PEXSI{

// *********************************************************************
// Constants
// *********************************************************************

namespace mpi{

// *********************************************************************
// Gather
// *********************************************************************

void
Gatherv ( 
		std::vector<Int>& localVec, 
		std::vector<Int>& allVec,
    Int root,
		MPI_Comm          comm )
{
#ifndef _RELEASE_
  PushCallStack("mpi::Gatherv");
#endif
  Int mpirank, mpisize;
  MPI_Comm_rank( comm, &mpirank );
  MPI_Comm_size( comm, &mpisize );

  Int localSize = localVec.size();
  std::vector<Int>  localSizeVec( mpisize );
  MPI_Gather( &localSize, 1, MPI_INT, &localSizeVec[0], 1, MPI_INT,root, comm );

  if(mpirank==root){
    std::vector<Int>  localSizeDispls( mpisize );
    localSizeDispls[0] = 0;
    for( Int ip = 1; ip < mpisize; ip++ ){
      localSizeDispls[ip] = localSizeDispls[ip-1] + localSizeVec[ip-1];
    }
    Int totalSize = localSizeDispls[mpisize-1] + localSizeVec[mpisize-1];

    allVec.clear();
    allVec.resize( totalSize );

    MPI_Gatherv( &localVec[0], localSize, MPI_INT, &allVec[0], 
        &localSizeVec[0], &localSizeDispls[0], MPI_INT, root, comm	);
  }
  else{
    MPI_Gatherv( &localVec[0], localSize, MPI_INT, NULL, 
        NULL, NULL, MPI_INT, root, comm	);
  }
#ifndef _RELEASE_
  PopCallStack();
#endif

  return ;
}		// -----  end of function Gatherv  ----- 



void
Gatherv ( 
		std::vector<Int>& localVec, 
		std::vector<Int>& allVec,
		std::vector<Int>& sizes,
		std::vector<Int>& displs,
    Int root,
		MPI_Comm          comm )
{
#ifndef _RELEASE_
  PushCallStack("mpi::Gatherv");
#endif
  Int mpirank, mpisize;
  MPI_Comm_rank( comm, &mpirank );
  MPI_Comm_size( comm, &mpisize );

  Int localSize = localVec.size();

  if(mpirank==root){
    std::vector<Int> & localSizeVec = sizes;
    localSizeVec.resize( mpisize );
    MPI_Gather( &localSize, 1, MPI_INT, &localSizeVec[0], 1, MPI_INT,root, comm );
    std::vector<Int> &  localSizeDispls = displs;
    localSizeDispls.resize( mpisize );
    localSizeDispls[0] = 0;
    for( Int ip = 1; ip < mpisize; ip++ ){
      localSizeDispls[ip] = localSizeDispls[ip-1] + localSizeVec[ip-1];
    }
    Int totalSize = localSizeDispls[mpisize-1] + localSizeVec[mpisize-1];

    allVec.clear();
    allVec.resize( totalSize);

    MPI_Gatherv( &localVec[0], localSize, MPI_INT, &allVec[0], 
        &localSizeVec[0], &localSizeDispls[0], MPI_INT, root, comm	);
  }
  else{
    MPI_Gather( &localSize, 1, MPI_INT, NULL, 1, MPI_INT,root, comm );
    MPI_Gatherv( &localVec[0], localSize, MPI_INT, NULL, 
        NULL, NULL, MPI_INT, root, comm	);
  }
#ifndef _RELEASE_
  PopCallStack();
#endif

  return ;
}		// -----  end of function Gatherv  ----- 




void
Allgatherv ( 
		std::vector<Int>& localVec, 
		std::vector<Int>& allVec,
		MPI_Comm          comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Allgatherv");
#endif
	Int mpirank, mpisize;
	MPI_Comm_rank( comm, &mpirank );
	MPI_Comm_size( comm, &mpisize );

	Int localSize = localVec.size();
	std::vector<Int>  localSizeVec( mpisize );
	std::vector<Int>  localSizeDispls( mpisize );
	MPI_Allgather( &localSize, 1, MPI_INT, &localSizeVec[0], 1, MPI_INT, comm );
	localSizeDispls[0] = 0;
	for( Int ip = 1; ip < mpisize; ip++ ){
    localSizeDispls[ip] = localSizeDispls[ip-1] + localSizeVec[ip-1];
	}
	Int totalSize = localSizeDispls[mpisize-1] + localSizeVec[mpisize-1];

	allVec.clear();
	allVec.resize( totalSize );

	MPI_Allgatherv( &localVec[0], localSize, MPI_INT, &allVec[0], 
		 &localSizeVec[0], &localSizeDispls[0], MPI_INT, comm	);

#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Allgatherv  ----- 


// *********************************************************************
// Send / Recv
// *********************************************************************
void 
Send( std::stringstream& sstm, Int dest, Int tagSize, Int tagContent, 
		MPI_Comm comm ){
#ifndef _RELEASE_
	PushCallStack("mpi::Send");
#endif
	std::vector<char> sstr;
	sstr.resize( Size( sstm ) );
	Int sizeStm = sstr.size();
	sstm.read( &sstr[0], sizeStm );
	MPI_Send( &sizeStm, 1, MPI_INT,  dest, tagSize, comm );
	MPI_Send( (void*)&sstr[0], sizeStm, MPI_BYTE, dest, tagContent, comm );
#ifndef _RELEASE_
	PopCallStack();
#endif
	return; 
} // -----  end of function Send ----- 


void
Recv ( std::stringstream& sstm, Int src, Int tagSize, Int tagContent, 
		MPI_Comm comm, MPI_Status& statSize, MPI_Status& statContent )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Recv");
#endif
	std::vector<char> sstr;
	Int sizeStm;
	MPI_Recv( &sizeStm, 1, MPI_INT, src, tagSize, comm, &statSize );
	sstr.resize( sizeStm );
	MPI_Recv( (void*) &sstr[0], sizeStm, MPI_BYTE, src, tagContent, comm, &statContent );
	sstm.write( &sstr[0], sizeStm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Recv  ----- 

void
Recv ( std::stringstream& sstm, Int src, Int tagSize, Int tagContent, 
		MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Recv (MPI_STATUS_IGNORE)");
#endif
	std::vector<char> str;
	Int sizeStm;
	MPI_Recv( &sizeStm, 1, MPI_INT, src, tagSize, comm, MPI_STATUS_IGNORE );
	str.resize( sizeStm );
	MPI_Recv( (void*) &str[0], sizeStm, MPI_BYTE, src, tagContent, comm, MPI_STATUS_IGNORE );
	sstm.write( &str[0], sizeStm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Recv  ----- 


// *********************************************************************
// Wait
// *********************************************************************


void
Wait	( MPI_Request& req  )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Wait");
#endif
  MPI_Wait( &req, MPI_STATUS_IGNORE );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
} 		// -----  end of method Wait  ----- 

void
Waitall ( std::vector<MPI_Request>& reqs, std::vector<MPI_Status>& stats )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Waitall");
#endif
  if( reqs.size() != stats.size() ){
    throw std::runtime_error( "MPI_Request does not have the same as as MPI_Status." );
	}
	for( Int i = 0; i < reqs.size(); i++ ){
		MPI_Wait( &reqs[i], &stats[i] );
	}
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Waitall  ----- 

void
Waitall ( std::vector<MPI_Request>& reqs )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Waitall (MPI_STATUS_IGNORE)");
#endif
	for( Int i = 0; i < reqs.size(); i++ ){
		MPI_Wait( &reqs[i], MPI_STATUS_IGNORE );
	}
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Waitall  ----- 


// *********************************************************************
// Reduce
// *********************************************************************


void
Reduce ( Real* sendbuf, Real* recvbuf, Int count, MPI_Op op, Int root, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Reduce");
#endif
	MPI_Reduce( sendbuf,  recvbuf, count, MPI_DOUBLE, op, root, comm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Reduce  ----- 

void
Reduce ( Complex* sendbuf, Complex* recvbuf, Int count, MPI_Op op, Int root, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Reduce");
#endif
	MPI_Reduce( (Real*)sendbuf,  (Real*)recvbuf, 2 * count, MPI_DOUBLE, op, root, comm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Reduce  ----- 



#ifdef _USE_MPI3_
void
Ireduce ( Real* sendbuf, Real* recvbuf, Int count, MPI_Op op, Int root, MPI_Comm comm, MPI_Request & request )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Ireduce");
#endif
	MPI_Ireduce( sendbuf,  recvbuf, count, MPI_DOUBLE, op, root, comm,&request );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Reduce  ----- 

void
Ireduce ( Complex* sendbuf, Complex* recvbuf, Int count, MPI_Op op, Int root, MPI_Comm comm , MPI_Request & request)
{
#ifndef _RELEASE_
	PushCallStack("mpi::Ireduce");
#endif
	MPI_Ireduce( (Real*)sendbuf,  (Real*)recvbuf, 2 * count, MPI_DOUBLE, op, root, comm ,&request);
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Reduce  ----- 
#endif





void
Allreduce ( Int* sendbuf, Int* recvbuf, Int count, MPI_Op op, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Allreduce");
#endif
	MPI_Allreduce( sendbuf,  recvbuf, count, MPI_INT, 
			op, comm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Allreduce  ----- 


void
Allreduce ( Real* sendbuf, Real* recvbuf, Int count, MPI_Op op, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Allreduce");
#endif
	MPI_Allreduce( sendbuf,  recvbuf, count, MPI_DOUBLE, 
			op, comm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Allreduce  ----- 


void
Allreduce ( Complex* sendbuf, Complex* recvbuf, Int count, MPI_Op op, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Allreduce");
#endif
	MPI_Allreduce( (Real*)sendbuf, (Real*) recvbuf, 2*count, MPI_DOUBLE, 
			op, comm );
#ifndef _RELEASE_
	PopCallStack();
#endif

	return ;
}		// -----  end of function Allreduce  ----- 


// *********************************************************************
// Alltoall
// *********************************************************************

void
Alltoallv ( Int *bufSend, Int *sizeSend, Int *displsSend, 
		Int *bufRecv, Int *sizeRecv, 
		Int *displsRecv, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Alltoallv");
#endif
  MPI_Alltoallv( bufSend, sizeSend, displsSend, MPI_INT,
		 bufRecv, sizeRecv, displsRecv, MPI_INT, comm );	
#ifndef _RELEASE_
	PopCallStack();
#endif
	return ;
}		// -----  end of function Alltoallv  ----- 


void
Alltoallv ( Real *bufSend, Int *sizeSend, Int *displsSend, 
		Real *bufRecv, Int *sizeRecv, 
		Int *displsRecv, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Alltoallv");
#endif
  MPI_Alltoallv( bufSend, sizeSend, displsSend, MPI_DOUBLE,
		 bufRecv, sizeRecv, displsRecv, MPI_DOUBLE, comm );	
#ifndef _RELEASE_
	PopCallStack();
#endif
	return ;
}		// -----  end of function Alltoallv  ----- 

void
Alltoallv ( Complex *bufSend, Int *sizeSend, Int *displsSend, 
		Complex *bufRecv, Int *sizeRecv, 
		Int *displsRecv, MPI_Comm comm )
{
#ifndef _RELEASE_
	PushCallStack("mpi::Alltoallv");
#endif
	Int mpisize; 
	MPI_Comm_size( comm, &mpisize );
	std::vector<Int> dblSizeSend( mpisize );
	std::vector<Int> dblDisplsSend( mpisize ); 
	std::vector<Int> dblSizeRecv( mpisize );
	std::vector<Int> dblDisplsRecv( mpisize );

	for( Int ip = 0; ip < mpisize; ip++ ){
    dblSizeSend[ip] = 2 * sizeSend[ip];
		dblSizeRecv[ip] = 2 * sizeRecv[ip];
		dblDisplsSend[ip] = 2 * displsSend[ip];
		dblDisplsRecv[ip] = 2 * displsRecv[ip];
	}

  MPI_Alltoallv( 
			(Real*)bufSend, &dblSizeSend[0], &dblDisplsSend[0], MPI_DOUBLE, 
			(Real*)bufRecv, &dblSizeRecv[0], &dblDisplsRecv[0], MPI_DOUBLE, comm );	

#ifndef _RELEASE_
	PopCallStack();
#endif
	return ;
}		// -----  end of function Alltoallv  ----- 


} // namespace mpi

} // namespace PEXSI
