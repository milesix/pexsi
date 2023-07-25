# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.6)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.20)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget PEXSI superlu_dist parmetis metis)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target PEXSI
add_library(PEXSI STATIC IMPORTED)

set_target_properties(PEXSI PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "Add_"
  INTERFACE_COMPILE_FEATURES "cxx_std_11"
  INTERFACE_INCLUDE_DIRECTORIES "/home/mx/prj/pexsi/include;/home/mx/prj/pexsi/build_gnu/include"
  INTERFACE_LINK_LIBRARIES "MPI::MPI_C;MPI::MPI_CXX;MPI::MPI_Fortran;superlu_dist;parmetis;LAPACK::LAPACK"
)

# Create imported target superlu_dist
add_library(superlu_dist STATIC IMPORTED)

set_target_properties(superlu_dist PROPERTIES
  INTERFACE_LINK_LIBRARIES "/usr/lib/x86_64-linux-gnu/libblas.so.3;m;MPI::MPI_CXX;MPI::MPI_C"
)

# Create imported target parmetis
add_library(parmetis STATIC IMPORTED)

set_target_properties(parmetis PROPERTIES
  INTERFACE_LINK_LIBRARIES "metis"
)

# Create imported target metis
add_library(metis STATIC IMPORTED)

set_target_properties(metis PROPERTIES
  INTERFACE_LINK_LIBRARIES "m"
)

# Import target "PEXSI" for configuration "Release"
set_property(TARGET PEXSI APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PEXSI PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX;Fortran"
  IMPORTED_LOCATION_RELEASE "/home/mx/prj/pexsi/build_gnu/src/libpexsi.a"
  )

# Import target "superlu_dist" for configuration "Release"
set_property(TARGET superlu_dist APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(superlu_dist PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "/home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0/SRC/libsuperlu_dist.a"
  )

# Import target "parmetis" for configuration "Release"
set_property(TARGET parmetis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(parmetis PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "/home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/libparmetis/libparmetis.a"
  )

# Import target "metis" for configuration "Release"
set_property(TARGET metis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metis PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "/home/mx/prj/pexsi/build_gnu/libmetis/libmetis/libmetis.a"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
