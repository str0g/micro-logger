# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.21)
project(micro_logger VERSION 0.2 LANGUAGES C)

option(WITH_TESTS "Enable unit tests" ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

aux_source_directory(../src BACKPORT_SRC)
add_library(${PROJECT_NAME} SHARED ${BACKPORT_SRC} micro_logger.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -DUSE_C_VERSION=1)
target_include_directories(${PROJECT_NAME} PUBLIC ../includes)

configure_file(../package/micro_logger.pc.in micro_logger.pc @ONLY)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/micro_logger.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

if(WITH_MICRO_LOGGER_TESTS)
  custom_c_test(demo_c)
endif()
