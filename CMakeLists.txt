cmake_minimum_required(VERSION 3.5)
project(micro_logger VERSION 0.1 LANGUAGES CXX)

option(WITH_TESTS "Enable unit tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(includes)
aux_source_directory(src SRC)

add_library(${PROJECT_NAME} SHARED ${SRC})

if(WITH_TESTS)
enable_testing()
function(custom_test test_name)
    add_executable(${test_name} tests/${test_name}.cpp)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
    add_test(${test_name} ${test_name})
endfunction()

custom_test(main)

endif()
