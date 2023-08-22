

# https://cmake.org/cmake/help/latest/module/GoogleTest.html
include(GoogleTest)

file(GLOB_RECURSE CHESS_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*test.cpp")


###################### "make check-tests" ######################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

###################### "make xyz-test" #########################
foreach (chess_test_source ${CHESS_TEST_SOURCES})
  # Create a human readable name.
  get_filename_component(chess_test_filename ${chess_test_source} NAME) # just in case it's in a sub folder
  string(REPLACE ".cpp" "" chess_test_name ${chess_test_filename})
  
  # Add the test target separately and as part of "make check-tests".
  add_executable(${chess_test_name} EXCLUDE_FROM_ALL ${chess_test_source})

  add_dependencies(build-tests ${chess_test_name})
  add_dependencies(check-tests ${chess_test_name})

  gtest_discover_tests(${chess_test_name}
          EXTRA_ARGS
          --gtest_color=yes
          --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${chess_test_name}.xml
          --gtest_catch_exceptions=0
          DISCOVERY_TIMEOUT 120
          PROPERTIES
          TIMEOUT 120
          )

  target_link_libraries(${chess_test_name} chess gtest gmock_main)

  # Set test target properties and dependencies.
  set_target_properties(${chess_test_name}
          PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
          COMMAND ${chess_test_name}
          )
endforeach ()