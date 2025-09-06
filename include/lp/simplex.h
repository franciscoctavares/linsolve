#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "lp/lp.h"
#include "matrix.h"

#include <utility>
#include <vector>
#include <sys/types.h>

namespace LP {

struct SimplexMatrices {
    Matrix tableau;
    Matrix b;
    Matrix cj;
    Matrix basisIndices;
    Matrix cb;
    Matrix zj;
    Matrix cj_minus_zj;

    // extra stuff
    std::pair<uint, uint> pivots;
};

class SimplexSolver {
    private:
        LpProblem problem;
        SimplexMatrices simplexMatrices;

        /**
         * @brief Checks the cj - zj(passed as an argument) row see if any of the elements is positive, to see if more simplex iterations are necessary
         * 
         * @return true - if all values in the cj - zj row are less than or equal to zero, false otherwise
         */
        bool isSimplexDone(Matrix& cj_minus_zj);

        /**
         * @brief Given the pivot column elements(), the b column elements, and the ratios column matrix, returns the index of the pivot row
         * 
         * @return uint - the index of the pivot row
         */
        uint getPivotRow(Matrix& simplexAux, Matrix& bAux, Matrix& ratios);

        /**
         * @brief Given the extraCj row matrix(cj row minus the objective function's coefficients), return the basic variables' indexes
         * 
         * @return Matrix 
         */
        Matrix getBasisIndexes(Matrix& extraCj);

        /**
         * @brief Retrieves all the constraints' LHS(left hand side) and returns them in matrix form
         * 
         */
        Matrix getConstraintsLHS();

        /**
         * @brief Retrieves all the constraints' types
         * 
         */
        std::vector<ConstraintType> getConstraintsTypes();

        /**
         * @brief Retrieves all the constraint's RHS(right hand side) and returns them in matrix form
         * 
         * @return Matrix 
         */
        Matrix getConstraintsRHS();

        /**
         * @brief Checks if `potentialSolution` respects the constraint whose index is `constraintIndex`
         * 
         * @throw std::invalid_argument - if `constraintIndex` < 0 or `constraintIndex` >= `constraints.size()`
         * 
         */        
        bool isConstraintSatisfied(Matrix potentialSolution, int constraintIndex);

        /**
         * @brief Checks if `potentialSolution` satisfies all the constraints of the model
         */
        bool isSolutionAdmissible(Matrix potentialSolution);

        /**
         * @brief Builds and returns the extra variables(surplus, slack and artificial) matrix, which is used to stack horizontally to the constraints' LHS matrix
         *        , for the initial simplex tableau
         */
        Matrix extraVariablesMatrix();

        /**
         * @brief Builds all the matrices of the initial simplex tableau, and returns them encapsulated in an std::vector<Matrix> variable
         */
        std::vector<Matrix> initialSimplexTableau();

        /**
         * @brief Given the `extraCj` row matrix(cj row minus the objective function's coefficients at the end), returns the extra variables' indexes in the extraCj matrix.
         *        This method is auxiliary to some other methods
         */
        std::vector<std::pair<int, int>> getConstraintsIndexes(Matrix extraCj);

        /**
         * @brief Solves the LP model using the simplex method
         */
        std::pair<Matrix, SolutionType> solveSimplex();

        void isOptimalSolutionWhole();

        void computeOriginalProblemSolution(std::pair<Matrix, SolutionType>& simplifiedResult, SimplifierHelper& helper);


    public:
        SimplexSolver(LpProblem problemToSolve);

        std::pair<Matrix, SolutionType> runSolver();
};

}

#endif