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

# Utility rule file for ContinuousBuild.

# Include any custom commands dependencies for this target.
include external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/compiler_depend.make

# Include the progress variables for this target.
include external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/progress.make

external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild:
	cd /ws/pexsi/build/external/SuperLU_DIST_8.1.0 && /usr/bin/ctest -D ContinuousBuild

ContinuousBuild: external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild
ContinuousBuild: external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/build.make
.PHONY : ContinuousBuild

# Rule to build all files generated by this target.
external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/build: ContinuousBuild
.PHONY : external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/build

external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/clean:
	cd /ws/pexsi/build/external/SuperLU_DIST_8.1.0 && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousBuild.dir/cmake_clean.cmake
.PHONY : external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/clean

external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/depend:
	cd /ws/pexsi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /ws/pexsi /ws/pexsi/external/SuperLU_DIST_8.1.0 /ws/pexsi/build /ws/pexsi/build/external/SuperLU_DIST_8.1.0 /ws/pexsi/build/external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/SuperLU_DIST_8.1.0/CMakeFiles/ContinuousBuild.dir/depend

