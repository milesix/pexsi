#/usr/bin/bash
PROFILE        =   0
PROF_MPI       =   0
USE_TAU        =   0
USE_AUTO_TAU   =   0

# Different compiling and linking options.
#MODE           = debug
MODE	         = release
PLATFORM       = hopper_v0.5.5
SUFFIX         =$(MODE)_${PLATFORM}

ifdef USE_TAU
  ifeq ($(USE_TAU),1)
    SUFFIX=$(MODE)_tau_${PLATFORM}
  endif

  ifeq ($(PROFILE),1)
    SUFFIX=$(MODE)_tau_${PLATFORM}
  endif

  ifeq ($(PROF_MPI),1)
    SUFFIX=$(MODE)_tau_${PLATFORM}
  endif
endif


PEXSI_DIR     = /global/homes/l/linlin/project/pexsi
DSUPERLU_DIR  = /global/homes/l/linlin/software/SuperLU_DIST_3.3
PARMETIS_DIR  = /global/homes/l/linlin/software/parmetis-4.0.2/build/Linux-x86_64
SCOTCH_DIR    = /project/projectdirs/m1027/PEXSI/scotch_6.0.0
# SCOTCH_DIR    = /global/homes/l/linlin/software/scotch_6.0.0
# inclues

PEXSI_INCLUDE    = -I${PEXSI_DIR}/include 
DSUPERLU_INCLUDE = -I${DSUPERLU_DIR}/SRC
INCLUDES         = ${PEXSI_INCLUDE} ${DSUPERLU_INCLUDE} 

ifeq ($(USE_TAU),1)
  INCLUDES += -I${TAUROOTDIR}/include 
endif

# Libraries
#METIS_LIB        = /usr/common/acts/PARMETIS/3.1.1/craypgi-xe6_O/libmetis.a 
#PARMETISLIB	     = /usr/common/acts/PARMETIS/3.1.1/craypgi-xe6_O/libparmetis.a 
METIS_LIB        = ${PARMETIS_DIR}/libmetis/libmetis.a
PARMETISLIB	     = ${PARMETIS_DIR}/libparmetis/libparmetis.a
SCOTCH_LIB       = -L${SCOTCH_DIR}/lib -lptscotchparmetis -lptscotch -lptscotcherr -lscotch 
SELINV_LIB       = ${PEXSI_DIR}/external/SelInv/libselinv.a
# DSUPERLU_LIB     = ${DSUPERLU_DIR}/craypgi-xe6_O/lib/libsuperlu_dist_2.5.a
DSUPERLU_LIB     = ${DSUPERLU_DIR}/build/lib/libsuperlu_dist_3.3.a
PEXSI_LIB        = ${PEXSI_DIR}/src/libpexsi_${SUFFIX}.a

ifdef USE_TAU
  ifeq ($(USE_TAU),1)
TAU_LIB          = -L/opt/cray/mpt/5.5.2/gni/mpich2-pgi/119/lib -L/usr/common/acts/TAU/tau-2.22/craycnl/lib -lTauMpi-papi-mpi-pdt-pgi -lpthread -lrt -lmpichcxx -lmpich -lrt -L/usr/common/acts/TAU/tau-2.22/craycnl/lib -ltau-papi-mpi-pdt-pgi -R/opt/cray/papi/4.3.0.1/perf_events/no-cuda/lib -L/opt/cray/papi/4.3.0.1/perf_events/no-cuda/lib -lpapi -L/usr/common/acts/TAU/tau-2.22/craycnl/binutils-2.20/lib -L/usr/common/acts/TAU/tau-2.22/craycnl/binutils-2.20/lib64 -lbfd -liberty -lz -Wl,--export-dynamic -lrt -L/opt/pgi/12.5.0/linux86-64/12.5/bin/../lib -lstd -lC -lstdc++ -L/usr/common/acts/TAU/tau-2.22/craycnl/lib/static-papi-mpi-pdt-pgi
  endif
endif

LIBS_PARMETIS    = ${PEXSI_LIB} ${DSUPERLU_LIB} ${PARMETIS_LIB} ${METIS_LIB} ${TAU_LIB} ${IPM}   
LIBS_PTSCOTCH    = ${PEXSI_LIB} ${DSUPERLU_LIB} ${SCOTCH_LIB}  ${METIS_LIB} ${TAU_LIB} ${IPM} 
LIBS             = ${LIBS_PTSCOTCH}

CC           = cc
CXX          = CC
FC           = ftn
LOADER       = CC

ifdef USE_TAU
  ifeq ($(USE_TAU),1)
#  ifeq ($(USE_AUTO_TAU),1)
CC           = tau_cc.sh
CXX          = tau_cxx.sh
FC           = tau_f90.sh
LOADER       = tau_cxx.sh
#  endif
  endif
endif


AR           = ar 
ARFLAGS      = rvcu
# For System V based machine without ranlib, like Cray and SGI,
# use touch instead.
#RANLIB      = touch
RANLIB       = ranlib

CP           = cp
RM           = rm
RMFLAGS      = -f


ifeq ($(MODE), debug)
	COMMONDEFS   = -DDEBUG=0 -g -DAdd_ #-DUSE_REDUCE_L -DUSE_BCAST_UL -DPRINT_COMMUNICATOR_STAT #-DUSE_BCAST_UL #-DCOMPARE_LUPDATE #-DUSE_MPI_COLLECTIVES -DBLOCK_REDUCE
  CFLAGS       = -O0 -g -w ${INCLUDES} -DAdd_
  FFLAGS       = -O0 -g -w ${INCLUDES}
  CXXFLAGS     = -O0 -g -w ${INCLUDES} -DAdd_ #-DSANITY_CHECK -DSANITY_PRECISION=1e-5 #-DSELINV_TIMING -DSELINV_MEMORY -DNO_PARMETIS_FIX 
	CCDEFS       = ${COMMONDEFS}
	CPPDEFS      = ${COMMONDEFS}
  LOADOPTS     = ${LIBS}
  LOADOPTS_PARMETIS     = ${LIBS_PARMETIS}
  LOADOPTS_PTSCOTCH     = ${LIBS_PTSCOTCH}
  FLOADOPTS    = ${LIBS}
endif

ifeq ($(MODE), release)
	COMMONDEFS   = -DRELEASE -DDEBUG=0 -DAdd_ #-DUSE_REDUCE_L -DUSE_BCAST_UL #-DPRINT_COMMUNICATOR_STAT #-DUSE_BCAST_UL #-DCOMPARE_LUPDATE #-DUSE_MPI_COLLECTIVES 
  CFLAGS       = -fast -w -g ${INCLUDES} 
  FFLAGS       = -fast -w -g ${INCLUDES}
  CXXFLAGS     = -fast -w -g ${INCLUDES}  #-DNO_PARMETIS_FIX  #-DSANITY_CHECK -DSANITY_PRECISION=1e-5 
	CCDEFS       = ${COMMONDEFS}
	CPPDEFS      = ${COMMONDEFS}
  LOADOPTS     = ${LIBS}
  LOADOPTS_PARMETIS     = ${LIBS_PARMETIS}
  LOADOPTS_PTSCOTCH     = ${LIBS_PTSCOTCH}
  FLOADOPTS    = ${LIBS}
endif


# Generate auto-dependencies 
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CCDEFS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPDEFS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$