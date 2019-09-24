cmake_minimum_required (VERSION 3.5)

set (PROJECT_NAME phasesim)

project(${PROJECT_NAME})

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

option (BuildPintools "BuildPintools" ON)
option (BuildTools "BuildTools" ON)
option (BuildTests "BuildTests" OFF)

# Grab git hash
execute_process(COMMAND git rev-parse HEAD OUTPUT_VARIABLE GITHASH)
add_definitions(-DGITHASH=${GITHASH})
message("Git hash: " ${GITHASH})

# Add dependencies
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/clipp)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/pin)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml-cpp)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/simpoint)

# Sources
file (GLOB MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file (GLOB PREFETCHER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/policies/prefetcher/*.cc)
file (GLOB BRANCH_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/policies/branch/*.cc)
file (GLOB REPLACEMENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/policies/replacement/*.cc)
file (GLOB TOOLS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/ref/tools/*.cc)
file (GLOB TESTS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/ref/tests/*.cc)

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
  ${PIN_INCLUDE_DIRS}
  ${PIN_XED_INCLUDE_DIRS}
  )

# Link directories
link_directories (
  ${YAML_CPP_LIBRARY_DIRS}
  $ENV{CONDA_PREFIX}/lib
  )

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

# Build tracing libraries
set (
  PINTOOLS
  tracer
  static_instruction_counter
  dynamic_instruction_counter
  )

if (BuildPintools)
  foreach (tool ${PINTOOLS})
    set (PINTOOL_OBJ_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps/pin/obj-intel64)
    file (GLOB TOOL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/ref/pintools/${tool})
    file (MAKE_DIRECTORY ${PINTOOL_OBJ_DIRECTORY})
    file (MAKE_DIRECTORY ${TOOL_SOURCE}/obj-intel64)

    message(${TOOL_SOURCE})

    add_custom_target (
      ${tool}
      ALL
      COMMAND PIN_ROOT=${PIN_ROOT} PHASESIM_HOME=${CMAKE_CURRENT_SOURCE_DIR} make obj-intel64/${tool}.so
      WORKING_DIRECTORY  ${TOOL_SOURCE}
      DEPENDS always_rebuild ${PIN_EXTERNAL_PROJECT}
      )
  endforeach (tool)
endif (BuildPintools)

if (BuildTools)
  foreach (tool_path ${TOOLS_SOURCE})
    get_filename_component (tool_name ${tool_path} NAME_WE)

    add_executable (
      ${tool_name}
      ${tool_path}
      )

    target_link_libraries (
      ${tool_name}
      readline
      z
      ${CMAKE_THREAD_LIBS_INIT}
      )
  endforeach (tool_path)
endif(BuildTools)

if (BuildTests)
  add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/deps/gtest)
  foreach (test_path ${TESTS_SOURCE})
    get_filename_component (test_name ${test_path} NAME_WE)

    add_executable (
      ${test_name}
      ${test_path}
      )

    add_dependencies (
      ${test_name}
      ${GTEST_EXTERNAL_PROJECT}
      )
    target_link_libraries (
      ${test_name}
      ${GTEST_LIBS}
      ${CMAKE_THREAD_LIBS_INIT}
      )
  endforeach ()
endif(BuildTests)

add_custom_command (
  OUTPUT always_rebuild
  COMMAND cmake -E echo
  )
