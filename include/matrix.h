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

        /**
         * @brief Matrix class constructor
         * 
         * @param vec vector with the newElements
         * @param a number of rows
         * @param b number of columns
         */
        Matrix(std::vector<double> vec, uint a, uint b);

        /**
         * @brief Copy constructor
         * 
         */
        Matrix(const Matrix& matrix);

        /**
         * @brief Displays the matrix on the terminal
         * 
         */
        void displayMatrix();

        /**
         * @brief Summation of two matrices, but only if the number of dimention matches. If not, it throws a std::runtime_error with an error message.
         * 
         * @param matrix the second operator
         * @return Matrix - the result of the sum
         */
        Matrix operator+(Matrix matrix);

        /**
         * @brief Summation of two matrices, but in a more compact way. Instead of returning the result as a new Matrix object, it stores the result in the caller.
         * 
         * @param matrix the second operator
         */
        void operator+=(Matrix matrix);

        /**
         * @brief Subtraction of two matrices, but only if the number of dimention matches. If not, it throws a std::runtime_error with an error message.
         * 
         * @param matrix the second operator
         * @return Matrix - the result of the subtraction
         */
        Matrix operator-(Matrix matrix);

        /**
         * @brief Subtraction of two matrices, but in a more compact way. Instead of returning the result as a new Matrix object, it stores the result in the caller.
         * 
         * @param matrix the second operator
         */
        void operator-=(Matrix matrix);

        /**
         * @brief Multiplication of two matrices, but only if the dimensions of the matrices match. If not, it throws a std::runtime_error with an error message.
         * 
         * @param matrix the second operator
         * @return Matrix - the result of the multiplication
         */
        Matrix operator*(Matrix matrix);

        /**
         * @brief Retrieves the specified row from the matrix
         * 
         * @param r the index of the row to retrieve
         * @return std::vector<double> - a vector with elements of the specified row
         * @throw std::runtime_error - if r is negative or greater than or equal than the total number of rows
         */
        std::vector<double> getRow(uint r);

        /**
         * @brief Retrieves the specified column from the matrix
         * 
         * @param c the index of the column to retrieve
         * @return Matrix - a column matrix whose elements are from the specified column
         * @throw std::runtime_error - if c is negative or greater than or equal than the total number of columns
         */
        Matrix getColumn(uint c);

        /**
         * @brief Performs a row operation on the matrix, given a source row, a target row, and a factor.
         * 
         * @param a the source row
         * @param b the target row
         * @param factor the factor number
         * @throw std::runtime_error - if a or b are negative or greater than or equal than the total number of rows
         */
        void rowOperation(uint a, uint b, double factor);

        /**
         * @brief Performs a column operation on the matrix, given a source column, a target column, and a factor.
         * 
         * @param a the source column
         * @param b the target column
         * @param factor the factor number
         * @throw std::runtime_error - if a or b are negative or greater than or equal than the total number of columns
         */
        void columnOperation(uint a, uint b, double factor);
        
        /**
         * @brief Performs the dot product, if the matrices are vector(row or column) matrices
         * 
         * @param matrix the second operator
         * @return double the dot product of the two matrices
         * @throw std::runtime_error - if one or both matrices are not vector(row or column) matrices
         */
        double dotProduct(Matrix matrix);
        
        /**
         * @brief Retrieves the number of rows of the matrix
         * 
         */
        uint rows();

        /**
         * @brief Retrieves the number of columns of the matrix
         * 
         */
        uint columns();
        
        /**
         * @brief Retrieves the element of the matrix whose coordinates are specified by the arguments
         *  
         */
        double getElement(uint row, uint column);

        /**
         * @brief Replaces the element specified by arguments(row and col) by a new value
         * 
         */
        void setElement(uint row, uint col, double value);
        
        /**
         * @brief Stacks two matrices vertically, and stores the result in the caller
         * 
         * @throw std::runtime_error - if both matrices don't have the same number of columns
         */
        void stackVertical(Matrix matrix);

        /**
         * @brief Stacks two matrices horizontally, and stores the result in the caller
         * 
         * @throw std::runtime_error - if both matrices don't have the same number of rows
         */
        void stackHorizontal(Matrix matrix);
        
        /**
         * @brief Returns the transpose of the caller
         * 
         */
        Matrix transpose();

        /**
         * @brief Multiplies all elements by a number passed as an argument and returns the result matrix
         * 
         */
        Matrix operator*(double value);

        /**
         * @brief Multiplies all elements by a number passed as an argument, and stores the result in the caller
         * 
         */
        void operator*=(double value);

        /**
         * @brief Retrieves the index of the highest element in the matrix
         * 
         */
        uint maxValueIndex();

        /**
         * @brief Retrieves the index of the lowest element in the matrix
         * 
         */
        uint minValueIndex();

        /**
         * @brief Performs point-wise division, and returns the result matrix
         * 
         * @throw std::runtime_error - if both matrices don't have the same number of rows and columns
         */
        Matrix pointDivision(Matrix matrix);

        /**
         * @brief Replaces the row of the caller specified by the argument by the column matrix passed as an argument, and returns the result matrix
         * 
         * @throw std::runtime_error - if row is negative or greater than or equal than the total number of rows
         * @throw std::runtime_error - if the column matrix and the caller don't have the same number of columns
         */
        Matrix setRow(uint row, Matrix matrix);

        /**
         * @brief Replaces the column of the caller specified by the argument by the row matrix passed as an argument, and returns the result matrix
         * 
         * @throw std::runtime_error - if column is negative or greater than or equal than the total number of column
         * @throw std::runtime_error - if the row matrix and the caller don't have the same number of rows
         */
        Matrix setColumn(uint column, Matrix matrix);
        
        /**
         * @brief Removes the specified row from the matrix, and results the result matrix
         * 
         * @throw std::runtime_error - if row is negative or greater than or equal than the total number of rows
         */
        Matrix removeRow(uint row);

        /**
         * @brief Removes the specified column from the matrix, and results the result matrix
         * 
         * @throw std::runtime_error - if column is negative or greater than or equal than the total number of columns
         */
        void removeColumn(uint column);
        
        /**
         * @brief Searches the matrix(assumes its a vector matrix) for the value passed as an argument and returns its index if found. If not, it returns -1
         * 
         */
        uint findValueInVectorMatrix(double value);

        /**
         * @brief Constructs the sub matrix contained within the rows and columns borders passed as arguments, and returns the sub matrix
         * 
         */
        Matrix subMatrix(uint r1, uint r2, uint c1, uint c2);

        /**
         * @brief Retrieves the elements of the matrix
         * 
         */
        std::vector<double> getElements();

        /**
         * @brief Checks if the matrix is a basis vector, and if it is, returns the index of the only non-zero element(1).
         */
        int isBasisVector();

        /**
         * @brief Checks if two matrices are equal, meaning all elements are the same and they have the number of rows and columns
         * 
         */
        bool operator==(Matrix matrix);
};

/**
 * @brief Constructs and returns a matrix with specified dimensions which contains only zeros
 * 
 */
Matrix zeros(uint rows, uint columns);

/**
 * @brief Constructs and returns a basis vector matrix with the specified size, as well as the only non-zero element(1) being placed in the index passed as an argument
 * 
 */
Matrix basisVector(uint size, uint index);

#endif