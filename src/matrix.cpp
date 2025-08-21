#include "../include/matrix.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <string>

Matrix::Matrix(std::vector<double> newElements, int rows, int columns) {
    if(rows <= 0) {
        std::ostringstream errorMsg;
        errorMsg << "Error using Matrix(class constructor): nRows must be a positive number but the value provided was " << nRows;
        throw std::invalid_argument(errorMsg.str());
    }
    if(columns <= 0) {
        std::ostringstream errorMsg;
        errorMsg << "Error using Matrix(class constructor): nColumns must be a positive number but the value provided was " << nColumns;
        throw std::invalid_argument(errorMsg.str());
    }
    if(rows * columns != newElements.size()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using Matrix(class constructor): nRows x nColumns is not equal to the size of the newElements vector";
        throw std::invalid_argument(errorMsg.str());
    }

    elements = newElements;
    nRows = rows;
    nColumns = columns;
}

Matrix::Matrix(const Matrix& matrix) {
    elements = matrix.elements;
    nRows = matrix.nRows;
    nColumns = matrix.nColumns;
}

void Matrix::displayMatrix() {
    size_t maxWidth = 0;
    for(double val: elements) {
        std::string str = std::to_string(val);
        // Trim trailing zeroes for nicer formatting (optional)
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back(); // remove trailing dot if needed
        maxWidth = std::max(maxWidth, str.length());
    }

    unsigned padding = 1;
    maxWidth += padding;

    for(int i = 0; i < nRows; i++) {
        //for(int l = 0; l < 18 - 1; l++) std::cout << " ";
        std::cout << "|";
        for(int j = 0; j < nColumns; j++) {
            //if(elements[i * m + j] < 0) std::cout << "-";
            //else std::cout << "+";
            std::cout << std::setw(maxWidth) << elements[i * nColumns + j];
            //std::cout << std::setprecision(3) << std::fixed << fabs(elements[i * m + j]);
            //if(j < m - 1) std::cout << " ";
        }
        for(int k = 0; k < padding; k++) std::cout << " ";
        std::cout << "|" << std::endl;
        //for(int k = 0; k < padding; k++)
    }
    //for(int i = 0; i < padding; i++) std::cout
}

Matrix Matrix::operator+(Matrix matrix) {
    if(nRows != matrix.nRows || nColumns != matrix.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using operator+: matrix1 has dimensions (" << nRows << ", " << nColumns << 
                    ") and matrix 2 has dimenions (" << matrix.nRows << ", " << matrix.nColumns << ")";
        throw std::invalid_argument(errorMsg.str());
    }

    Matrix aux(elements, nRows, nColumns);
    for(int i = 0; i < nRows * nColumns; i++) {
        aux.elements[i] += matrix.elements[i];
    }
    return aux;
}

void Matrix::operator+=(Matrix matrix) {
    if(nRows != matrix.nRows || nColumns != matrix.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using operator+=: matrix1 has dimensions (" << nRows << ", " << nColumns << 
                    ") and matrix 2 has dimenions (" << matrix.nRows << ", " << matrix.nColumns << ")";
        throw std::runtime_error(errorMsg.str());
    }

    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            elements[i * nColumns + j] += matrix.elements[i * nColumns + j];
        }
    }
}

Matrix Matrix::operator-(Matrix matrix) {
    if(nRows != matrix.nRows || nColumns != matrix.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using operator-: matrix1 has dimensions (" << nRows << ", " << nColumns << 
                    ") and matrix 2 has dimenions (" << matrix.nRows << ", " << matrix.nColumns << ")";
        throw std::runtime_error(errorMsg.str());
    }

    Matrix aux(elements, nRows, nColumns);
    for(int i = 0; i < nRows * nColumns; i++) {
        aux.elements[i] -= matrix.elements[i];
    }
    return aux;
}

void Matrix::operator-=(Matrix matrix) {
    if(nRows != matrix.nRows || nColumns != matrix.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using operator-=: matrix1 has dimensions (" << nRows << ", " << nColumns << 
                    ") and matrix 2 has dimenions (" << matrix.nRows << ", " << matrix.nColumns << ")";
        throw std::runtime_error(errorMsg.str());
    }
    
    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            elements[i * nColumns + j] -= matrix.elements[i * nColumns + j];
        }
    }
}

Matrix Matrix::operator*(Matrix matrix) {
    if(nColumns == matrix.nRows) {
        std::vector<double> newStuff;
        for(int i = 0; i < nRows * matrix.nColumns; i++) newStuff.push_back(0.0);
        Matrix newMatrix(newStuff, nRows, matrix.nColumns);
        double aux;
        for(int i = 0; i < nRows; i++) {
            for(int j = 0; j < matrix.nColumns; j++) {
                aux = 0;
                for(int k = 0; k < nColumns; k++) {
                    aux += elements[i * nColumns + k] * matrix.elements[k * matrix.nRows + j];
                }
                newMatrix.elements[i * matrix.nColumns + j] = aux;
            }
        }
        return newMatrix;
    }
    else throw std::invalid_argument("Dimensions don't match");
}

Matrix Matrix::getRow(int row) {
    if(row < 0 || row >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using getRow: row be between 0 and " << nRows - 1 << ", but the value provided was " << row;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> aux;
    for(int j = 0; j < nColumns; j++) {
        aux.push_back(elements[row * nColumns + j]);
    }
    return Matrix(aux, 1, nColumns);
}

Matrix Matrix::getColumn(int column) {
    if(column < 0 || column >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using getColumn: column be between 0 and " << nColumns - 1 << ", but the value provided was " << column;
        throw std::invalid_argument(errorMsg.str());    
    }

    std::vector<double> aux;
    for(int i = 0; i < nRows; i++) {
        aux.push_back(elements[i * nColumns + column]);
    }
    return Matrix(aux, nRows, 1);
}

void Matrix::rowOperation(int sourceRow, int targetRow, double factor) {
    if(sourceRow < 0 || sourceRow >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using rowOperation: sourceRow must be between 0 and " << nRows - 1 << ", but the value provided was " << sourceRow;
        throw std::invalid_argument(errorMsg.str());
    }
    if(targetRow < 0 || targetRow >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using rowOperation: targetRow must be between 0 and " << nRows - 1 << ", but the value provided was " << targetRow;
        throw std::invalid_argument(errorMsg.str());
    }

    for(int j = 0; j < nColumns; j++) {
        elements[targetRow * nColumns + j] += elements[sourceRow * nColumns + j] * factor;
    }
}

void Matrix::columnOperation(int sourceColumn, int targetColumn, double factor) {
    if(sourceColumn < 0 || sourceColumn >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using columnOperation: sourceColumn must be between 0 and " << nColumns - 1 << ", but the value provided was " << sourceColumn;
        throw std::invalid_argument(errorMsg.str());    
    }
    if(targetColumn < 0 || targetColumn >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using columnOperation: targetColumn must be between 0 and " << nColumns - 1 << ", but the value provided was " << targetColumn;
        throw std::invalid_argument(errorMsg.str()); 
    }

    for(int i = 0; i < nRows; i++) {
        elements[i * nColumns + targetColumn] += elements[i * nColumns + sourceColumn] * factor;
    }
}

double Matrix::dotProduct(Matrix matrix) { // assumes both matrices are row/column matrices(matrix 1 could be row and 2 could be column)
    if((nRows != 1 && nColumns != 1) || (matrix.nRows != 1 && matrix.nColumns != 1)) {
        std::ostringstream errorMsg;
        errorMsg << "Error using dotProduct: Both matrices have to be vector(row or column) matrices";
        throw std::invalid_argument(errorMsg.str());
    }
    else if(elements.size() != matrix.elements.size()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using dotProduct: cannot calculate the dot product when matrix 1 and matrix 2 have " << elements.size() 
                 << " and " << matrix.elements.size() << " elements, respectively";
        throw std::invalid_argument(errorMsg.str());
    }

    double result = 0;
    for(int i = 0; i < elements.size(); i++) {
        result += elements[i] * matrix.elements[i];
    }
    return result;

}

uint Matrix::getNRows() {
    return nRows;
}

uint Matrix::getNColumns() {
    return nColumns;
}

double Matrix::getElement(int row, int column) {
    if(row < 0 || row >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using getElement: row be between 0 and " << nRows - 1 << ", but the value provided was " << row;
        throw std::invalid_argument(errorMsg.str());
    }
    if(column < 0 || column >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using getElement: row be between 0 and " << nColumns - 1 << ", but the value provided was " << column;
        throw std::invalid_argument(errorMsg.str());
    }

    return elements[row * nColumns + column];
}

void Matrix::setElement(int row, int col, double value) {
    if(row < 0 || row >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using setElement: row must be between 0 and " << nRows - 1 << ", but the value provided was " << row;
        throw std::invalid_argument(errorMsg.str());
    }
    if(col < 0 || col >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using setElement: col must be between 0 and " << nColumns - 1 << ", but the value provided was " << col;
        throw std::invalid_argument(errorMsg.str());
    }

    elements[row * nColumns + col] = value;
}

void Matrix::stackVertical(Matrix matrix) {
    if(nColumns != matrix.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using stackVertical: cannot vertically stack a matrix with " << matrix.nColumns << " columns below a matrix with " << nColumns << " columns";
        throw std::invalid_argument(errorMsg.str());
    }

    for(int i = 0; i < matrix.nRows * matrix.nColumns; i++) {
        elements.push_back(matrix.elements[i]);
    }
    nRows += matrix.nRows;
}

void Matrix::stackHorizontal(Matrix matrix) {
    if(nRows != matrix.nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using stackHorizontal: cannot horizontally stack a matrix with " << matrix.nRows << " rows next to a matrix with " << nRows << " rows";
        throw std::invalid_argument(errorMsg.str());
    }

    unsigned current_index;
    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < matrix.nColumns; j++) {
            current_index = i * (nColumns + matrix.nColumns) + (nColumns + j);
            elements.insert(elements.begin() + current_index, matrix.elements[i * matrix.nColumns + j]);
        }
    }
    nColumns += matrix.nColumns;
}

Matrix Matrix::transpose() {
    Matrix newMatrix = zeros(nColumns, nRows);

    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            newMatrix.setElement(j, i, getElement(i, j));
        }
    }
    return newMatrix;
}

Matrix Matrix::operator*(double value) {
    std::vector<double> aux = elements;
    for(int i = 0; i < nRows * nColumns; i++) aux[i] *= value;
    return Matrix(aux, nRows, nColumns);
}

void Matrix::operator*=(double value) {
    for(int i = 0; i < nRows * nColumns; i++) elements[i] *= value;
}

uint Matrix::maxValueIndex() {
    auto maxIt = std::max_element(elements.begin(), elements.end());
    size_t maxIndex = std::distance(elements.begin(), maxIt);
    return maxIndex;
}

uint Matrix::minValueIndex() {
    auto minIt = std::min_element(elements.begin(), elements.end());
    size_t minIndex = std::distance(elements.begin(), minIt);
    return minIndex;
}

Matrix Matrix::pointDivision(Matrix matrix) {
    if(nRows != matrix.nRows || nColumns != matrix.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using pointDivision: cannot multiply matrix1(" << nRows << " x " << nColumns << ") by matrix2(" << matrix.nRows << " x " << matrix.nColumns << ")";
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> aux = elements;
    for(int i = 0; i < nRows * nColumns; i++) {
        aux[i] /= matrix.getElement(i / nColumns, i % nColumns);
    }
    return Matrix(aux, nRows, nColumns);
}

Matrix Matrix::setRow(int row, Matrix newRow) {
    if(row < 0 || row >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using setRow: the row argument must be between 0 and " << nRows - 1 << ", but the value provided was " << row;
        throw std::invalid_argument(errorMsg.str());
    }
    else if(nColumns != newRow.nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using setRow: matrix has " << nColumns << " columns, but the new row matrix has " << newRow.nColumns << " columns";
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> aux;
    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            if(i == row) aux.push_back(newRow.getElement(0, j));
            else aux.push_back(elements[i * nColumns + j]);
        }
    }
    return Matrix(aux, nRows, nColumns);
}

Matrix Matrix::setColumn(int column, Matrix newColumn) {
    if(column < 0 || column >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using setColumn: the column argument must be between 0 and " << nColumns - 1 << ", but the value provided was " << column;
        throw std::invalid_argument(errorMsg.str());
    }
    else if(nRows != newColumn.nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using setRow: the matrix has " << nRows << " rows, but the new column matrix has " << newColumn.nRows << " rows";
        throw std::invalid_argument(errorMsg.str());
    }
    
    std::vector<double> aux;
    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            if(j == column) aux.push_back(newColumn.getElement(i, 0));
            else aux.push_back(elements[i * nColumns + j]);
        }
    }
    return Matrix(aux, nRows, nColumns);
}

Matrix Matrix::removeRow(int row) {
    if(row < 0 || row >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using removeRow: the row argument must be between 0 and " << nRows - 1 << ", but the value provided was " << row;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> aux;
    for(int i = 0; i < nRows; i++) {
        if(i == row) continue;
        for(int j = 0; j < nColumns; j++) {
            aux.push_back(getElement(i, j));
        }
    }
    return Matrix(aux, nRows - 1, nColumns);
}

void Matrix::removeColumn(int column) {
    if(column < 0 || column > nColumns - 1) {
        std::ostringstream errorMsg;
        errorMsg << "Error using removeColumn: the column argument must be between 0 and " << nColumns - 1 << ", but the value provided was " << column;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> aux = elements;
    elements.clear();
    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            if(j == column) continue;
            elements.push_back(getElement(i, j));
        }
    }
    //return Matrix(aux, n, m - 1);
    nColumns--;
}

int Matrix::findValueInVectorMatrix(double value) {
    for(int i = 0; i < elements.size(); i++) {
        if(elements[i] == value) return i;
    }
    return -1;
}

Matrix Matrix::subMatrix(int startingRow, int endingRow, int startingColumn, int endingColumn) {
    if(startingRow < 0 || startingRow >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using subMatrix: startingRow must be between 0 and " << nRows - 1 << ", but the value provided was " << startingRow;
        throw std::invalid_argument(errorMsg.str());
    }
    if(endingRow < 0 || endingRow >= nRows) {
        std::ostringstream errorMsg;
        errorMsg << "Error using subMatrix: endingRow must be between 0 and " << nRows - 1 << ", but the value provided was " << endingRow;
        throw std::invalid_argument(errorMsg.str());
    }
    if(startingRow > endingRow) {
        std::ostringstream errorMsg;
        errorMsg << "Error using subMatrix: startingRow must be less than or equal than endingRow";
        throw std::invalid_argument(errorMsg.str());
    }
    if(startingColumn < 0 || startingColumn >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using subMatrix: startingColumn must be between 0 and " << nColumns - 1 << ", but the value provided was " << startingColumn;
        throw std::invalid_argument(errorMsg.str());
    }
    if(endingColumn < 0 || endingColumn >= nColumns) {
        std::ostringstream errorMsg;
        errorMsg << "Error using subMatrix: endingColumn must be between 0 and " << nColumns - 1 << ", but the value provided was " << endingColumn;
        throw std::invalid_argument(errorMsg.str());
    }
    if(startingColumn > endingColumn) {
        std::ostringstream errorMsg;
        errorMsg << "Error using subMatrix: startingColumn must be less than or equal than endingColumn";
        throw std::invalid_argument(errorMsg.str());
    }
    
    std::vector<double> aux;
    for(int i = 0; i < nRows; i++) {
        for(int j = 0; j < nColumns; j++) {
            if(i >= startingRow && i <= endingRow && j >= startingColumn && j <= endingColumn) aux.push_back(elements[i * nColumns + j]);
        }
    }
    return Matrix(aux, endingRow - startingRow + 1, endingColumn - startingColumn + 1);
}

std::vector<double> Matrix::getElements() {
    return elements;
}

int Matrix::isBasisVector() {
    unsigned one_count = 0;
    unsigned zero_count = 0;

    int index_of_only_one;

    for(int i = 0; i < elements.size(); i++) {
        if(elements[i] == 0) zero_count++;
        else if(elements[i] == 1) {
            one_count++;
            index_of_only_one = i;
        }
        else return false;

        if(one_count > 1) return false;
    }
    if(one_count == 1 && zero_count == elements.size() - 1) return index_of_only_one;
    else return -1;
}

bool Matrix::operator==(Matrix matrix) {
    return (elements == matrix.elements && nRows == matrix.nRows && nColumns == matrix.nColumns) ? true : false;
}

// Non Matrix class functions

Matrix zeros(int rows, int columns) {
    if(rows <= 0) {
        std::ostringstream errorMsg;
        errorMsg << "Error using zeros: rows must be a positive integer";
        throw std::invalid_argument(errorMsg.str());
    }
    if(columns <= 0) {
        std::ostringstream errorMsg;
        errorMsg << "Error using zeros: columns must be a positive integer";
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> vec;
    for(int i = 0; i < rows * columns; i++) {
        vec.push_back(0.0);
    }
    return Matrix(vec, rows, columns);
}

Matrix basisVector(int size, int index) {
    if(size <= 0) {
        std::ostringstream errorMsg;
        errorMsg << "Error using basisVector: size must be a positive integer";
        throw std::invalid_argument(errorMsg.str());
    }
    if(index < 0 || index >= size) {
        std::ostringstream errorMsg;
        errorMsg << "Error using basisVector: index must be between 0 and " << size - 1 << ", but the value provided was " << index;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> vec;
    for(int i = 0; i < size; i++) {
        if(i == index) vec.push_back(1.0);
        else vec.push_back(0.0);
    }
    return Matrix(vec, size, 1);
}