#
# Copyright (c) 2021 Krister Joas
#
cmake_minimum_required(VERSION 3.18.0)
project(option VERSION 1.1.0)
set(CMAKE_CXX_STANDARD 17)
include(CTest)

add_subdirectory(external)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

add_library(option INTERFACE)
target_sources(option
  INTERFACE
  src/option/Commands.hh
  src/option/Option.hh
  src/option/Program.hh
  src/option/parse_args.hh
  src/option/string_functions.hh
  src/option/usage.hh
  src/option/overloaded.hh)
target_link_libraries(option INTERFACE fmt::fmt)
target_include_directories(option INTERFACE "src")

# This library is here to compile the empty .cc files.  They only
# exist to test that the .hh files include all needed headers and they
# don't contain any symbols.
add_library(_option OBJECT)
target_sources(_option
  PRIVATE
  src/option/Commands.cc
  src/option/Option.cc
  src/option/Program.cc
  src/option/parse_args.cc
  src/option/string_functions.cc
  src/option/usage.cc)
target_link_libraries(_option PRIVATE option fmt::fmt)

#
# External dependencies downloaded during the CMake configuration
# phase are installed here.
#
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_BINARY_DIR}/install")

add_executable(option_test)
target_sources(option_test
  PRIVATE
  src/option/Program.test.cc)
target_link_libraries(option_test
  PRIVATE
  option
  fmt::fmt
  Catch2::Catch2)

add_test(NAME option
  COMMAND option_test)
