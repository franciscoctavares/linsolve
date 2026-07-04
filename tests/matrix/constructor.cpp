#include "catch2/catch_test_macros.hpp"
#include "matrix.hpp"
#include <vector>

static std::vector<double> validElements = {1, 2, 3, 4};

TEST_CASE("Constructor builds valid Matrix", "[Matrix][Constructor]") {
    REQUIRE_NOTHROW(Matrix(validElements, 2, 2));
}

TEST_CASE("Matrix dimensions are stored correctly", "[Matrix][Constructor]") {
    Matrix m(validElements, 2, 2);

    REQUIRE(m.getNRows() == 2);
    REQUIRE(m.getNColumns() == 2);
    REQUIRE(m.getElements() == validElements);
}

TEST_CASE("Matrix constructor throws on invalid rows", "[Matrix][Constructor]") {
    REQUIRE_THROWS(Matrix(validElements, -2, 2));
}

TEST_CASE("Matrix constructor throws on invalid columns", "[Matrix][Constructor]") {
    REQUIRE_THROWS(Matrix(validElements, 2, -2));
}

TEST_CASE("Matrix constructor throws on invalid matrix size (rows * columns)", "[Matrix][Constructor]") {
    REQUIRE_THROWS(Matrix(validElements, 2, 3));
}
