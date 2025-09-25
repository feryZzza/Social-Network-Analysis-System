cmake_minimum_required(VERSION 3.15)

project(homework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
# 从控制台目标源文件中过滤掉 Qt UI 源文件
list(FILTER SRC_FILES EXCLUDE REGEX ".*/src/ui/.*")
file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${SRC_FILES} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# GUI target using Qt5 Widgets
add_executable(homework_gui
    src/ui/qt_main.cpp
    src/ui/mainwindow.cpp
    src/ui/models/clients_table_model.cpp
    src/ui/models/posts_table_model.cpp
    src/ui/delegates/post_item_delegate.cpp
    src/ui/resources.qrc
    ${SRC_FILES}
)

target_include_directories(homework_gui PRIVATE ${INCLUDE_DIR})
target_link_libraries(homework_gui PRIVATE Qt5::Widgets)
target_include_directories(homework_gui PRIVATE ${CMAKE_SOURCE_DIR}/src)