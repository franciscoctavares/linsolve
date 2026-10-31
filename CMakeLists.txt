cmake_minimum_required(VERSION 3.20)

project(linsolve)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory(external/Catch2)
add_subdirectory(external/tabulate)
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
    external/tabulate
)

target_compile_options(linsolve_lib
    PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wno-unused-variable
)

add_executable(linsolve
    src/main.cpp
)

target_link_libraries(linsolve
    PRIVATE
    linsolve_lib
)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(linsolve_lib PRIVATE
        --coverage
        -O0
        -g
    )

    target_compile_options(unit_tests PRIVATE
        --coverage
        -O0
        -g
    )

    target_link_options(unit_tests PRIVATE
        --coverage
    )

    target_link_options(linsolve PRIVATE
        --coverage
    )

    find_program(GCOVR_EXECUTABLE gcovr REQUIRED)

    set(COVERAGE_DIR "${CMAKE_SOURCE_DIR}/docs/coverage")

    file(MAKE_DIRECTORY "${COVERAGE_DIR}")

    configure_file(
        "${CMAKE_SOURCE_DIR}/CTestCustom.cmake.in"
        "${CMAKE_BINARY_DIR}/CTestCustom.cmake"
        @ONLY
    )
endif()