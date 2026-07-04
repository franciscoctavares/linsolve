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

TEST_CASE("Matrix addition requires both matrices have the same dimensions", "[Matrix][Addition]") {
    REQUIRE_THROWS(a + c);
    REQUIRE_THROWS(a + d);
    REQUIRE_THROWS(a + e);
}

TEST_CASE("Matrix addition produces the correct result", "[Matrix][Addition]") {
    REQUIRE_NOTHROW(a + b);
    Matrix additionResult = a + b;

    REQUIRE(additionResult.getElements()[0] == 2);
    REQUIRE(additionResult.getElements()[1] == 4);
    REQUIRE(additionResult.getElements()[2] == 6);
    REQUIRE(additionResult.getElements()[3] == 8);
    REQUIRE(additionResult.getNRows() == 2);
    REQUIRE(additionResult.getNColumns() == 2);
}

TEST_CASE("Matrix addition assignment (+=) requires both matrices have the same dimensions", "[Matrix][Addition]") {
    REQUIRE_THROWS(a += c);
    REQUIRE_THROWS(a += d);
    REQUIRE_THROWS(a += e);
}

TEST_CASE("Matrix addition assignment (+=) produces the correct result", "[Matrix][Addition]") {
    REQUIRE_NOTHROW(a += b);

    REQUIRE(a.getElement(0, 0) == 2);
    REQUIRE(a.getElement(0, 1) == 4);
    REQUIRE(a.getElement(1, 0) == 6);
    REQUIRE(a.getElement(1, 1) == 8);
    REQUIRE(a.getNRows() == 2);
    REQUIRE(a.getNColumns() == 2);
}


