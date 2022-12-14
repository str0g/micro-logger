# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.5)
project(micro_logger VERSION 0.1 LANGUAGES C)

option(WITH_TESTS "Enable unit tests" ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(WITH_TESTS)
    enable_testing()

    function(custom_c_test test_name)
        add_executable(${test_name} ../tests/${test_name}.c)
        target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
        add_test(${test_name} ${test_name})
    endfunction()

    custom_c_test(main_c)

endif()