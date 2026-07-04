#include "catch2/catch_test_macros.hpp"
#include "matrix.hpp"
#include <vector>

static std::vector<double> m1 = {1, 2, 3, 4};
static std::vector<double> m2 = {1, 2, 3, 4, 5, 6};
static Matrix a(m1, 2, 2);
static Matrix b(m2, 3, 2);
static Matrix c(m2, 2, 3);

TEST_CASE("Matrix multiplication (A * B) requires A_columns = B_rows", "[Matrix][Multiplication]") {
    REQUIRE_THROWS(a * b);
    REQUIRE_NOTHROW(a * c);
}

TEST_CASE("Matrix multiplication produces the correct result", "[Matrix][Multiplication]") {
    Matrix multResult1 = a * c;

    multResult1.displayMatrix();

    REQUIRE(multResult1.getNRows() == 2);
    REQUIRE(multResult1.getNColumns() == 3);
    REQUIRE(multResult1.getElement(0, 0) == 9);
    REQUIRE(multResult1.getElement(0, 1) == 12);
    REQUIRE(multResult1.getElement(0, 2) == 15);
    REQUIRE(multResult1.getElement(1, 0) == 19);
    REQUIRE(multResult1.getElement(1, 1) == 26);
    REQUIRE(multResult1.getElement(1, 2) == 33);
}