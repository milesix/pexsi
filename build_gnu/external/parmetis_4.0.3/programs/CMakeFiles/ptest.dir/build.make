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
include external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/compiler_depend.make

# Include the progress variables for this target.
include external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/progress.make

# Include the compile flags for this target's objects.
include external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/flags.make

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.o: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/flags.make
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.o: ../external/parmetis_4.0.3/programs/ptest.c
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.o: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.o"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.o -MF CMakeFiles/ptest.dir/ptest.c.o.d -o CMakeFiles/ptest.dir/ptest.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/ptest.c

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ptest.dir/ptest.c.i"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/ptest.c > CMakeFiles/ptest.dir/ptest.c.i

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ptest.dir/ptest.c.s"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/ptest.c -o CMakeFiles/ptest.dir/ptest.c.s

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.o: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/flags.make
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.o: ../external/parmetis_4.0.3/programs/io.c
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.o: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.o"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.o -MF CMakeFiles/ptest.dir/io.c.o.d -o CMakeFiles/ptest.dir/io.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/io.c

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ptest.dir/io.c.i"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/io.c > CMakeFiles/ptest.dir/io.c.i

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ptest.dir/io.c.s"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/io.c -o CMakeFiles/ptest.dir/io.c.s

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.o: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/flags.make
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.o: ../external/parmetis_4.0.3/programs/adaptgraph.c
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.o: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.o"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.o -MF CMakeFiles/ptest.dir/adaptgraph.c.o.d -o CMakeFiles/ptest.dir/adaptgraph.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/adaptgraph.c

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ptest.dir/adaptgraph.c.i"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/adaptgraph.c > CMakeFiles/ptest.dir/adaptgraph.c.i

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ptest.dir/adaptgraph.c.s"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/programs/adaptgraph.c -o CMakeFiles/ptest.dir/adaptgraph.c.s

# Object files for target ptest
ptest_OBJECTS = \
"CMakeFiles/ptest.dir/ptest.c.o" \
"CMakeFiles/ptest.dir/io.c.o" \
"CMakeFiles/ptest.dir/adaptgraph.c.o"

# External object files for target ptest
ptest_EXTERNAL_OBJECTS =

external/parmetis_4.0.3/programs/ptest: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/ptest.c.o
external/parmetis_4.0.3/programs/ptest: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/io.c.o
external/parmetis_4.0.3/programs/ptest: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/adaptgraph.c.o
external/parmetis_4.0.3/programs/ptest: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/build.make
external/parmetis_4.0.3/programs/ptest: external/parmetis_4.0.3/libparmetis/libparmetis.a
external/parmetis_4.0.3/programs/ptest: libmetis/libmetis/libmetis.a
external/parmetis_4.0.3/programs/ptest: external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable ptest"
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ptest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/build: external/parmetis_4.0.3/programs/ptest
.PHONY : external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/build

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/clean:
	cd /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs && $(CMAKE_COMMAND) -P CMakeFiles/ptest.dir/cmake_clean.cmake
.PHONY : external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/clean

external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/depend:
	cd /home/mx/prj/pexsi/build_gnu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mx/prj/pexsi /home/mx/prj/pexsi/external/parmetis_4.0.3/programs /home/mx/prj/pexsi/build_gnu /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs /home/mx/prj/pexsi/build_gnu/external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/parmetis_4.0.3/programs/CMakeFiles/ptest.dir/depend

