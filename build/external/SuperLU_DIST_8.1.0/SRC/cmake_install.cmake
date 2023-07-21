# Install script for directory: /ws/pexsi/external/SuperLU_DIST_8.1.0/SRC

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/ws/pexsi/build/external/SuperLU_DIST_8.1.0/SRC/libsuperlu_dist.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_FCnames.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/dcomplex.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/machines.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/psymbfact.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_defs.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_enum_consts.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/supermatrix.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/util_dist.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/colamd.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/gpu_api_utils.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/gpu_wrapper.h"
    "/ws/pexsi/build/external/SuperLU_DIST_8.1.0/SRC/superlu_dist_config.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_FortranCInterface.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_ddefs.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/dlustruct_gpu.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_sdefs.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/slustruct_gpu.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_zdefs.h"
    "/ws/pexsi/external/SuperLU_DIST_8.1.0/SRC/zlustruct_gpu.h"
    )
endif()

