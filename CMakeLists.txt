cmake_minimum_required(VERSION 3.14.4)

set(CMAKE_CXX_STANDARD 20)

project(vbscript_preprocessor)

add_executable(${PROJECT_NAME} source/main.cpp)

add_subdirectory(libutilities)
add_subdirectory(libcommandline)

get_target_property(
    LIBCOMMANDLINE_INCLUDE_DIRECTORIES
    libcommandline
    INCLUDE_DIRECTORIES)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${LIBCOMMANDLINE_INCLUDE_DIRECTORIES})

target_link_libraries(
    ${PROJECT_NAME}
    libcommandline)
