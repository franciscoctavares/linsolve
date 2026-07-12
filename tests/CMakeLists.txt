add_executable(unit_tests)

target_sources(unit_tests
    PRIVATE
        matrix.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    linsolve_lib
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(unit_tests)