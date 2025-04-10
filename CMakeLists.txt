cmake_minimum_required(VERSION 3.0)

project(conet)

option(CONET_BUILD_EXAMPLE "Build conet example" OFF)
option(CONET_BUILD_TEST "Build conet test" ON)
option(ENABLE_CONET_DEBUG "Enable CONET_DEBUG macro" OFF)

set(CXX_FLAGS
 -O2
 -g
 -Wall
 -Wextra
 -Werror
 -std=c++11
 -pthread
)

if(ENABLE_CONET_DEBUG)
  add_definitions(-DCONET_DEBUG)
endif()

# FLAGS
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

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