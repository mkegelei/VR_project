<<<<<<< HEAD
# Install script for directory: /home/panda-cool/openGL/VR_project/src/lib/assimp/code
=======
# Install script for directory: /home/lucas/VR_project/src/lib/assimp/code
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

<<<<<<< HEAD
if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/panda-cool/openGL/VR_project/build/src/lib/assimp/code/libassimp.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/anim.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/ai_assert.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/camera.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/color4.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/color4.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/config.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/defs.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/cfileio.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/light.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/material.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/material.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/matrix3x3.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/matrix3x3.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/matrix4x4.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/matrix4x4.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/mesh.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/postprocess.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/quaternion.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/quaternion.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/scene.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/metadata.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/texture.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/types.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/vector2.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/vector2.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/vector3.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/vector3.inl"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/version.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/cimport.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/importerdesc.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/Importer.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/IOStream.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/IOSystem.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/Logger.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/LogStream.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/NullLogger.hpp"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/cexport.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/Exporter.hpp"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/home/panda-cool/openGL/VR_project/src/lib/assimp/code/../include/assimp/Compiler/pstdint.h"
=======
if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/lucas/VR_project/build/src/lib/assimp/code/libassimp.a")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/anim.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/ai_assert.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/camera.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/color4.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/color4.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/config.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/defs.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/cfileio.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/light.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/material.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/material.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/matrix3x3.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/matrix3x3.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/matrix4x4.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/matrix4x4.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/mesh.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/postprocess.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/quaternion.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/quaternion.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/scene.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/metadata.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/texture.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/types.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/vector2.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/vector2.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/vector3.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/vector3.inl"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/version.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/cimport.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/importerdesc.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/Importer.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/IOStream.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/IOSystem.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/Logger.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/LogStream.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/NullLogger.hpp"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/cexport.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/Exporter.hpp"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/home/lucas/VR_project/src/lib/assimp/code/../include/assimp/Compiler/pstdint.h"
>>>>>>> 1e57215b089641099e79fd8f7b0b1f408357cc43
    )
endif()

