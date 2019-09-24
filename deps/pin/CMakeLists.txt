cmake_minimum_required (VERSION 3.1)

include (ExternalProject)

set (PIN_EXTERNAL_PROJECT pin-git)
message (${CMAKE_CURRENT_BINARY_DIR})
set (PIN_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PIN_EXTERNAL_PROJECT}-prefix/src/${PIN_EXTERNAL_PROJECT})

set (PIN_URL "https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.7-97619-g0d0c92f4f-gcc-linux.tar.gz")
if (APPLE)
  set (PIN_URL "https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.7-97619-g0d0c92f4f-clang-mac.tar.gz")
endif (APPLE)

ExternalProject_Add (
  ${PIN_EXTERNAL_PROJECT}
  URL ${PIN_URL}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )

set (PIN_EXTERNAL_PROJECT ${PIN_EXTERNAL_PROJECT} PARENT_SCOPE)
set (PIN_ROOT ${PIN_SOURCE_DIR} PARENT_SCOPE)
set (PIN_XED_INCLUDE_DIRS ${PIN_SOURCE_DIR}/extras/xed-intel64/include PARENT_SCOPE)
set (PIN_INCLUDE_DIRS ${PIN_SOURCE_DIR}/source/include/pin PARENT_SCOPE)
