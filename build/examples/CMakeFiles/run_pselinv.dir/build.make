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
CMAKE_SOURCE_DIR = /ws/pexsi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /ws/pexsi/build

# Include any dependencies generated for this target.
include examples/CMakeFiles/run_pselinv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include examples/CMakeFiles/run_pselinv.dir/compiler_depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/run_pselinv.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/run_pselinv.dir/flags.make

examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o: examples/CMakeFiles/run_pselinv.dir/flags.make
examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o: ../examples/run_pselinv.cpp
examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o: examples/CMakeFiles/run_pselinv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/ws/pexsi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o"
	cd /ws/pexsi/build/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o -MF CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o.d -o CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o -c /ws/pexsi/examples/run_pselinv.cpp

examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_pselinv.dir/run_pselinv.cpp.i"
	cd /ws/pexsi/build/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /ws/pexsi/examples/run_pselinv.cpp > CMakeFiles/run_pselinv.dir/run_pselinv.cpp.i

examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_pselinv.dir/run_pselinv.cpp.s"
	cd /ws/pexsi/build/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /ws/pexsi/examples/run_pselinv.cpp -o CMakeFiles/run_pselinv.dir/run_pselinv.cpp.s

# Object files for target run_pselinv
run_pselinv_OBJECTS = \
"CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o"

# External object files for target run_pselinv
run_pselinv_EXTERNAL_OBJECTS =

examples/run_pselinv: examples/CMakeFiles/run_pselinv.dir/run_pselinv.cpp.o
examples/run_pselinv: examples/CMakeFiles/run_pselinv.dir/build.make
examples/run_pselinv: src/libpexsi.a
examples/run_pselinv: external/parmetis_4.0.3/libparmetis/libparmetis.a
examples/run_pselinv: external/SuperLU_DIST_8.1.0/SRC/libsuperlu_dist.a
examples/run_pselinv: libmetis/libmetis/libmetis.a
examples/run_pselinv: /usr/lib/gcc/x86_64-linux-gnu/11/libgomp.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libpthread.a
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/openmpi/lib/libmpi_cxx.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libmpi_usempif08.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libmpi_usempi_ignore_tkr.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libmpi_mpifh.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/openmpi/lib/libmpi.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libopen-rte.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libopen-pal.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libhwloc.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libevent_core.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libevent_pthreads.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libm.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libz.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/liblapack.so
examples/run_pselinv: /usr/lib/x86_64-linux-gnu/libblas.so
examples/run_pselinv: examples/CMakeFiles/run_pselinv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/ws/pexsi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable run_pselinv"
	cd /ws/pexsi/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/run_pselinv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/run_pselinv.dir/build: examples/run_pselinv
.PHONY : examples/CMakeFiles/run_pselinv.dir/build

examples/CMakeFiles/run_pselinv.dir/clean:
	cd /ws/pexsi/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/run_pselinv.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/run_pselinv.dir/clean

examples/CMakeFiles/run_pselinv.dir/depend:
	cd /ws/pexsi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /ws/pexsi /ws/pexsi/examples /ws/pexsi/build /ws/pexsi/build/examples /ws/pexsi/build/examples/CMakeFiles/run_pselinv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/run_pselinv.dir/depend

