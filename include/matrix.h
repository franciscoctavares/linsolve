#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <sys/types.h>

class Matrix {
    private:
        std::vector<double> elements;
        uint nRows, nColumns;
    public:
        Matrix(void) = default;

        /**
         * @brief Standard constructor
         * 
         * @throw std::invalid_argument - if `rows` <= 0
         * @throw std::invalid_argument - if `columns` <= 0
         * @throw std::invalid_argument - if `rows`x`columns` != `newElements`.size()
         */
        Matrix(std::vector<double> newElements, int rows, int columns);

        /**
         * @brief Copy constructor
         */
        Matrix(const Matrix& matrix);

        /**
         * @brief Displays the matrix on the terminal
         */
        void displayMatrix();

        /**
         * @brief Sums 2 matrices and returns the resulting matrix
         * 
         * @throw std::invalid_argument - if both matrices don't have the same number of rows and columns
         */
        Matrix operator+(Matrix matrix);

        /**
         * @brief Sums 2 matrices, but stores the result in the caller
         * 
         * @throw std::invalid_argument - if both matrices don't have the same number of rows and columns
         */
        void operator+=(Matrix matrix);

        /**
         * @brief Subtracts 2 matrices and returns the result
         * 
         * std::invalid_argument - if both matrices don't have the same number of rows and columns
         */
        Matrix operator-(Matrix matrix);

        /**
         * @brief Subtracts 2 matrices, but stores the result in the caller
         * 
         * std::invalid_argument - if both matrices don't have the same number of rows and columns
         */
        void operator-=(Matrix matrix);

        /**
         * @brief Multiplies 2 matrices and returns the result
         * 
         * @throw std::invalid_argument - if `matrix`.rows() != number of columns of the first matrix
         */
        Matrix operator*(Matrix matrix);

        /**
         * @brief Returns a specified row of the matrix
         * 
         * @throw std::invalid_argument - if `row` < 0 or `row` >= nRows
         */
        Matrix getRow(int row);

        /**
         * @brief Returns a specified column of the matrix
         * 
         * std::invalid_argument - if `column` < 0 or `column` >= nColumns
         */
        Matrix getColumn(int column);

        /**
         * @brief Performs a row operation on the matrix(targetRow += sourceRow * factor)
         * 
         * @throw std::invalid_argument - if `sourceRow` < 0 or `sourceRow` >= nRows
         * @throw std::invalid_argument - if `targetRow` < 0 or `targetRow` >= nRows
         */
        void rowOperation(int sourceRow, int targetRow, double factor);

        /**
         * @brief Performs a column operation on the matrix(targetColumn += sourceColumn * factor)
         * 
         * @throw std::invalid_argument - if `sourceColumn` < 0 or `sourceColumn` >= nColumns
         * @throw std::invalid_argument - if `targetColumn` < 0 or `targetColumn` >= nColumns
         */
        void columnOperation(int sourceColumn, int targetColumn, double factor);
        
        /**
         * @brief Computes the dot product of 2 vector matrices
         * 
         * @throw std::invalid_argument - if any of the operators is not a row/column matrix(nRows == 1 or nColumns == 1)
         */
        double dotProduct(Matrix matrix);
        
        /**
         * @brief Returns the number of rows of the matrix
         */
        uint getNRows();

        /**
         * @brief Returns the number of columns of the matrix
         */
        uint getNColumns();
        
        /**
         * @brief Returns a specific element from the matrix, given its coordinates
         * 
         * @throw std::invalid_argument - if `row` < 0 or `row` >= `nRows`
         * @throw std::invalid_argument - if `column` < 0 or `column` >= `nColumns`
         */
        double getElement(int row, int column);

        /**
         * @brief Sets the value of a specific element from the matrix, given its coordinates and the value
         * 
         * @throw std::invalid_argument - if `row` < 0 or `row` >= `nRows`
         * @throw std::invalid_argument - if `col` < 0 or `col` >= `nColumns`
         */
        void setElement(int row, int col, double value);
        
        /**
         * @brief Stacks 2 matrices vertically
         * 
         * @throw std::invalid_argument - if `nColumns` != `matrix.nColumns`
         */
        void stackVertical(Matrix matrix);

        /**
         * @brief Stacks 2 matrices horizontally
         * 
         * @throw std::invalid_argument - if `nRows` != `matrix.nRows`
         */
        void stackHorizontal(Matrix matrix);
        
        /**
         * @brief Returns the transpose of the caller
         */
        Matrix transpose();

        /**
         * @brief Multiplies all elements of the matrix by a value and returns the resulting matrix
         */
        Matrix operator*(double value);

        /**
         * @brief Multiplies all elements of the matrix by a value, but stores the result in the caller
         */
        void operator*=(double value);

        /**
         * @brief Returns the index of the highest element in the matrix
         */
        uint maxValueIndex();

        /**
         * @brief Returns the index of the lowest element in the matrix
         */
        uint minValueIndex();

        /**
         * @brief Performs point-wise division, and returns the resulting matrix
         * 
         * @throw std::invalid_argument - if `nRows` != `matrix.nRows` or `nColumns` != `matrix.nColumns`
         */
        Matrix pointDivision(Matrix matrix);

        /**
         * @brief Replaces the specified row of the matrix with a new row matrix and returns the resulting matrix
         * 
         * @throw std::invalid_argument - if `row` < 0 or `row` >= `nRows`
         * @throw std::invalid_argument - if `nColumns` != `newRow.nColumns`
         */
        Matrix setRow(int row, Matrix newRow);

        /**
         * @brief Replaces the specified column of the matrix with a new column matrix and returns the resulting matrix
         * 
         * @throw std::invalid_argument - if `column` < 0 or `column` >= `nColumns`
         * @throw std::invalid_argument - if `nRows` != `newRow.nRows`
         */
        Matrix setColumn(int column, Matrix newColumn);
        
        /**
         * @brief Removes the specified row from the matrix and returns the resulting matrix
         * 
         * @throw std::invalid_argument - if `row` < 0 || `row` >= `nRows`
         */
        Matrix removeRow(int row);

        /**
         * @brief Removes the specified column from the matrix and returns the resulting matrix
         * 
         * @throw std::invalid_argument - if `column` < 0 || `column` >= `nColumns`
         */
        void removeColumn(int column);
        
        /**
         * @brief Searches the matrix(assumes its a vector matrix) for the value passed as an argument and returns its index if found. If not, it returns -1
         * 
         */
        int findValueInVectorMatrix(double value);

        /**
         * @brief Constructs the sub matrix contained within the rows and columns borders passed as arguments, and returns the resulting sub matrix
         * 
         * @throw std::invalid_argument - if `startingRow` < 0 or `startingRow` >= ´nRows´
         * @throw std::invalid_argument - if `endingRow` < 0 or `endingRow` >= ´nRows´
         * @throw std::invalid_argument - if `startingRow` > `endingRow`
         * 
         * @throw std::invalid_argument - if `startingColumn` < 0 or `startingColumn` >= ´nColumns´
         * @throw std::invalid_argument - if `endingColumn` < 0 or `endingColumn` >= ´nColumns´
         * @throw std::invalid_argument - if `startingColumn` > `endingColumn`
         */
        Matrix subMatrix(int startingRow, int endingRow, int startingColumn, int endingColumn);

        /**
         * @brief Retrieves the elements of the matrix
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
 * @throw std::invalid_argument - if `rows` <= 0
 * @throw std::invalid_argument - if `columns` <= 0
 */
Matrix zeros(int rows, int columns);

/**
 * @brief Constructs and returns a basis vector matrix with the specified size, as well as the only non-zero element(1) being placed in the index passed as an argument
 * 
 * @throw std::invalid_argument - if `size` <= 0
 * @throw std::invalid_argument - if `index` < 0 or `index` >= `size`
 */
Matrix basisVector(int size, int index);

#endif