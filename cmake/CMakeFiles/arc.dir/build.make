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
CMAKE_SOURCE_DIR = /user/3/.base/badaouii/home/projet_c/projet_c_ig

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake

# Include any dependencies generated for this target.
include CMakeFiles/arc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/arc.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/arc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/arc.dir/flags.make

CMakeFiles/arc.dir/tests/arc.c.o: CMakeFiles/arc.dir/flags.make
CMakeFiles/arc.dir/tests/arc.c.o: ../tests/arc.c
CMakeFiles/arc.dir/tests/arc.c.o: CMakeFiles/arc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/arc.dir/tests/arc.c.o"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/arc.dir/tests/arc.c.o -MF CMakeFiles/arc.dir/tests/arc.c.o.d -o CMakeFiles/arc.dir/tests/arc.c.o -c /user/3/.base/badaouii/home/projet_c/projet_c_ig/tests/arc.c

CMakeFiles/arc.dir/tests/arc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/arc.dir/tests/arc.c.i"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /user/3/.base/badaouii/home/projet_c/projet_c_ig/tests/arc.c > CMakeFiles/arc.dir/tests/arc.c.i

CMakeFiles/arc.dir/tests/arc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/arc.dir/tests/arc.c.s"
	/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /user/3/.base/badaouii/home/projet_c/projet_c_ig/tests/arc.c -o CMakeFiles/arc.dir/tests/arc.c.s

# Object files for target arc
arc_OBJECTS = \
"CMakeFiles/arc.dir/tests/arc.c.o"

# External object files for target arc
arc_EXTERNAL_OBJECTS =

arc: CMakeFiles/arc.dir/tests/arc.c.o
arc: CMakeFiles/arc.dir/build.make
arc: libei.a
arc: ../_x11/libeibase.a
arc: CMakeFiles/arc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable arc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/arc.dir/build: arc
.PHONY : CMakeFiles/arc.dir/build

CMakeFiles/arc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/arc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/arc.dir/clean

CMakeFiles/arc.dir/depend:
	cd /user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /user/3/.base/badaouii/home/projet_c/projet_c_ig /user/3/.base/badaouii/home/projet_c/projet_c_ig /user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake /user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake /user/3/.base/badaouii/home/projet_c/projet_c_ig/cmake/CMakeFiles/arc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/arc.dir/depend

