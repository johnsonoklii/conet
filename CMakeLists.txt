cmake_minimum_required(VERSION 3.0)

project(conet)

# 默认编译Release版本
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    message(STATUS "No build type selected, default to Release")
endif()

option(CONET_BUILD_EXAMPLE "Build conet example" OFF)
option(CONET_BUILD_TEST "Build conet test" OFF)
option(ENABLE_CONET_DEBUG "Enable CONET_DEBUG macro" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build detected, enabling debug options")
    set(CONET_BUILD_EXAMPLE ON CACHE BOOL "Force ON for Debug build" FORCE)
    set(CONET_BUILD_TEST ON CACHE BOOL "Force ON for Debug build" FORCE)
    set(ENABLE_CONET_DEBUG ON CACHE BOOL "Force ON for Debug build" FORCE)
endif()

if(ENABLE_CONET_DEBUG)
  add_definitions(-DCONET_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG 
    "-O0 -g3 -Wall -Wextra -Werror -pthread"
    CACHE STRING "Debug compiler flags" FORCE
)

set(CMAKE_CXX_FLAGS_RELEASE
    "-O2 -g -Wall -Wextra -Werror -pthread"
    CACHE STRING "Release compiler flags" FORCE
)

# OUTPUT
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# INCLUDE
include_directories(
  ${PROJECT_SOURCE_DIR}/src/
  ${PROJECT_SOURCE_DIR}/src/
  /usr/local/include
  /usr/include
)

# LIBRARIES
link_directories(
    /usr/local/lib
    /usr/lib
    lib
)

if(CONET_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if(CONET_BUILD_TEST)
  add_subdirectory(test)
endif()

add_subdirectory(src)