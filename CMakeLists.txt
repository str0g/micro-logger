# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.21)
project(micro_logger++ VERSION 0.2 LANGUAGES CXX)

option(WITH_MICRO_LOGGER_TESTS "Enable unit tests" OFF)
include(GoogleTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

include_directories(includes)
aux_source_directory(src SRC)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
    $<INSTALL_INTERFACE:include>
)

set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/micro_logger)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY includes/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

aux_source_directory(backport SRC_BACKPORT)
add_library(micro_logger SHARED ${SRC_BACKPORT} ${SRC})
target_compile_options(micro_logger PRIVATE -DUSE_C_VERSION=1)
add_subdirectory(backport)

configure_file(package/micro_logger.pc.in micro_logger++.pc @ONLY)

if(WITH_MICRO_LOGGER_TESTS)
enable_testing()

function(custom_test test_name)
    add_executable(${test_name} tests/${test_name}.cpp)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
endfunction()

function(custom_gtest test_name)
    add_executable(${test_name} tests/${test_name}.cpp)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread -lgtest -lgtest_main)
    gtest_add_tests(
      TARGET ${test_name}
    )
endfunction()

custom_test(demo)
custom_test(benchmark)
custom_gtest(test_hex)

endif()
