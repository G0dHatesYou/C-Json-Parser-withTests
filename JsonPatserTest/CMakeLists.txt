# CMakeList.txt : CMake project for JsonPatserTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

  add_subdirectory(./gtest-1.6.0)
	
  enable_testing()

  # Include the gtest library.

  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(${PROJECT_SOURCE_DIR}/JsonParser)

  add_executable (JsonPatserTest "JsonPatserTest.cpp")

  # Standard linking to gtest stuff.
  target_link_libraries(JsonPatserTest gtest gtest_main)

  # Extra linking for the project.
  target_link_libraries(JsonPatserTest JsonParser)


