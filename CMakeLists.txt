cmake_minimum_required(VERSION 3.0)
project(vr)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(src/lib/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(src/lib/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(src/lib/bullet)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(src/vr/
                    build/
                    src/lib/assimp/include/
                    src/lib/bullet/src/
                    src/lib/glad/include/
                    src/lib/glfw/include/
                    src/lib/glm/
                    src/lib/stb/)

file(GLOB LIB_SOURCES src/lib/glad/src/glad.c)
file(GLOB_RECURSE PROJECT_SOURCES src/vr/*.hpp
                          src/vr/*.h
						  src/vr/*.cpp)
file(GLOB PROJECT_SHADERS src/shaders/*.comp
                          src/shaders/*.frag
                          src/shaders/*.geom
                          src/shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)
	 
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("lib" FILES ${LIB_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
foreach(SHADER ${PROJECT_SHADERS})
	if(UNIX)
		file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
	endif(UNIX)

endforeach(SHADER)
