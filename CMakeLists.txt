cmake_minimum_required(VERSION 3.15)

project(homework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})