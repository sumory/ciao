cmake_minimum_required (VERSION 2.8)

project (ciao C CXX)

# some configuration.
set (CIAO_VERSION_MAJOR 0)
set (CIAO_VERSION_MINOR 0)
set (CIAO_VERSION_PATCH 1)
option (INJECT_BUILD_DATE "inject build date or not" ON)

configure_file (
  "${PROJECT_SOURCE_DIR}/ciao/config.h.in"
  "${PROJECT_SOURCE_DIR}/ciao/config.h"
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set (CXX_FLAGS
        -g
        -fPIC
        -Wall
        -Wno-unused-parameter
        -Wno-unused-function
        -Wunused-variable
        -Wunused-value
        -Wshadow
        -Wcast-qual
        -Wcast-align
        -Wwrite-strings
        -Wsign-compare
        -Winvalid-pch
        -fms-extensions
        -Wfloat-equal
        -Wextra
        -std=c++11
)

include_directories (${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty)
link_directories (${PROJECT_SOURCE_DIR}/libs)

string (REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -D_DEBUG -DGOOGLE_STRIP_LOG=0 -ftest-coverage")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -DNDEBUG -DGOOGLE_STRIP_LOG=1")
SET(DEPENDENT_LIBRARIES glog pthread)
set (CMAKE_MACOSX_RPATH 0)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

message (STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message (STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
message (STATUS "CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})
message (STATUS "EXECUTABLE_OUTPUT_PATH " ${EXECUTABLE_OUTPUT_PATH})
message (STATUS "LIBRARY_OUTPUT_PATH " ${LIBRARY_OUTPUT_PATH})

add_subdirectory (ciao)
add_subdirectory (test)
