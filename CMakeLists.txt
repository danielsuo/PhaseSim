cmake_minimum_required (VERSION 3.5)

set (PROJECT_NAME phasesim)

project(${PROJECT_NAME})

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

option (BuildPintools "BuildPintools" OFF)
option (BuildTools "BuildTools" OFF)

# Grab git hash
execute_process(COMMAND git rev-parse HEAD OUTPUT_VARIABLE GITHASH)
add_definitions(-DGITHASH=${GITHASH})
message("Git hash: " ${GITHASH})

# Add dependencies
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/clipp)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/pin)

# Sources
file (GLOB MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file (GLOB PREFETCHER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/policies/prefetcher/*.cc)
file (GLOB BRANCH_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/policies/branch/*.cc)
file (GLOB REPLACEMENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/policies/replacement/*.cc)

set (
  POLICY_SOURCE
  ${PREFETCHER_SOURCE}
  ${REPLACEMENT_SOURCE}
  ${BRANCH_SOURCE}
  )

# Include directories
include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CLIPP_INCLUDE_DIRS}
  )

add_executable (
  ${PROJECT_NAME}
  ${MAIN_SOURCE}
  ${POLICY_SOURCE}
  )
