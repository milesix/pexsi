# Install script for directory: /home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/mx/prj/pexsi_install/v2.1_gnu")
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
  set(CMAKE_OBJDUMP "/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0/SRC/libsuperlu_dist.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_FCnames.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/dcomplex.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/machines.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/psymbfact.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_defs.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_enum_consts.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/supermatrix.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/util_dist.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/colamd.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/gpu_api_utils.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/gpu_wrapper.h"
    "/home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0/SRC/superlu_dist_config.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_FortranCInterface.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_ddefs.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/dlustruct_gpu.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_sdefs.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/slustruct_gpu.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/superlu_zdefs.h"
    "/home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0/SRC/zlustruct_gpu.h"
    )
endif()

