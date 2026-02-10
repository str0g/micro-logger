project(micro_logger++ VERSION 0.31 LANGUAGES CXX)

aux_source_directory(src SRC)
add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/micro_logger>
)

if(MICRO_LOGGER_BUILD_TESTS)
  custom_gtest(test_hex)
  custom_gtest(test_to_string)
  custom_gtest(test_pattern)
  custom_gtest(test_custom_parameters)
endif()

if(MICRO_LOGGER_BUILD_DEMOS)
  custom_test_app(demo)
  custom_test_app(benchmark)
endif()

configure_file(../package/micro_logger.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${INCLUDE_INSTALL_DIR})
