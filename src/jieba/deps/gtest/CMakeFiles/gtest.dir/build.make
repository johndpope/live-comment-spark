# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kovean/WORK/live-comment-spark/src/cppjieba-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kovean/WORK/live-comment-spark/src/jieba

# Include any dependencies generated for this target.
include deps/gtest/CMakeFiles/gtest.dir/depend.make

# Include the progress variables for this target.
include deps/gtest/CMakeFiles/gtest.dir/progress.make

# Include the compile flags for this target's objects.
include deps/gtest/CMakeFiles/gtest.dir/flags.make

deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: deps/gtest/CMakeFiles/gtest.dir/flags.make
deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: /home/kovean/WORK/live-comment-spark/src/cppjieba-master/deps/gtest/src/gtest-all.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kovean/WORK/live-comment-spark/src/jieba/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o"
	cd /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest.dir/src/gtest-all.cc.o -c /home/kovean/WORK/live-comment-spark/src/cppjieba-master/deps/gtest/src/gtest-all.cc

deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest-all.cc.i"
	cd /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kovean/WORK/live-comment-spark/src/cppjieba-master/deps/gtest/src/gtest-all.cc > CMakeFiles/gtest.dir/src/gtest-all.cc.i

deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest-all.cc.s"
	cd /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kovean/WORK/live-comment-spark/src/cppjieba-master/deps/gtest/src/gtest-all.cc -o CMakeFiles/gtest.dir/src/gtest-all.cc.s

deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.requires:

.PHONY : deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.requires

deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.provides: deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.requires
	$(MAKE) -f deps/gtest/CMakeFiles/gtest.dir/build.make deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.provides.build
.PHONY : deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.provides

deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.provides.build: deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o


# Object files for target gtest
gtest_OBJECTS = \
"CMakeFiles/gtest.dir/src/gtest-all.cc.o"

# External object files for target gtest
gtest_EXTERNAL_OBJECTS =

deps/gtest/libgtest.a: deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
deps/gtest/libgtest.a: deps/gtest/CMakeFiles/gtest.dir/build.make
deps/gtest/libgtest.a: deps/gtest/CMakeFiles/gtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kovean/WORK/live-comment-spark/src/jieba/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libgtest.a"
	cd /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean_target.cmake
	cd /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
deps/gtest/CMakeFiles/gtest.dir/build: deps/gtest/libgtest.a

.PHONY : deps/gtest/CMakeFiles/gtest.dir/build

deps/gtest/CMakeFiles/gtest.dir/requires: deps/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o.requires

.PHONY : deps/gtest/CMakeFiles/gtest.dir/requires

deps/gtest/CMakeFiles/gtest.dir/clean:
	cd /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean.cmake
.PHONY : deps/gtest/CMakeFiles/gtest.dir/clean

deps/gtest/CMakeFiles/gtest.dir/depend:
	cd /home/kovean/WORK/live-comment-spark/src/jieba && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kovean/WORK/live-comment-spark/src/cppjieba-master /home/kovean/WORK/live-comment-spark/src/cppjieba-master/deps/gtest /home/kovean/WORK/live-comment-spark/src/jieba /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest /home/kovean/WORK/live-comment-spark/src/jieba/deps/gtest/CMakeFiles/gtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : deps/gtest/CMakeFiles/gtest.dir/depend

