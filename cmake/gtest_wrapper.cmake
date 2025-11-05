include(GoogleTest)

function(custom_gtest test_name)
    add_executable(${test_name} tests/${test_name}.cpp)
    target_link_libraries(${test_name} LINK_PUBLIC ${PROJECT_NAME} -lpthread -lgtest -lgtest_main)
    target_include_directories(${test_name}
      PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    gtest_add_tests(
      TARGET ${test_name}
    )
endfunction()
