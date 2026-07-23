cmake_minimum_required(VERSION 3.20)

project(linsolve)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory(external/Catch2)
add_subdirectory(tests)

file(GLOB_RECURSE LINSOLVE_SOURCES
    src/*.cpp
)

list(REMOVE_ITEM LINSOLVE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_library(linsolve_lib
    ${LINSOLVE_SOURCES}
)

target_include_directories(linsolve_lib
    PUBLIC
    src
)

target_compile_options(linsolve_lib
    PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(linsolve
    src/main.cpp
)

target_link_libraries(linsolve
    PRIVATE
    linsolve_lib
)