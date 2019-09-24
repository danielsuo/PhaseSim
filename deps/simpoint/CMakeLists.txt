cmake_minimum_required (VERSION 3.1)

include (ExternalProject)

set (SIMPOINT_EXTERNAL_PROJECT simpoint-git)
message (${CMAKE_CURRENT_BINARY_DIR})
set (SIMPOINT_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SIMPOINT_EXTERNAL_PROJECT}-prefix/src/${SIMPOINT_EXTERNAL_PROJECT})

#set (SIMPOINT_URL "https://cseweb.ucsd.edu/~calder/simpoint/releases/SimPoint.3.2.tar.gz")

ExternalProject_Add (
  ${SIMPOINT_EXTERNAL_PROJECT}
  #URL ${SIMPOINT_URL}
  GIT_REPOSITORY "https://github.com/danielsuo/simpoint"
  GIT_TAG "master"
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make -j
  INSTALL_COMMAND ""
  )

set (SIMPOINT_EXTERNAL_PROJECT ${SIMPOINT_EXTERNAL_PROJECT} PARENT_SCOPE)
set (SIMPOINT_ROOT ${SIMPOINT_SOURCE_DIR} PARENT_SCOPE)
