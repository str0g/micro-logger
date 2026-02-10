# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.25)
project(
  micro_logger_project
  VERSION 0.31
  LANGUAGES CXX)

# ------------------------------------------------------------
# Detect top-level
# ------------------------------------------------------------
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(MICRO_LOGGER_TOP_LEVEL ON)
else()
  set(MICRO_LOGGER_TOP_LEVEL OFF)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(MICRO_LOGGER_TOP_LEVEL)
  option(MICRO_LOGGER_DEMOS "Enable demo applications" OFF)
  option(MICRO_LOGGER_BUILD_TESTS "Enable unit tests" OFF)
  option(MICRO_LOGGER_SANITIZER "Enable sanitizer" OFF)
else()
  set(MICRO_LOGGER_BUILD_TESTS OFF)
  set(MICRO_LOGGER_BUILD_DEMOS OFF)
  set(MICRO_LOGGER_SANITIZER OFF)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(INCLUDE_INSTALL_DIR
    ${CMAKE_INSTALL_INCLUDEDIR}/micro_logger
    CACHE PATH "Location of header files")

if(MICRO_LOGGER_SANITIZER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if(MICRO_LOGGER_BUILD_DEMOS)
  include(cmake/other_tests.cmake)
endif()

if(MICRO_LOGGER_BUILD_TESTS)
  include(cmake/gtest_wrapper.cmake)
  enable_testing()
endif()

add_subdirectory(micro_logger++)
add_subdirectory(micro_logger)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/micro_logger.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/micro_logger
  PATH_VARS INCLUDE_INSTALL_DIR)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/micro_logger)

install(
  EXPORT micro_logger++Targets
  FILE micro_loggerTargets.cmake
  NAMESPACE micro_logger::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/micro_logger)
