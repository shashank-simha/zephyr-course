cmake_minimum_required(VERSION 3.20.0)

get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
list(APPEND BOARD_ROOT "${PROJECT_ROOT}")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_zephyr_app)

add_subdirectory(drivers)

target_sources(app PRIVATE src/main.cpp)
