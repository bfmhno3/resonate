cmake_minimum_required(VERSION 3.30.5)
project(resonate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 开启对 MOC, UIC, RCC 的支持
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui")

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    Network
    Sql
)

add_executable(${CMAKE_PROJECT_NAME}
    src/main.cpp
    src/gui/main_window.cpp
    include/gui/main_window.hpp
    ui/main_window.ui
    src/utils/logger.cpp
    src/gui/sidebar.cpp
    include/gui/sidebar.hpp
    ui/sidebar.ui
    src/gui/header.cpp
    include/gui/header.hpp
    ui/header.ui
    src/gui/player_bar.cpp
    include/gui/player_bar.hpp
    ui/player_bar.ui
)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual
        -Wnull-dereference -Wformat=2 -Wimplicit-fallthrough -Wno-unused-parameter
        $<$<CONFIG:Debug>: -Og -g3 -fno-omit-frame-pointer>
        $<$<CONFIG:Release>: -O2 -DNDEBUG>
    >
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::Network
    Qt5::Sql
)
