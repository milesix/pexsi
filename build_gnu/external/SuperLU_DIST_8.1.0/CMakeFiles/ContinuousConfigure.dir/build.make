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

# Utility rule file for ContinuousConfigure.

# Include any custom commands dependencies for this target.
include external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/compiler_depend.make

# Include the progress variables for this target.
include external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/progress.make

external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure:
	cd /home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0 && /usr/bin/ctest -D ContinuousConfigure

ContinuousConfigure: external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure
ContinuousConfigure: external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/build.make
.PHONY : ContinuousConfigure

# Rule to build all files generated by this target.
external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/build: ContinuousConfigure
.PHONY : external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/build

external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/clean:
	cd /home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0 && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousConfigure.dir/cmake_clean.cmake
.PHONY : external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/clean

external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/depend:
	cd /home/mx/prj/pexsi/build_gnu && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mx/prj/pexsi /home/mx/prj/pexsi/external/SuperLU_DIST_8.1.0 /home/mx/prj/pexsi/build_gnu /home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0 /home/mx/prj/pexsi/build_gnu/external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousConfigure.dir/depend

