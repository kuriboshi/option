# Copyright 2021, 2024 Krister Joas
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

cmake_minimum_required(VERSION 3.21.0)
project(option VERSION 2.1.0)
set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

add_subdirectory(external)

#
# External dependencies downloaded during the CMake configuration phase are
# installed here.
#
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}/install")

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

add_library(option INTERFACE)
add_library(Option::option ALIAS option)
target_sources(option INTERFACE)
set_property(
  TARGET option
  PROPERTY PUBLIC_HEADER
           src/option/Commands.hh
           src/option/Option.hh
           src/option/Program.hh
           src/option/parse_args.hh
           src/option/string_functions.hh
           src/option/usage.hh
           src/option/overloaded.hh)
target_link_libraries(option INTERFACE fmt::fmt)
target_include_directories(
  option INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_subdirectory(examples)

#
# This library is here to compile the empty .cc files.  They only exist to test
# that the .hh files include all needed headers and they don't contain any
# symbols.
#
add_library(_option OBJECT)
target_sources(
  _option
  PRIVATE src/option/Commands.cc src/option/Option.cc src/option/Program.cc
          src/option/parse_args.cc src/option/string_functions.cc
          src/option/usage.cc)
target_link_libraries(_option PRIVATE option fmt::fmt)

#
# Installation section.
#
include(GNUInstallDirs)
install(
  TARGETS option
  EXPORT OptionTargets
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/option)
install(
  EXPORT OptionTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Option
  NAMESPACE Option::)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/OptionConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/OptionConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/OptionConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Option)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OptionConfigVersion.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/OptionConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Option)

#
# Unit test section.
#
include(CTest)
add_executable(option_test)
target_sources(option_test PRIVATE src/option/Commands.test.cc
                                   src/option/Program.test.cc)
target_link_libraries(option_test PRIVATE option fmt::fmt Catch2::Catch2)
add_test(NAME option COMMAND option_test)
