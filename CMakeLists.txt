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
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml-cpp)

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
  ${YAML_CPP_INCLUDE_DIRS}
  )

# Link directories
link_directories (
  ${YAML_CPP_LIBRARY_DIRS}
  )

message (${YAML_CPP_LIBRARY_DIRS})

add_executable (
  ${PROJECT_NAME}
  ${MAIN_SOURCE}
  ${POLICY_SOURCE}
  )

target_link_libraries (
  ${PROJECT_NAME}
  yaml-cpp
  )

add_dependencies (
  ${PROJECT_NAME}
  ${CLIPP_EXTERNAL_PROJECT}
  ${YAML_CPP_EXTERNAL_PROJECT}
  )
