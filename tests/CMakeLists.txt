add_executable(unit_tests)

add_subdirectory(matrix)

target_link_libraries(unit_tests
    PRIVATE
    linsolve_lib
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(unit_tests)