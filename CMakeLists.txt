# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.5)
project(micro_logger++ VERSION 0.1 LANGUAGES CXX)

option(WITH_MICRO_LOGGER_TESTS "Enable unit tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

include_directories(includes)
aux_source_directory(src SRC)

add_library(${PROJECT_NAME} SHARED ${SRC})

aux_source_directory(backport SRC_BACKPORT)
add_library(micro_logger SHARED ${SRC_BACKPORT} ${SRC})
target_compile_options(micro_logger PRIVATE -DUSE_C_VERSION=1)
add_subdirectory(backport)

if(WITH_MICRO_LOGGER_TESTS)
enable_testing()

function(custom_test test_name)
    add_executable(${test_name} tests/${test_name}.cpp)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
    add_test(${test_name} ${test_name})
endfunction()

custom_test(main)

endif()
