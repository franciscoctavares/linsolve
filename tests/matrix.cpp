#include "catch2/catch_test_macros.hpp"
#include "matrix.hpp"
#include <vector>

static std::vector<double> m1 = {1, 2, 3, 4};
static std::vector<double> m2 = {1, 2, 3, 4, 5, 6};
static std::vector<double> m3 = {1, 2, 3, 4, 5, 6, 7, 8, 9};

TEST_CASE("Matrix::Matrix()", "[Matrix][Constructor]") {

    SECTION("Constructor does not throw exceptions") {
        REQUIRE_NOTHROW(Matrix(m1, 2, 2));
    }

    SECTION("Constructor stores matrix dimensions correctly") {
        Matrix m(m1, 2, 2);

        REQUIRE(m.getNRows() == 2);
        REQUIRE(m.getNColumns() == 2);
        REQUIRE(m.getElements() == m1);
    }

    SECTION("Constructor throws on invalid rows") {
        REQUIRE_THROWS(Matrix(m1, -2, 2));
    }

    SECTION("Constructor throws on invalid columns") {
        REQUIRE_THROWS(Matrix(m1, 2, -2));
    }

    SECTION("Constructor throws on invalid matrix size (rows * columns)") {
        REQUIRE_THROWS(Matrix(m1, 2, 3));
    }

}

TEST_CASE("Matrix::operator+()", "[Matrix][operator+]") {
    Matrix a(m1, 2, 2);
    Matrix b(m1, 2, 2);
    Matrix c(m2, 2, 3);
    Matrix d(m2, 3, 2);
    Matrix e(m3, 3, 3);

    
    SECTION("Matrix::operator+() requires both matrices to have the same dimensions") {
        REQUIRE_THROWS(a + c);
        REQUIRE_THROWS(a + d);
        REQUIRE_THROWS(a + e);
    }

    SECTION("Matrix::operator+() produces the correct result") {
        REQUIRE_NOTHROW(a + b);
        Matrix additionResult = a + b;

        REQUIRE(additionResult.getElements()[0] == 2);
        REQUIRE(additionResult.getElements()[1] == 4);
        REQUIRE(additionResult.getElements()[2] == 6);
        REQUIRE(additionResult.getElements()[3] == 8);
        REQUIRE(additionResult.getNRows() == 2);
        REQUIRE(additionResult.getNColumns() == 2);        
    }

}

TEST_CASE("Matrix::operator+=()", "[Matrix][operator+=]") {
    Matrix a(m1, 2, 2);
    Matrix c(m2, 2, 3);
    Matrix d(m2, 3, 2);
    Matrix e(m3, 3, 3);

    SECTION("Matrix::operator+=() requires both matrices to have the same dimensions") {
        REQUIRE_THROWS(a += c);
        REQUIRE_THROWS(a += d);
        REQUIRE_THROWS(a += e);
    }

    SECTION("Matrix::operator+=() produces the correct result") {
        Matrix a(m1, 2, 2);
        Matrix b(m1, 2, 2);

        REQUIRE_NOTHROW(a += b);

        REQUIRE(a.getElement(0, 0) == 2);
        REQUIRE(a.getElement(0, 1) == 4);
        REQUIRE(a.getElement(1, 0) == 6);
        REQUIRE(a.getElement(1, 1) == 8);
        REQUIRE(a.getNRows() == 2);
        REQUIRE(a.getNColumns() == 2);
    }

}

TEST_CASE("Matrix::operator-()", "[Matrix][operator-]") {
    Matrix a(m1, 2, 2);
    Matrix b(m1, 2, 2);
    Matrix c(m2, 2, 3);
    Matrix d(m2, 3, 2);
    Matrix e(m3, 3, 3);

    SECTION("Matrix::operator-() requires both matrices to have the same dimensions") {
        REQUIRE_THROWS(a - c);
        REQUIRE_THROWS(a - d);
        REQUIRE_THROWS(a - e);
    }

    SECTION("Matrix::operator-() produces the correct result") {
        REQUIRE_NOTHROW(a - b);
        Matrix subtractionResult = a - b;

        REQUIRE(subtractionResult.getElements()[0] == 0);
        REQUIRE(subtractionResult.getElements()[1] == 0);
        REQUIRE(subtractionResult.getElements()[2] == 0);
        REQUIRE(subtractionResult.getElements()[3] == 0);
        REQUIRE(subtractionResult.getNRows() == 2);
        REQUIRE(subtractionResult.getNColumns() == 2);
    }

}

TEST_CASE("Matrix::operator-=()", "[Matrix][operator-=]") {
    Matrix a(m1, 2, 2);
    Matrix b(m1, 2, 2);
    Matrix c(m2, 2, 3);
    Matrix d(m2, 3, 2);
    Matrix e(m3, 3, 3);

    SECTION("Matrix::operator-=() requires both matrices to have the same dimensions") {
        REQUIRE_THROWS(a -= c);
        REQUIRE_THROWS(a -= d);
        REQUIRE_THROWS(a -= e);
    }

    SECTION("Matrix::operator-=() produces the correct result") {
        REQUIRE_NOTHROW(a -= b);

        REQUIRE(a.getElement(0, 0) == 0);
        REQUIRE(a.getElement(0, 1) == 0);
        REQUIRE(a.getElement(1, 0) == 0);
        REQUIRE(a.getElement(1, 1) == 0);
        REQUIRE(a.getNRows() == 2);
        REQUIRE(a.getNColumns() == 2);
    }

}

TEST_CASE("Matrix::operator*()", "[Matrix][operator*]") {
    Matrix a(m1, 2, 2);
    Matrix b(m2, 3, 2);
    Matrix c(m2, 2, 3);

    SECTION("Matrix::operator*() (A * B) requires A_columns = B_rows") {
        REQUIRE_THROWS(a * b);
        REQUIRE_NOTHROW(a * c);
    }

    SECTION("Matrix::operator*() produces the correct result") {
        Matrix multResult1 = a * c;

        REQUIRE(multResult1.getNRows() == 2);
        REQUIRE(multResult1.getNColumns() == 3);
        REQUIRE(multResult1.getElement(0, 0) == 9);
        REQUIRE(multResult1.getElement(0, 1) == 12);
        REQUIRE(multResult1.getElement(0, 2) == 15);
        REQUIRE(multResult1.getElement(1, 0) == 19);
        REQUIRE(multResult1.getElement(1, 1) == 26);
        REQUIRE(multResult1.getElement(1, 2) == 33);
    }

}

TEST_CASE("Matrix::getRow()", "[Matrix][getRow]") {
    Matrix a(m1, 2, 2);

    SECTION("Matrix::getRow() throws an exception with invalid row indexes") {
        REQUIRE_THROWS(a.getRow(-1));
        REQUIRE_THROWS(a.getRow(2));

        REQUIRE_NOTHROW(a.getRow(0));
        REQUIRE_NOTHROW(a.getRow(1));
    }

    SECTION("Matrix::getRow() produces the correct result") {
        Matrix firstRow = a.getRow(0);

        REQUIRE(firstRow.getNRows() == 1);
        REQUIRE(firstRow.getNColumns() == 2);
        REQUIRE(firstRow.getElement(0, 0) == 1);
        REQUIRE(firstRow.getElement(0, 1) == 2);
    }

}

TEST_CASE("Matrix::getColumn()", "[Matrix][getColumn]") {
    Matrix a(m1, 2, 2);

    SECTION("Matrix::getColumn() throws an exception with invalid row indexes") {
        REQUIRE_THROWS(a.getColumn(-1));
        REQUIRE_THROWS(a.getColumn(2));

        REQUIRE_NOTHROW(a.getColumn(0));
        REQUIRE_NOTHROW(a.getColumn(1));
    }

    SECTION("Matrix::getColumn() produces the correct result") {
        Matrix firstColumn = a.getColumn(0);

        REQUIRE(firstColumn.getNRows() == 2);
        REQUIRE(firstColumn.getNColumns() == 1);
        REQUIRE(firstColumn.getElement(0, 0) == 1);
        REQUIRE(firstColumn.getElement(1, 0) == 3);
    }

}

TEST_CASE("Matrix::rowOperation()", "[Matrix][rowOperation]") {

    SECTION("Matrix::rowOperation() throws an exception when sourceRow is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.rowOperation(-1, 0, 2));
        REQUIRE_THROWS(a.rowOperation(2, 0, 2));

        REQUIRE_NOTHROW(a.rowOperation(0, 0, 2));
        REQUIRE_NOTHROW(a.rowOperation(1, 0, 2));
    }

    SECTION("Matrix::rowOperation() throws an exception when targetRow is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.rowOperation(0, -1, 2));
        REQUIRE_THROWS(a.rowOperation(0, 2, 2));

        REQUIRE_NOTHROW(a.rowOperation(0, 0, 2));
        REQUIRE_NOTHROW(a.rowOperation(0, 1, 2));
    }

    SECTION("Matrix::rowOperation() produces the correct result") {
        Matrix a(m1, 2, 2);
        a.rowOperation(0, 1, 2);
        REQUIRE(a.getElement(0, 0) == 1);
        REQUIRE(a.getElement(0, 1) == 2);
        REQUIRE(a.getElement(1, 0) == 5);
        REQUIRE(a.getElement(1, 1) == 8);
    }

}

TEST_CASE("Matrix::columnOperation()", "[Matrix][columnOperation]") {

    SECTION("Matrix::columnOperation() throws an exception when sourceColumn is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.columnOperation(-1, 0, 2));
        REQUIRE_THROWS(a.columnOperation(2, 0, 2));

        REQUIRE_NOTHROW(a.columnOperation(0, 0, 2));
        REQUIRE_NOTHROW(a.columnOperation(1, 0, 2));
    }

    SECTION("Matrix::columnOperation() throws an exception when targetColumn is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.columnOperation(0, -1, 2));
        REQUIRE_THROWS(a.columnOperation(0, 2, 2));

        REQUIRE_NOTHROW(a.columnOperation(0, 0, 2));
        REQUIRE_NOTHROW(a.columnOperation(0, 1, 2));
    }

    SECTION("Matrix::rowOperation() produces the correct result") {
        Matrix a(m1, 2, 2);
        a.columnOperation(0, 1, 2);
        REQUIRE(a.getElement(0, 0) == 1);
        REQUIRE(a.getElement(0, 1) == 4);
        REQUIRE(a.getElement(1, 0) == 3);
        REQUIRE(a.getElement(1, 1) == 10);
    }

}

