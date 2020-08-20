cmake_minimum_required(VERSION 2.8)

#project(${PROJECT_NAME})

add_library(JsonParser Json.cpp)

add_executable(JsonParser-main main.cpp)

target_link_libraries(JsonParser-main JsonParser)
