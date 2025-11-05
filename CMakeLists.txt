# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.23)
project(micro_logger++ VERSION 0.23 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(WITH_MICRO_LOGGER_DEMOS "Enable demo applications" OFF)
option(WITH_MICRO_LOGGER_TESTS "Enable unit tests" OFF)
option(WITH_SANITIZER "Enable sanitizer" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/micro_logger
    CACHE PATH "Location of header files" )

if (${WITH_SANITIZER})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if(${WITH_MICRO_LOGGER_DEMOS})
  include(cmake/other_tests.cmake)
endif()

if(${WITH_MICRO_LOGGER_TESTS})
  include(cmake/gtest_wrapper.cmake)
  enable_testing()

  custom_gtest(test_hex)
endif()

if(${WITH_MICRO_LOGGER_DEMOS})
  custom_test_app(demo)
  custom_test_app(benchmark)
endif()

aux_source_directory(src SRC)
add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include/micro_logger>
)

add_subdirectory(micro_logger)

configure_file(package/micro_logger.pc.in micro_logger++.pc @ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/micro_logger++.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY includes/ DESTINATION ${INCLUDE_INSTALL_DIR})

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/micro_logger.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/micro_logger
    PATH_VARS INCLUDE_INSTALL_DIR
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/micro_loggerConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/micro_logger)

install(EXPORT ${PROJECT_NAME}Targets
    FILE micro_loggerTargets.cmake
    NAMESPACE micro_logger::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/micro_logger
)
