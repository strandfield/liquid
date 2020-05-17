
enable_testing()

if(NOT DEFINED CACHE{LIQUID_BUILD_TESTS})
  set(LIQUID_BUILD_TESTS ON CACHE BOOL "whether to build liquid tests")
endif()

if(LIQUID_BUILD_TESTS)

  configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/googletest-download")
  execute_process(COMMAND "${CMAKE_COMMAND}" --build .
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/googletest-download" )
  
  set(GTEST_DIR "${CMAKE_BINARY_DIR}/googletest-src/googletest" CACHE PATH "Root directory for GoogleTest")
  
  add_executable(TEST_liquid tests.cpp ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
  add_dependencies(TEST_liquid liquid)
  target_include_directories(TEST_liquid PUBLIC "${GTEST_DIR}/include")
  target_include_directories(TEST_liquid PRIVATE "${GTEST_DIR}")
  target_include_directories(TEST_liquid PUBLIC "../include")
  target_link_libraries(TEST_liquid liquid)

  if (NOT DEFINED WIN32)
    target_link_libraries(TEST_liquid pthread)
  endif()

  if (WIN32)
    set_target_properties(TEST_liquid PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set_target_properties(TEST_liquid PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  endif()

endif()