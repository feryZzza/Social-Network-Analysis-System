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

# 公开 include 目录给所有 target（尤其包含 include/ui 下的头）
include_directories(${INCLUDE_DIR})

# GUI target using Qt5 Widgets
add_executable(homework_gui
    src/ui/qt_main.cpp
    src/ui/mainwindow.cpp
    src/ui/models/clients_table_model.cpp
    src/ui/models/posts_table_model.cpp
    src/ui/delegates/post_item_delegate.cpp
    src/ui/resources.qrc
    ${SRC_FILES}
    # 显式加入包含 Q_OBJECT 的头文件，确保 AUTOMOC 处理
    ${CMAKE_SOURCE_DIR}/include/ui/mainwindow.h
    ${CMAKE_SOURCE_DIR}/include/ui/models/clients_table_model.h
    ${CMAKE_SOURCE_DIR}/include/ui/models/posts_table_model.h
    ${CMAKE_SOURCE_DIR}/include/ui/delegates/post_item_delegate.h
)

# GUI 目标包含公共 include 目录
target_include_directories(homework_gui PRIVATE ${INCLUDE_DIR})
target_link_libraries(homework_gui PRIVATE Qt5::Widgets)
# 如需访问 src 里局部头（例如 models 下的 .h 已在 include/models，保留这行仅作为安全网）
target_include_directories(homework_gui PRIVATE ${CMAKE_SOURCE_DIR}/src)