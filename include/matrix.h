#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <sys/types.h>

class Matrix {
    private:
        std::vector<double> elements;
        uint n, m;
    public:
        Matrix(void) = default;
        Matrix(std::vector<double>, uint, uint);
        void displayMatrix();
        
        // Basic Operations
        Matrix operator+(Matrix);
        void operator+=(Matrix);
        Matrix operator-(Matrix);
        void operator-=(Matrix);
        Matrix operator*(Matrix);

        // extra operator
        void operator=(Matrix);

        std::vector<double> getRow(uint);
        Matrix getColumn(uint);

        void rowOperation(uint, uint, double);
        void columnOperation(uint, uint, double);
        
        double dotProduct(Matrix); // dot product, when the 2 matrices are vectors
        
        unsigned rows(); // returns the number of rows
        unsigned columns(); // returns the number of columns
        
        double getElement(uint, uint); // returns the element given the indices of the row and column
        void setElement(uint, uint, double); // given the indices of the row and column, sets the element to the value specified by the argument
        
        void stackVertical(Matrix);
        void stackHorizontal(Matrix);
        
        Matrix transpose();

        Matrix operator*(double);
        void operator*=(double);
        
        unsigned maxValueIndex();
        unsigned minValueIndex();

        Matrix pointDivision(Matrix);

        Matrix setRow(uint, Matrix);
        Matrix setColumn(uint, Matrix);
        
        Matrix removeRow(uint);
        void removeColumn(uint);
        
        unsigned findValueInVectorMatrix(double);

        Matrix subMatrix(uint, uint, uint, uint);
        std::vector<double> getElements();

        // More auxiliary methods...
        int isBasisVector();

        bool operator==(Matrix matrix);
};

Matrix zeros(uint, uint);
Matrix basisVector(uint, uint);

#endif