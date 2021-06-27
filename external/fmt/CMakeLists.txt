#
# Copyright (c) 2021 Krister Joas
#
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/download/CMakeLists.txt"
  CONTENT [[
cmake_minimum_required(VERSION 3.18.0)

project(fmt.download NONE)

include(ExternalProject)

ExternalProject_Add(fmt.external
  URL https://github.com/fmtlib/fmt/archive/8.0.0.tar.gz
  URL_HASH SHA256=7bce0e9e022e586b178b150002e7c2339994e3c2bbe44027e9abb0d60f9cce83
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/src
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build
  INSTALL_DIR ${PROJECT_BINARY_DIR}/install
  CONFIGURE_COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}"
                                       -D CMAKE_BUILD_TYPE=Release
                                       -D FMT_TEST=OFF
                                       -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                                       <SOURCE_DIR>
  BUILD_COMMAND "${CMAKE_COMMAND}" --build . --config Release
  INSTALL_COMMAND "${CMAKE_COMMAND}" --install . --config Release
  TEST_COMMAND ""
  USES_TERMINAL_BUILD TRUE
  )
]])

execute_process(
  COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/download")
execute_process(
  COMMAND "${CMAKE_COMMAND}" --build .
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/download")
