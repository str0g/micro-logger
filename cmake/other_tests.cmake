function(custom_test_app test_name)
    add_executable(${test_name} demos/${test_name}.cpp)
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread)
endfunction()

function(custom_app_test_c test_name)
    add_executable(${test_name}_c demos/${test_name}.c)
    target_link_libraries(${test_name}_c LINK_PUBLIC ${PROJECT_NAME} -lpthread)
endfunction()
