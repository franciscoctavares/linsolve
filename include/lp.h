#ifndef LP_H
#define LP_H

#include "matrix.h"
#include "constraint.h"

#include <vector>
#include <string>
#include <tuple>

#include <limits>
#include <sys/types.h>

#define M 1000000

enum ProblemType {
    MIN,
    MAX
};

enum ProblemStatus {
    NOT_YET_SOLVED,
    CONTINUOUS_SOLUTION,
    WHOLE_SOLUTION,
    INFEASIBLE,
    UNBOUNDED
};

typedef struct {
    std::vector<uint> repeatedConstraints;
    std::vector<std::pair<uint, double>> fixedVariables;
    std::vector<uint> constraintsToRemove;
    std::vector<std::pair<uint, uint>> pairsOfVars;
}SimplifiedConstraintsHelper;

class LpProblem {
    private:
        ProblemType type;
        Matrix objectiveFunction;
        std::vector<Constraint> constraints;
        Matrix optimalSolution;
        ProblemStatus status;

        /**
         * @brief Checks the cj - zj(passed as an argument) row see if any of the elements is positive, to see if more simplex iterations are necessary
         * 
         * @return true - if all values in the cj - zj row are less than or equal to zero, false otherwise
         */
        bool isSimplexDone(Matrix cj_minus_zj);

        /**
         * @brief Given the pivot column elements(), the b column elements, and the ratios column matrix, returns the index of the pivot row
         * 
         * @return uint - the index of the pivot row
         */
        uint getPivotRow(std::vector<double> simplexAux, std::vector<double> bAux, Matrix ratios);

        /**
         * @brief Given the extraCj row matrix(cj row minus the objective function's coefficients), return the basic variables' indexes
         * 
         * @return Matrix 
         */
        Matrix getBasisIndexes(Matrix extraCj);

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
         * 
         * @return Matrix - optimal solution
         */
        Matrix solveSimplex();

        /**
         * @brief Checks if the LP problem can be simplified, and stores information about which constraints to remove 
         *        and variables to fix in the `helper` variable passed as an argument
         */
        bool canProblemBeSimplified(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Uses the information stored in the `helper` variable argument to simplify the problem
         */
        bool simplifyProblem(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Uses the information stored in the `helper` variable argument and the simplified problem solution(`simplifiedSolution`)
         *        to obtain the initial problem's solution
         */
        void simplifiedProblemSolution(SimplifiedConstraintsHelper* helper, Matrix simplifiedSolution);

        /**
         * @brief Checks if any variables can be fixed and stores the constraints to remove and the set values of the those fixed variables
         *        in the `helper` variable argument
         */
        void canVariablesBeFixed(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Checks for repeated constraints and stores the indexes of those repeated constraints in the
         *        `helper` variable argument
         */
        void checkForRepeatedConstraints(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Checks the `helper` variable argumnt and removes redundant information
         */
        void removeRepeatedFixedVariablesPairs(SimplifiedConstraintsHelper* helper);

        /**
         * @brief For fixed variables, creates a correspondence between the original problem's variable's indexes 
         *        and the simplified problem's variable's indexes, and stores it in the `helper` variable argument
         */
        void newVarsToOldVars(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Removes the constraints from the model whose indexes are stored in the `helper` variable argument
         */
        void removeConstraints(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Sets the variable whose index is `varIndex` equal to `fixedVarValue`, and removes it from the objective function and all constraints
         * 
         * @throw std::invalid_argument - if `varIndex` < 0 or `varIndex` >= `objectiveFunction.getNColumns()`
         */
        void removeOneFixedVariable(int varIndex, double fixedVarValue);

        /**
         * @brief Removes all fixed variables, as specified by the `helper` argument
         */
        void removeFixedVariables(SimplifiedConstraintsHelper* helper);

    public:
        LpProblem(void) = default;
        
        /**
         * @brief Constructs a new LP model
         * 
         * @param modelType the type of optimization problem: maximization(MAX) or minimization(MIN)
         * @param newObjectiveFunction the objective function's coefficients
         * @param newConstraints the constraints of the model
         */
        LpProblem(ProblemType modelType, std::vector<double> newObjectiveFunction, std::vector<Constraint> newConstraints);

        /**
         * @brief Copy constructor
         */
        LpProblem(const LpProblem& problem);

        /**
         * @brief Copy assignment operator
         */
        LpProblem& operator=(const LpProblem& otherProblem);

        /**
         * @brief Displays the current simplex tableau on the terminal
         */
        void displaySimplexTableau(Matrix tableau, Matrix cb, Matrix basisIndexes, Matrix cj, Matrix b, Matrix zj, Matrix cj_minus_zj);

        /**
         * @brief Displays the LP model and its optimal solution on the terminal
         */
        void displayProblem();
        
        /**
         * @brief Adds `newConstraint` to the LP model
         */
        void addConstraint(Constraint newConstraint);

        /**
         * @brief Removes the constraint with index `constraintIndex`
         * 
         * @throw std::invalid_argument - if `constraintIndex` < 0 or `constraintIndex` >= `constraints.size()`
         */
        void removeConstraint(int constraintIndex);

        /**
         * @brief Returns `optimalSolution`
         */
        Matrix getOptimalSolution();

        /**
         * @brief Returns the LP model type(MAX or MIN)
         */
        ProblemType getType();

        /**
         * @brief Returns `objectiveFunction`
         */
        Matrix getObjectiveFunction();
        
        /**
         * @brief Solves the LP model
         */
        void solveProblem();

        /**
         * @brief Checks if `status` is equal to `statusToCheck`
         */
        bool operator==(ProblemStatus statusToCheck);
        
        /**
         * @brief Checks if `status` is not equal to `statusToCheck`
         */
        bool operator!=(ProblemStatus statusToCheck);

        /**
         * @brief Checks if all variables of the optimal solution are integers
         */
        bool isOptimalSolutionWhole();

        /**
         * @brief Returns the `status`
         */
        ProblemStatus getStatus();

};

/**
 * @brief Helper function for rounding integers that are within a margin of error(set by the epsilon variable)
 */
std::pair<bool, double> isDoubleAnInteger(double number, double epsilon = 1e-10);

#endif