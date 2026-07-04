#include "catch2/catch_test_macros.hpp"
#include "matrix.hpp"
#include <vector>

static std::vector<double> m1 = {1, 2, 3, 4};
static std::vector<double> m2 = {1, 2, 3, 4, 5, 6};
static std::vector<double> m3 = {1, 2, 3, 4, 5, 6, 7, 8, 9};

static Matrix a(m1, 2, 2);
static Matrix b(m1, 2, 2);
static Matrix c(m2, 2, 3);
static Matrix d(m2, 3, 2);
static Matrix e(m3, 3, 3);

TEST_CASE("Matrix subtraction requires both matrices have the same dimensions", "[Matrix][Subtraction]") {
    REQUIRE_THROWS(a - c);
    REQUIRE_THROWS(a - d);
    REQUIRE_THROWS(a - e);
}

TEST_CASE("Matrix subtraction produces the correct result", "[Matrix][Subtraction]") {
    REQUIRE_NOTHROW(a - b);
    Matrix subtractionResult = a - b;

    REQUIRE(subtractionResult.getElements()[0] == 0);
    REQUIRE(subtractionResult.getElements()[1] == 0);
    REQUIRE(subtractionResult.getElements()[2] == 0);
    REQUIRE(subtractionResult.getElements()[3] == 0);
    REQUIRE(subtractionResult.getNRows() == 2);
    REQUIRE(subtractionResult.getNColumns() == 2);
}

TEST_CASE("Matrix subtraction assignment (-=) requires both matrices have the same dimensions", "[Matrix][Subtraction]") {
    REQUIRE_THROWS(a -= c);
    REQUIRE_THROWS(a -= d);
    REQUIRE_THROWS(a -= e);
}

TEST_CASE("Matrix subtraction assignment (-=) produces the correct result", "[Matrix][Subtraction]") {
    REQUIRE_NOTHROW(a -= b);

    REQUIRE(a.getElement(0, 0) == 0);
    REQUIRE(a.getElement(0, 1) == 0);
    REQUIRE(a.getElement(1, 0) == 0);
    REQUIRE(a.getElement(1, 1) == 0);
    REQUIRE(a.getNRows() == 2);
    REQUIRE(a.getNColumns() == 2);
}