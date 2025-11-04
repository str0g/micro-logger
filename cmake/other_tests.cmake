function(custom_test_app test_name)
    add_executable(${test_name} tests/${test_name}.cpp)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
endfunction()


function(custom_c_test test_name)
    add_executable(${test_name} ../tests/${test_name}.c)
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
endfunction()
