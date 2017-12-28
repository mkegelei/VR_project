<<<<<<< HEAD
# Install script for directory: /home/panda-cool/openGL/VR_project
=======
# Install script for directory: /home/lucas/VR_project
>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
<<<<<<< HEAD
  include("/home/panda-cool/openGL/VR_project/build/src/lib/glfw/cmake_install.cmake")
  include("/home/panda-cool/openGL/VR_project/build/src/lib/assimp/cmake_install.cmake")
  include("/home/panda-cool/openGL/VR_project/build/src/lib/bullet/cmake_install.cmake")
=======
  include("/home/lucas/VR_project/build/src/lib/glfw/cmake_install.cmake")
  include("/home/lucas/VR_project/build/src/lib/assimp/cmake_install.cmake")
  include("/home/lucas/VR_project/build/src/lib/bullet/cmake_install.cmake")
>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
<<<<<<< HEAD
file(WRITE "/home/panda-cool/openGL/VR_project/build/${CMAKE_INSTALL_MANIFEST}"
=======
file(WRITE "/home/lucas/VR_project/build/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
