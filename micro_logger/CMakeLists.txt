# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.21)
project(
  micro_logger
  VERSION 0.40
  LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED src/micro_logger.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -DUSE_C_VERSION=1)
target_link_libraries(${PROJECT_NAME} PUBLIC micro_logger++)
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include/micro_logger>)

configure_file(../package/micro_logger.pc.in ${PROJECT_NAME}.pc @ONLY)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${INCLUDE_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

if(MICRO_LOGGER_BUILD_DEMOS)
  custom_app_test_c(demo)
endif()
