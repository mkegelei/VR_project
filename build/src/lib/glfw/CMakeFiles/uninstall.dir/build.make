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
CMAKE_SOURCE_DIR = /home/panda-cool/openGL/VR_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/panda-cool/openGL/VR_project/build

# Utility rule file for uninstall.

# Include the progress variables for this target.
include src/lib/glfw/CMakeFiles/uninstall.dir/progress.make

src/lib/glfw/CMakeFiles/uninstall:
	cd /home/panda-cool/openGL/VR_project/build/src/lib/glfw && /usr/bin/cmake -P /home/panda-cool/openGL/VR_project/build/src/lib/glfw/cmake_uninstall.cmake

uninstall: src/lib/glfw/CMakeFiles/uninstall
uninstall: src/lib/glfw/CMakeFiles/uninstall.dir/build.make

.PHONY : uninstall

# Rule to build all files generated by this target.
src/lib/glfw/CMakeFiles/uninstall.dir/build: uninstall

.PHONY : src/lib/glfw/CMakeFiles/uninstall.dir/build

src/lib/glfw/CMakeFiles/uninstall.dir/clean:
	cd /home/panda-cool/openGL/VR_project/build/src/lib/glfw && $(CMAKE_COMMAND) -P CMakeFiles/uninstall.dir/cmake_clean.cmake
.PHONY : src/lib/glfw/CMakeFiles/uninstall.dir/clean

src/lib/glfw/CMakeFiles/uninstall.dir/depend:
	cd /home/panda-cool/openGL/VR_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/panda-cool/openGL/VR_project /home/panda-cool/openGL/VR_project/src/lib/glfw /home/panda-cool/openGL/VR_project/build /home/panda-cool/openGL/VR_project/build/src/lib/glfw /home/panda-cool/openGL/VR_project/build/src/lib/glfw/CMakeFiles/uninstall.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/lib/glfw/CMakeFiles/uninstall.dir/depend

