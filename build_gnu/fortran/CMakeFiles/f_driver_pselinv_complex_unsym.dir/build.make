# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mx/prj/pexsi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mx/prj/pexsi/build_gnu

# Include any dependencies generated for this target.
include fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/compiler_depend.make

# Include the progress variables for this target.
include fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/progress.make

# Include the compile flags for this target's objects.
include fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/flags.make

fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.o: fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/flags.make
fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.o: ../fortran/f_driver_pselinv_complex_unsym.f90
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building Fortran object fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.o"
	cd /home/mx/prj/pexsi/build_gnu/fortran && /bin/mpifort $(Fortran_DEFINES) $(Fortran_INCLUDES) $(Fortran_FLAGS) -c /home/mx/prj/pexsi/fortran/f_driver_pselinv_complex_unsym.f90 -o CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.o

fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing Fortran source to CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.i"
	cd /home/mx/prj/pexsi/build_gnu/fortran && /bin/mpifort $(Fortran_DEFINES) $(Fortran_INCLUDES) $(Fortran_FLAGS) -E /home/mx/prj/pexsi/fortran/f_driver_pselinv_complex_unsym.f90 > CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.i

fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling Fortran source to assembly CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.s"
	cd /home/mx/prj/pexsi/build_gnu/fortran && /bin/mpifort $(Fortran_DEFINES) $(Fortran_INCLUDES) $(Fortran_FLAGS) -S /home/mx/prj/pexsi/fortran/f_driver_pselinv_complex_unsym.f90 -o CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.s

# Object files for target f_driver_pselinv_complex_unsym
f_driver_pselinv_complex_unsym_OBJECTS = \
"CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.o"

# External object files for target f_driver_pselinv_complex_unsym
f_driver_pselinv_complex_unsym_EXTERNAL_OBJECTS =

fortran/f_driver_pselinv_complex_unsym: fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/f_driver_pselinv_complex_unsym.f90.o
fortran/f_driver_pselinv_complex_unsym: fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/build.make
fortran/f_driver_pselinv_complex_unsym: src/libpexsi.a
fortran/f_driver_pselinv_complex_unsym: external/parmetis_4.0.3/libparmetis/libparmetis.a
fortran/f_driver_pselinv_complex_unsym: external/SuperLU_DIST_8.1.0/SRC/libsuperlu_dist.a
fortran/f_driver_pselinv_complex_unsym: libmetis/libmetis/libmetis.a
fortran/f_driver_pselinv_complex_unsym: /usr/lib/x86_64-linux-gnu/libblas.so.3
fortran/f_driver_pselinv_complex_unsym: /usr/lib/x86_64-linux-gnu/lapack/liblapack.so.3
fortran/f_driver_pselinv_complex_unsym: fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking Fortran executable f_driver_pselinv_complex_unsym"
	cd /home/mx/prj/pexsi/build_gnu/fortran && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/f_driver_pselinv_complex_unsym.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/build: fortran/f_driver_pselinv_complex_unsym
.PHONY : fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/build

fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/clean:
	cd /home/mx/prj/pexsi/build_gnu/fortran && $(CMAKE_COMMAND) -P CMakeFiles/f_driver_pselinv_complex_unsym.dir/cmake_clean.cmake
.PHONY : fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/clean

fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/depend:
	cd /home/mx/prj/pexsi/build_gnu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mx/prj/pexsi /home/mx/prj/pexsi/fortran /home/mx/prj/pexsi/build_gnu /home/mx/prj/pexsi/build_gnu/fortran /home/mx/prj/pexsi/build_gnu/fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : fortran/CMakeFiles/f_driver_pselinv_complex_unsym.dir/depend

