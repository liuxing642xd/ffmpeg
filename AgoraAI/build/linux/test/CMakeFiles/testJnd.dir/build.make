# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /home/agoravideo/.vs/cmake/bin/cmake

# The command to remove a file.
RM = /home/agoravideo/.vs/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/proj.cmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux

# Include any dependencies generated for this target.
include test/CMakeFiles/testJnd.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/testJnd.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/testJnd.dir/flags.make

test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.o: test/CMakeFiles/testJnd.dir/flags.make
test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.o: /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.o"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && /home/jenkins/coverage/bullseye/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.o -c /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp

test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.i"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && /home/jenkins/coverage/bullseye/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp > CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.i

test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.s"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && /home/jenkins/coverage/bullseye/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp -o CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.s

test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.o: test/CMakeFiles/testJnd.dir/flags.make
test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.o: /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.o"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && /home/jenkins/coverage/bullseye/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.o -c /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp

test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.i"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && /home/jenkins/coverage/bullseye/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp > CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.i

test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.s"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && /home/jenkins/coverage/bullseye/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp -o CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.s

# Object files for target testJnd
testJnd_OBJECTS = \
"CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.o" \
"CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.o"

# External object files for target testJnd
testJnd_EXTERNAL_OBJECTS =

test/testJnd: test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/Test.cpp.o
test/testJnd: test/CMakeFiles/testJnd.dir/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/src/test/jnd_filter_sample.cpp.o
test/testJnd: test/CMakeFiles/testJnd.dir/build.make
test/testJnd: test/CMakeFiles/testJnd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable testJnd"
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testJnd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/testJnd.dir/build: test/testJnd

.PHONY : test/CMakeFiles/testJnd.dir/build

test/CMakeFiles/testJnd.dir/clean:
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test && $(CMAKE_COMMAND) -P CMakeFiles/testJnd.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/testJnd.dir/clean

test/CMakeFiles/testJnd.dir/depend:
	cd /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/proj.cmake /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/proj.cmake/test /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test /home/agoravideo/Downloads/ffmpeg-4.2.1/AgoraAI/build/linux/test/CMakeFiles/testJnd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/testJnd.dir/depend

