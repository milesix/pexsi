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
include examples/CMakeFiles/run_pselinv_unsym.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include examples/CMakeFiles/run_pselinv_unsym.dir/compiler_depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/run_pselinv_unsym.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/run_pselinv_unsym.dir/flags.make

examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o: examples/CMakeFiles/run_pselinv_unsym.dir/flags.make
examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o: ../examples/run_pselinv_unsym.cpp
examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o: examples/CMakeFiles/run_pselinv_unsym.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o"
	cd /home/mx/prj/pexsi/build_gnu/examples && /bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o -MF CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o.d -o CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o -c /home/mx/prj/pexsi/examples/run_pselinv_unsym.cpp

examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.i"
	cd /home/mx/prj/pexsi/build_gnu/examples && /bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mx/prj/pexsi/examples/run_pselinv_unsym.cpp > CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.i

examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.s"
	cd /home/mx/prj/pexsi/build_gnu/examples && /bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mx/prj/pexsi/examples/run_pselinv_unsym.cpp -o CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.s

# Object files for target run_pselinv_unsym
run_pselinv_unsym_OBJECTS = \
"CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o"

# External object files for target run_pselinv_unsym
run_pselinv_unsym_EXTERNAL_OBJECTS =

examples/run_pselinv_unsym: examples/CMakeFiles/run_pselinv_unsym.dir/run_pselinv_unsym.cpp.o
examples/run_pselinv_unsym: examples/CMakeFiles/run_pselinv_unsym.dir/build.make
examples/run_pselinv_unsym: src/libpexsi.a
examples/run_pselinv_unsym: external/parmetis_4.0.3/libparmetis/libparmetis.a
examples/run_pselinv_unsym: external/SuperLU_DIST_8.1.0/SRC/libsuperlu_dist.a
examples/run_pselinv_unsym: libmetis/libmetis/libmetis.a
examples/run_pselinv_unsym: /usr/lib/x86_64-linux-gnu/libblas.so.3
examples/run_pselinv_unsym: /usr/lib/x86_64-linux-gnu/lapack/liblapack.so.3
examples/run_pselinv_unsym: examples/CMakeFiles/run_pselinv_unsym.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable run_pselinv_unsym"
	cd /home/mx/prj/pexsi/build_gnu/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/run_pselinv_unsym.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/run_pselinv_unsym.dir/build: examples/run_pselinv_unsym
.PHONY : examples/CMakeFiles/run_pselinv_unsym.dir/build

examples/CMakeFiles/run_pselinv_unsym.dir/clean:
	cd /home/mx/prj/pexsi/build_gnu/examples && $(CMAKE_COMMAND) -P CMakeFiles/run_pselinv_unsym.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/run_pselinv_unsym.dir/clean

examples/CMakeFiles/run_pselinv_unsym.dir/depend:
	cd /home/mx/prj/pexsi/build_gnu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mx/prj/pexsi /home/mx/prj/pexsi/examples /home/mx/prj/pexsi/build_gnu /home/mx/prj/pexsi/build_gnu/examples /home/mx/prj/pexsi/build_gnu/examples/CMakeFiles/run_pselinv_unsym.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/run_pselinv_unsym.dir/depend

