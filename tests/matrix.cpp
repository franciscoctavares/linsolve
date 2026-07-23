#include "catch2/catch_test_macros.hpp"
#include "matrix.hpp"
#include <vector>

static std::vector<double> m1 = {1, 2, 3, 4};
static std::vector<double> m2 = {1, 2, 3, 4, 5, 6};
static std::vector<double> m3 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
static std::vector<double> m4 = {2, 4, 6, 8, 10, 12, 14, 16, 18};
static std::vector<double> m5 = {2, 3, 4, 5};

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

TEST_CASE("Matrix::dotProduct()", "[Matrix][dotProduct]") {

    SECTION("Matrix::dotProduct() throws an exception when either of the Matrices are not row or column vectors") {
        Matrix a(m1, 2, 2);
        Matrix b(m1, 1, 4);
        REQUIRE_THROWS(a.dotProduct(b));

        Matrix c(m1, 1, 4);
        Matrix d(m1, 2, 2);
        REQUIRE_THROWS(c.dotProduct(d));

        Matrix e(m1, 2, 2);
        Matrix f(m1, 2, 2);
        REQUIRE_THROWS(e.dotProduct(f));

        Matrix g(m1, 1, 4);
        Matrix h(m1, 1, 4);
        REQUIRE_NOTHROW(g.dotProduct(h));
    }

    SECTION("Matrix::dotProduct() throws an exception when both row/column matrices don't have the same dimentions") {
        Matrix a(m1, 1, 4);
        Matrix b(m2, 1, 6);
        REQUIRE_THROWS(a.dotProduct(b));
        REQUIRE_NOTHROW(a.dotProduct(a));
    }

    SECTION("Matrix::dotProduct() produces the correct result") {
        Matrix a(m1, 1, 4);
        REQUIRE(a.dotProduct(a) == 30);
    }

}

TEST_CASE("Matrix::getElement()", "[Matrix][getElement]") {

    SECTION("Matrix::getElement() throws an exception if the row argument is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.getElement(-1, 0));
        REQUIRE_THROWS(a.getElement(2, 0));
        REQUIRE_NOTHROW(a.getElement(0, 0));
        REQUIRE_NOTHROW(a.getElement(1, 0));
    }

    SECTION("Matrix::getElement() throws an exception if the column argument is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.getElement(0, -1));
        REQUIRE_THROWS(a.getElement(0, 2));
        REQUIRE_NOTHROW(a.getElement(0, 0));
        REQUIRE_NOTHROW(a.getElement(0, 1));
    }

    SECTION("Matrix::getElement() returns the correct element") {
        Matrix a(m1, 2, 2);
        REQUIRE(a.getElement(0, 0) == 1);
        REQUIRE(a.getElement(0, 1) == 2);
        REQUIRE(a.getElement(1, 0) == 3);
        REQUIRE(a.getElement(1, 1) == 4);
    }

}

TEST_CASE("Matrix::setElement()", "[Matrix][setElement]") {

    SECTION("Matrix::setElement() throws an exception if the row argument is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.setElement(-1, 0, 2));
        REQUIRE_THROWS(a.setElement(2, 0, 2));
        REQUIRE_NOTHROW(a.setElement(0, 0, 2));
        REQUIRE_NOTHROW(a.setElement(1, 0, 2));
    }

    SECTION("Matrix::setElement() throws an exception if the column argument is out of bounds") {
        Matrix a(m1, 2, 2);
        REQUIRE_THROWS(a.setElement(0, -1, 2));
        REQUIRE_THROWS(a.setElement(0, 2, 2));
        REQUIRE_NOTHROW(a.setElement(0, 0, 2));
        REQUIRE_NOTHROW(a.setElement(0, 1, 2));
    }

    SECTION("Matrix::setElement() produces the correct result") {
        Matrix a(m1, 2, 2);
        a.setElement(0, 0, 2);
        REQUIRE(a.getElement(0, 0) == 2);
        REQUIRE(a.getElement(0, 1) == 2);
        REQUIRE(a.getElement(1, 0) == 3);
        REQUIRE(a.getElement(1, 1) == 4);
    }

}

TEST_CASE("Matrix::stackVertical()", "[Matrix][stackVertical]") {

    SECTION("Matrix::stackVertical() throws an exception if both matrices don't have the same number of columns") {
        Matrix a(m1, 2, 2);
        Matrix b(m2, 2, 3);
        REQUIRE_THROWS(a.stackVertical(b));
        Matrix c(m2, 3, 2);
        REQUIRE_NOTHROW(a.stackVertical(c));
    }

    SECTION("Matrix::stackVertical() produces the correct result") {
        Matrix a(m1, 2, 2);
        Matrix b(m2, 3, 2);
        a.stackVertical(b);
        REQUIRE(a.getElement(0, 0) == 1);
        REQUIRE(a.getElement(0, 1) == 2);
        REQUIRE(a.getElement(1, 0) == 3);
        REQUIRE(a.getElement(1, 1) == 4);
        REQUIRE(a.getElement(2, 0) == 1);
        REQUIRE(a.getElement(2, 1) == 2);
        REQUIRE(a.getElement(3, 0) == 3);
        REQUIRE(a.getElement(3, 1) == 4);
        REQUIRE(a.getElement(4, 0) == 5);
        REQUIRE(a.getElement(4, 1) == 6);
    }

}

TEST_CASE("Matrix::stackHorizontal()", "[Matrix][stackHorizontal]") {

    SECTION("Matrix::stackHorizontal() throws an exception if both matrices don't have the same number of rows") {
        Matrix a(m1, 2, 2);
        Matrix b(m2, 3, 2);
        REQUIRE_THROWS(a.stackHorizontal(b));
        Matrix c(m2, 2, 3);
        REQUIRE_NOTHROW(a.stackHorizontal(c));
    }

    SECTION("Matrix::stackHorizontal() produces the correct result") {
        Matrix a(m1, 2, 2);
        Matrix b(m2, 2, 3);
        a.stackHorizontal(b);
        REQUIRE(a.getElement(0, 0) == 1);
        REQUIRE(a.getElement(0, 1) == 2);
        REQUIRE(a.getElement(0, 2) == 1);
        REQUIRE(a.getElement(0, 3) == 2);
        REQUIRE(a.getElement(0, 4) == 3);
        REQUIRE(a.getElement(1, 0) == 3);
        REQUIRE(a.getElement(1, 1) == 4);
        REQUIRE(a.getElement(1, 2) == 4);
        REQUIRE(a.getElement(1, 3) == 5);
        REQUIRE(a.getElement(1, 4) == 6);
    }

}

TEST_CASE("Matrix::transpose()", "[Matrix][transpose]") {
    Matrix a(m2, 3, 2);
    Matrix b = a.transpose();

    SECTION("Matrix::transpose() produces the correct result") {
        REQUIRE(b.getNRows() == 2);
        REQUIRE(b.getNColumns() == 3);
        REQUIRE(b.getElement(0, 0) == 1);
        REQUIRE(b.getElement(0, 1) == 3);
        REQUIRE(b.getElement(0, 2) == 5);
        REQUIRE(b.getElement(1, 0) == 2);
        REQUIRE(b.getElement(1, 1) == 4);
        REQUIRE(b.getElement(1, 2) == 6);
    }

}

TEST_CASE("Matrix::operator*(double)", "[Matrix][operator*(double)]") {
    Matrix a(m1, 2, 2);
    Matrix b = a * 2;

    SECTION("Matrix::operator*(double) produces the correct result") {
        REQUIRE(b.getElement(0, 0) == 2);
        REQUIRE(b.getElement(0, 1) == 4);
        REQUIRE(b.getElement(1, 0) == 6);
        REQUIRE(b.getElement(1, 1) == 8);
    }

}

TEST_CASE("Matrix::operator*=(double)", "[Matrix][operator*=(double)]") {
    Matrix a(m1, 2, 2);
    a *= 2;

    SECTION("Matrix::operator*=(double) produces the correct result") {
        REQUIRE(a.getElement(0, 0) == 2);
        REQUIRE(a.getElement(0, 1) == 4);
        REQUIRE(a.getElement(1, 0) == 6);
        REQUIRE(a.getElement(1, 1) == 8);
    }

}

TEST_CASE("Matrix::maxValueIndex()", "[Matrix][maxValueIndex]") {
    Matrix a(m1, 2, 2);
    Matrix b(m2, 3, 2);
    Matrix c(m3, 3, 3);
    Matrix d(m4, 3, 3);

    SECTION("Matrix::maxValueIndex() produces the correct result") {
        REQUIRE(a.maxValueIndex() == 3);
        REQUIRE(b.maxValueIndex() == 5);
        REQUIRE(c.maxValueIndex() == 8);
        REQUIRE(d.maxValueIndex() == 8);
    }

}

TEST_CASE("Matrix::minValueIndex()", "[Matrix][minValueIndex]") {
    Matrix a(m1, 2, 2);
    Matrix b(m2, 3, 2);
    Matrix c(m3, 3, 3);
    Matrix d(m4, 3, 3);

    SECTION("Matrix::minValueIndex() produces the correct result") {
        REQUIRE(a.minValueIndex() == 0);
        REQUIRE(b.minValueIndex() == 0);
        REQUIRE(c.minValueIndex() == 0);
        REQUIRE(d.minValueIndex() == 0);
    }

}

TEST_CASE("Matrix::pointDivision()", "[Matrix][pointDivision]") {
    Matrix a(m1, 2, 2);
    Matrix b(m2, 3, 2);
    Matrix c(m3, 3, 3);
    Matrix d(m4, 3, 3);
    Matrix e(m2, 2, 3);

    SECTION("Matrix::pointDivision() throws an exception when both matrices don't have the same dimensions") {
        REQUIRE_THROWS(a.pointDivision(b));
        REQUIRE_THROWS(a.pointDivision(e));
        REQUIRE_THROWS(a.pointDivision(c));
        REQUIRE_NOTHROW(d.pointDivision(c));
    }

    SECTION("Matrix::pointDivision() produces the correct result") {
        Matrix g = d.pointDivision(c);
        REQUIRE(g.getNRows() == 3);
        REQUIRE(g.getNColumns() == 3);
        REQUIRE(g.getElement(0, 0) == 2);
        REQUIRE(g.getElement(0, 1) == 2);
        REQUIRE(g.getElement(0, 2) == 2);
        REQUIRE(g.getElement(1, 0) == 2);
        REQUIRE(g.getElement(1, 1) == 2);
        REQUIRE(g.getElement(1, 2) == 2);
        REQUIRE(g.getElement(2, 0) == 2);
        REQUIRE(g.getElement(2, 1) == 2);
        REQUIRE(g.getElement(2, 2) == 2);
    }

}

TEST_CASE("Matrix::setRow()", "[Matrix][setRow]") {
    Matrix a(m1, 2, 2);
    Matrix b(m2, 3, 2);
    Matrix c(m2, 2, 3);
    Matrix d(m5, 2, 2);
    
    SECTION("Matrix::setRow() throws an exception if the row argument is out of bounds") {
        REQUIRE_THROWS(a.setRow(-1, b, 1.0));
        REQUIRE_THROWS(a.setRow(2, b, 1.0));
        REQUIRE_NOTHROW(a.setRow(0, b, 1.0));
        REQUIRE_NOTHROW(a.setRow(1, b, 1.0));
    }

    SECTION("Matrix::setRow() throws an exception if both matrices don't have the same number of columns") {
        REQUIRE_THROWS(a.setRow(0, c, 1.0));
        REQUIRE_NOTHROW(a.setRow(0, b, 1.0));
    }

    SECTION("Matrix::setRow() produces the correct result") {
        a.setRow(0, d, 1.0);
        REQUIRE(a.getElement(0, 0) == 2);
        REQUIRE(a.getElement(0, 1) == 3);
        REQUIRE(a.getElement(1, 0) == 3);
        REQUIRE(a.getElement(1, 1) == 4);
    }

}

TEST_CASE("Matrix::setColumn()", "[Matrix][setColumn]") {
    Matrix a(m1, 2, 2);
    Matrix b(m2, 3, 2);
    Matrix c(m2, 2, 3);
    Matrix d(m5, 2, 2);
    
    SECTION("Matrix::setColumn() throws an exception if the column argument is out of bounds") {
        REQUIRE_THROWS(a.setColumn(-1, c));
        REQUIRE_THROWS(a.setColumn(2, c));
        REQUIRE_NOTHROW(a.setColumn(0, c));
        REQUIRE_NOTHROW(a.setColumn(1, c));
    }

    SECTION("Matrix::setColumn() throws an exception if both matrices don't have the same number of rows") {
        REQUIRE_THROWS(a.setColumn(0, b));
        REQUIRE_NOTHROW(a.setColumn(0, c));
    }

    SECTION("Matrix::setColumn() produces the correct result") {
        a.setColumn(0, d);
        REQUIRE(a.getElement(0, 0) == 2);
        REQUIRE(a.getElement(0, 1) == 2);
        REQUIRE(a.getElement(1, 0) == 4);
        REQUIRE(a.getElement(1, 1) == 4);
    }

}

TEST_CASE("Matrix::removeRow()", "[Matrix][removeRow]") {
    Matrix a(m1, 2, 2);

    SECTION("Matrix::removeRow() throws an exception if the row argument is out of bounds") {
        REQUIRE_THROWS(a.removeRow(-1));
        REQUIRE_THROWS(a.removeRow(2));
        REQUIRE_NOTHROW(a.removeRow(0));
        REQUIRE_NOTHROW(a.removeRow(1));
    }

    SECTION("Matrix::removeRow() produces the correct result") {
        Matrix b = a.removeRow(0);
        REQUIRE(b.getElement(0, 0) == 3);
        REQUIRE(b.getElement(0, 1) == 4);
        REQUIRE(b.getNRows() == 1);
        REQUIRE(b.getNColumns() == 2);
    }

}

TEST_CASE("Matrix::removeColumn()", "[Matrix][removeColumn]") {
    Matrix a(m1, 2, 2);
    Matrix b(m1, 2, 2);
    Matrix c(m1, 2, 2);
    Matrix d(m1, 2, 2);

    SECTION("Matrix::removeColumn() throws an exception if the column argument is out of bounds") {
        REQUIRE_THROWS(a.removeColumn(-1));
        REQUIRE_THROWS(b.removeColumn(2));
        REQUIRE_NOTHROW(c.removeColumn(0));
        REQUIRE_NOTHROW(d.removeColumn(1));
    }

    SECTION("Matrix::removeColumn() produces the correct result") {
        Matrix a(m1, 2, 2);
        //Matrix b = a.removeRow(0);
        a.removeColumn(0);
        REQUIRE(a.getElement(0, 0) == 2);
        REQUIRE(a.getElement(1, 0) == 4);
        REQUIRE(a.getNRows() == 2);
        REQUIRE(a.getNColumns() == 1);
    }

}

TEST_CASE("Matrix::findValueInVectorMatrix()", "[Matrix][findValueInVectorMatrix]") {
    Matrix a(m1, 2, 2);

    SECTION("Matrix::findValueInVectorMatrix() produces the correct result") {
        REQUIRE(a.findValueInVectorMatrix(1) == 0);
        REQUIRE(a.findValueInVectorMatrix(2) == 1);
        REQUIRE(a.findValueInVectorMatrix(3) == 2);
        REQUIRE(a.findValueInVectorMatrix(4) == 3);
        REQUIRE(a.findValueInVectorMatrix(-1) == -1);
    }

}

TEST_CASE("Matrix::subMatrix()", "[Matrix][subMatrix]") {
    Matrix a(m3, 3, 3);

    SECTION("Matrix::subMatrix() throws an exception if the startingRow argument is out of bounds") {
        REQUIRE_THROWS(a.subMatrix(-1, 1, 0, 1));
        REQUIRE_THROWS(a.subMatrix(3, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(1, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(1, 2, 0, 1));
    }

    SECTION("Matrix::subMatrix() throws an exception if the endingRow argument is out of bounds") {
        REQUIRE_THROWS(a.subMatrix(0, -1, 0, 1));
        REQUIRE_THROWS(a.subMatrix(0, 3, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 0, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 2, 0, 1));
    }

    SECTION("Matrix::subMatrix() throws an exception if startingRow > endingRow") {
        REQUIRE_THROWS(a.subMatrix(1, 0, 0, 1));
        REQUIRE_THROWS(a.subMatrix(2, 0, 0, 1));
        REQUIRE_THROWS(a.subMatrix(2, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 0, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 2, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(1, 2, 0, 1));
    }

    SECTION("Matrix::subMatrix() throws an exception if the startingColumn argument is out of bounds") {
        REQUIRE_THROWS(a.subMatrix(0, 1, -1, 1));
        REQUIRE_THROWS(a.subMatrix(0, 1, 3, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 1, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 1, 2));
    }

    SECTION("Matrix::subMatrix() throws an exception if the endingColumn argument is out of bounds") {
        REQUIRE_THROWS(a.subMatrix(0, 1, 0, -1));
        REQUIRE_THROWS(a.subMatrix(0, 1, 0, 3));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 0));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 2));
    }

    SECTION("Matrix::subMatrix() throws an exception if startingColumn > endingColumn") {
        REQUIRE_THROWS(a.subMatrix(0, 1, 1, 0));
        REQUIRE_THROWS(a.subMatrix(0, 1, 2, 0));
        REQUIRE_THROWS(a.subMatrix(0, 1, 2, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 0));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 1));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 0, 2));
        REQUIRE_NOTHROW(a.subMatrix(0, 1, 1, 2));
    }

    SECTION("Matrix::subMatrix() produces the correct result") {
        Matrix b = a.subMatrix(0, 0, 1, 2);
        REQUIRE(b.getNRows() == 1);
        REQUIRE(b.getNColumns() == 2);
        REQUIRE(b.getElement(0, 0) == 2);
        REQUIRE(b.getElement(0, 1) == 3);

        Matrix c = a.subMatrix(1, 2, 1, 2);
        REQUIRE(c.getNRows() == 2);
        REQUIRE(c.getNColumns() == 2);
        REQUIRE(c.getElement(0, 0) == 5);
        REQUIRE(c.getElement(0, 1) == 6);
        REQUIRE(c.getElement(1, 0) == 8);
        REQUIRE(c.getElement(1, 1) == 9);
    }

}

TEST_CASE("Matrix::isBasisVector()", "[Matrix][isBasisVector]") {
    std::vector<double> v1 = {1, 1, 3, 4};
    std::vector<double> v2 = {1, 0, 0, 0};
    Matrix a(v1, 2, 2);
    Matrix b(v2, 2, 2);

    SECTION("Matrix::isBasisVector() produces the correct result") {
        REQUIRE(a.isBasisVector() == -1);
        REQUIRE(b.isBasisVector() == 0);
    }

}

TEST_CASE("Matrix::operator==()", "[Matrix][operator==]") {
    Matrix a(m1, 2, 2);
    Matrix b(m1, 2, 2);
    Matrix c(m1, 1, 4);

    SECTION("Matrix::operator==() produces the correct result") {
        REQUIRE(a == b);
        REQUIRE(!(a == c));
    }

}

// non class methods

TEST_CASE("zeros()", "[Matrix][zeros]") {

    SECTION("zeros() throws an exception if the rows argument is non positive") {
        REQUIRE_THROWS(zeros(-1, 2));
        REQUIRE_THROWS(zeros(0, 2));
        REQUIRE_NOTHROW(zeros(1, 2));
    }

    SECTION("zeros() throws an exception if the columns argument is non positive") {
        REQUIRE_THROWS(zeros(2, -1));
        REQUIRE_THROWS(zeros(2, 0));
        REQUIRE_NOTHROW(zeros(2, 1));
    }

    SECTION("zeros() produces the correct result") {
        Matrix a = zeros(2, 2);
        REQUIRE(a.getNRows() == 2);
        REQUIRE(a.getNColumns() == 2);
        REQUIRE(a.getElement(0, 0) == 0);
        REQUIRE(a.getElement(0, 1) == 0);
        REQUIRE(a.getElement(1, 0) == 0);
        REQUIRE(a.getElement(1, 1) == 0);
    }

}

TEST_CASE("basisVector()", "[Matrix][basisVector]") {

    SECTION("basisVector() throws an exception if the size argument is non positive") {
        REQUIRE_THROWS(basisVector(-1, 0));
        REQUIRE_THROWS(basisVector(0, 0));
        REQUIRE_NOTHROW(basisVector(1, 0));
    }

    SECTION("basisVector() throws an exception if index < 0 or index >= size") {
        REQUIRE_THROWS(basisVector(1, -1));
        REQUIRE_THROWS(basisVector(1, 2));
        REQUIRE_NOTHROW(basisVector(1, 0)); 
    }

    SECTION("basisVector() produces the correct result") {
        Matrix a = basisVector(4, 1);
        REQUIRE(a.getNRows() == 4);
        REQUIRE(a.getNColumns() == 1);
        REQUIRE(a.getElement(0, 0) == 0);
        REQUIRE(a.getElement(1, 0) == 1);
        REQUIRE(a.getElement(2, 0) == 0);
        REQUIRE(a.getElement(3, 0) == 0);
    }

}