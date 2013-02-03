!> @file ex20.f90
!> @brief Test the FORTRAN interface for PEXSI.
!> @author Lin Lin
!> @date 2013-01-31
program ex20
implicit none
include 'mpif.h'

integer :: nrows, nnz, nnzLocal, numColLocal
integer, allocatable, dimension(:) ::  colptrLocal, rowindLocal
double precision, allocatable, dimension(:) :: &
	HnzvalLocal, SnzvalLocal, DMnzvalLocal, EDMnzvalLocal, &
	FDMnzvalLocal
integer :: numPole
double precision :: temperature, numElectronExact, gap, deltaE
double precision :: mu0, muMin, muMax
integer:: muMaxIter
double precision :: poleTolerance, numElectronTolerance
integer:: npPerPole, nprow, npcol
integer :: mpirank, mpisize, ierr
double precision:: timeSta, timeEnd
character*32 :: Hfile, Sfile

call mpi_init( ierr )
call mpi_comm_rank( MPI_COMM_WORLD, mpirank, ierr )
call mpi_comm_size( MPI_COMM_WORLD, mpisize, ierr )


! Data is for the DNA matrix.
temperature      = 3000.0d0
numElectronExact = 2442.0d0
numPole          = 80
gap              = 0.0d0
deltaE           = 20.0d0
! Initial guess of chemical potential
mu0              = -0.63d0
! The information of muMin/muMax can be obtained from previous iterations in the
! case with SCF iterations.
muMin            = -0.7d0
muMax            = -0.6d0
! muMaxIter should be 1 or 2 later when combined with SCF.
muMaxIter        = 10
! Do not compute a pole if the corresponding weight is < poleTolerance.
poleTolerance    = 1d-12
! Stop mu-iteration if numElectronTolerance is < numElectronTolerance.
numElectronTolerance = 1d-2
! Number of processors used for each pole. At the moment use mpisize.
! Later can be changed to 
npPerPole        = mpisize
Hfile            = "H.matrix"
Sfile            = "S.matrix"

! Read and compute the size/local size of the arrays 
! The conversion of the string to fit the C format is important.
call f_read_distsparsematrix_formatted_head( &
	trim(Hfile)//char(0),&
	nrows,&
	nnz,&
	nnzLocal,&
	numColLocal,&
	MPI_COMM_WORLD )

if( mpirank .eq. 0 ) then
	write(*,*) "Matrix size (local data on proc 0):" 
	write(*,*) "size = ", nrows
	write(*,*) "nnz  = ", nnz
	write(*,*) "nnzLocal = ", nnzLocal
	write(*,*) "numColLocal = ", numColLocal
endif



! Allocate memory
allocate( colptrLocal( numColLocal + 1 ) )
allocate( rowindLocal( nnzLocal ) )
allocate( HnzvalLocal( nnzLocal ) )
allocate( SnzvalLocal( nnzLocal ) ) 
allocate( DMnzvalLocal( nnzLocal ) ) 
allocate( EDMnzvalLocal( nnzLocal ) ) 
allocate( FDMnzvalLocal( nnzLocal ) ) 

timeSta = mpi_wtime()

call f_read_distsparsematrix_formatted (&
	trim(Hfile)//char(0),&
	nrows,&
	nnz,&
	nnzLocal,&
	numColLocal,&
	colptrLocal,&
	rowindLocal,&
	HnzvalLocal,&
	MPI_COMM_WORLD )

call f_read_distsparsematrix_formatted (&
	trim(Sfile)//char(0),&
	nrows,&
	nnz,&
	nnzLocal,&
	numColLocal,&
	colptrLocal,&
	rowindLocal,&
	SnzvalLocal,&
	MPI_COMM_WORLD )

timeEnd = mpi_wtime()

if( mpirank == 0 ) then
  write(*,*) "Time for reading H/S matrices is ", &
		timeEnd - timeSta, " [s]"
endif

call f_ppexsi_interface( &
	nrows,&
	nnz,&
	nnzLocal,&
	numColLocal,&
	colptrLocal,&
  rowindLocal,&
	HnzvalLocal,&
	SnzvalLocal,&
	DMnzvalLocal,&
	EDMnzvalLocal,&
	FDMnzvalLocal,&
	numPole,&
	temperature,&
	numElectronExact,&
	gap,&
	deltaE,&
	mu0,&
	muMin,&
  muMax,&
	muMaxIter,&
	poleTolerance,&
	numElectronTolerance,&
	MPI_COMM_WORLD,&
	npPerPole )

deallocate( colptrLocal )
deallocate( rowindLocal )
deallocate( HnzvalLocal )
deallocate( SnzvalLocal )
deallocate( DMnzvalLocal )
deallocate( EDMnzvalLocal )
deallocate( FDMnzvalLocal )


call mpi_finalize( ierr )

end program ex20

