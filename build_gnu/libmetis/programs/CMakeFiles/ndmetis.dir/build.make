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
include libmetis/programs/CMakeFiles/ndmetis.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include libmetis/programs/CMakeFiles/ndmetis.dir/compiler_depend.make

# Include the progress variables for this target.
include libmetis/programs/CMakeFiles/ndmetis.dir/progress.make

# Include the compile flags for this target's objects.
include libmetis/programs/CMakeFiles/ndmetis.dir/flags.make

libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/flags.make
libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.o: ../external/parmetis_4.0.3/metis/programs/ndmetis.c
libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.o"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.o -MF CMakeFiles/ndmetis.dir/ndmetis.c.o.d -o CMakeFiles/ndmetis.dir/ndmetis.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/ndmetis.c

libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ndmetis.dir/ndmetis.c.i"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/ndmetis.c > CMakeFiles/ndmetis.dir/ndmetis.c.i

libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ndmetis.dir/ndmetis.c.s"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/ndmetis.c -o CMakeFiles/ndmetis.dir/ndmetis.c.s

libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/flags.make
libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o: ../external/parmetis_4.0.3/metis/programs/cmdline_ndmetis.c
libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o -MF CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o.d -o CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/cmdline_ndmetis.c

libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.i"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/cmdline_ndmetis.c > CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.i

libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.s"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/cmdline_ndmetis.c -o CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.s

libmetis/programs/CMakeFiles/ndmetis.dir/io.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/flags.make
libmetis/programs/CMakeFiles/ndmetis.dir/io.c.o: ../external/parmetis_4.0.3/metis/programs/io.c
libmetis/programs/CMakeFiles/ndmetis.dir/io.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object libmetis/programs/CMakeFiles/ndmetis.dir/io.c.o"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT libmetis/programs/CMakeFiles/ndmetis.dir/io.c.o -MF CMakeFiles/ndmetis.dir/io.c.o.d -o CMakeFiles/ndmetis.dir/io.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/io.c

libmetis/programs/CMakeFiles/ndmetis.dir/io.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ndmetis.dir/io.c.i"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/io.c > CMakeFiles/ndmetis.dir/io.c.i

libmetis/programs/CMakeFiles/ndmetis.dir/io.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ndmetis.dir/io.c.s"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/io.c -o CMakeFiles/ndmetis.dir/io.c.s

libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/flags.make
libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.o: ../external/parmetis_4.0.3/metis/programs/smbfactor.c
libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.o: libmetis/programs/CMakeFiles/ndmetis.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.o"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.o -MF CMakeFiles/ndmetis.dir/smbfactor.c.o.d -o CMakeFiles/ndmetis.dir/smbfactor.c.o -c /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/smbfactor.c

libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ndmetis.dir/smbfactor.c.i"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/smbfactor.c > CMakeFiles/ndmetis.dir/smbfactor.c.i

libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ndmetis.dir/smbfactor.c.s"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && /bin/mpicc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs/smbfactor.c -o CMakeFiles/ndmetis.dir/smbfactor.c.s

# Object files for target ndmetis
ndmetis_OBJECTS = \
"CMakeFiles/ndmetis.dir/ndmetis.c.o" \
"CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o" \
"CMakeFiles/ndmetis.dir/io.c.o" \
"CMakeFiles/ndmetis.dir/smbfactor.c.o"

# External object files for target ndmetis
ndmetis_EXTERNAL_OBJECTS =

libmetis/programs/ndmetis: libmetis/programs/CMakeFiles/ndmetis.dir/ndmetis.c.o
libmetis/programs/ndmetis: libmetis/programs/CMakeFiles/ndmetis.dir/cmdline_ndmetis.c.o
libmetis/programs/ndmetis: libmetis/programs/CMakeFiles/ndmetis.dir/io.c.o
libmetis/programs/ndmetis: libmetis/programs/CMakeFiles/ndmetis.dir/smbfactor.c.o
libmetis/programs/ndmetis: libmetis/programs/CMakeFiles/ndmetis.dir/build.make
libmetis/programs/ndmetis: libmetis/libmetis/libmetis.a
libmetis/programs/ndmetis: libmetis/programs/CMakeFiles/ndmetis.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mx/prj/pexsi/build_gnu/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable ndmetis"
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ndmetis.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libmetis/programs/CMakeFiles/ndmetis.dir/build: libmetis/programs/ndmetis
.PHONY : libmetis/programs/CMakeFiles/ndmetis.dir/build

libmetis/programs/CMakeFiles/ndmetis.dir/clean:
	cd /home/mx/prj/pexsi/build_gnu/libmetis/programs && $(CMAKE_COMMAND) -P CMakeFiles/ndmetis.dir/cmake_clean.cmake
.PHONY : libmetis/programs/CMakeFiles/ndmetis.dir/clean

libmetis/programs/CMakeFiles/ndmetis.dir/depend:
	cd /home/mx/prj/pexsi/build_gnu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mx/prj/pexsi /home/mx/prj/pexsi/external/parmetis_4.0.3/metis/programs /home/mx/prj/pexsi/build_gnu /home/mx/prj/pexsi/build_gnu/libmetis/programs /home/mx/prj/pexsi/build_gnu/libmetis/programs/CMakeFiles/ndmetis.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libmetis/programs/CMakeFiles/ndmetis.dir/depend

