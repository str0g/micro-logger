# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.23)
project(micro_logger++ VERSION 0.2 LANGUAGES CXX)

option(WITH_MICRO_LOGGER_TESTS "Enable unit tests" OFF)
option(WITH_SANITIZER "Enable sanitizer" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${WITH_SANITIZER})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

if(WITH_MICRO_LOGGER_TESTS)
include(cmake/gtest_wrapper.cmake)
include(cmake/other_tests.cmake)

enable_testing()

custom_test_app(demo)
custom_test_app(benchmark)
custom_gtest(test_hex)

endif()

include_directories(includes)
aux_source_directory(src SRC)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(backport)

#Installetion part
include(GNUInstallDirs)

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

install(DIRECTORY includes/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/micro_logger)

