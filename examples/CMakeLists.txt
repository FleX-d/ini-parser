cmake_minimum_required(VERSION 3.0)
project(iniparser-example1)

add_executable(${PROJECT_NAME} src/example1.cpp)
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion)
#target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} iniparser)
