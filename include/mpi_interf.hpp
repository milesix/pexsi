#ifndef _MPI_INTERF_HPP_
#define _MPI_INTERF_HPP_

#include  "environment_impl.hpp"

namespace PEXSI{

namespace mpi{


// *********************************************************************
// Allgatherv
// *********************************************************************
void Allgatherv( 
		std::vector<Int>& localVec, 
		std::vector<Int>& allVec,
		MPI_Comm          comm );


// *********************************************************************
// Send / Recv for stringstream 
// *********************************************************************

void Send( const std::stringstream& sstm, Int dest, Int tagSize, Int tagContent, 
		MPI_Comm comm );

void Recv ( std::stringstream& sstm, Int src, Int tagSize, Int tagContent, 
		MPI_Comm comm, MPI_Status& statSize, MPI_Status& statContent );

} // namespace mpi


} // namespace PEXSI

#endif // _MPI_INTERF_HPP_

