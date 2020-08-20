# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("JsonPatserTest")

set(CMAKE_CXX_FLAGS "-g -Wall")
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Build yamp-cpp tests")

add_subdirectory ("JsonPatserTest")
add_subdirectory ("JsonParser")
