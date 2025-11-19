cmake_minimum_required(VERSION 3.15)

project(homework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE CORE_SOURCES ${SRC_DIR}/*.cpp)
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/src/gui/.*")

add_executable(${PROJECT_NAME} ${CORE_SOURCES} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(GUI_SOURCES
    src/gui/main_qt.cpp
    src/gui/MainWindow.cpp
    src/gui/AddPostDialog.cpp
)

add_executable(snas_gui
    ${CORE_SOURCES}
    ${GUI_SOURCES}
)

target_include_directories(snas_gui PRIVATE ${INCLUDE_DIR} src/gui)
target_link_libraries(snas_gui PRIVATE Qt5::Widgets)
