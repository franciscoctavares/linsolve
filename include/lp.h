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
         * @brief Checks if the provided candidate solution respects the constraint whose index is 
         *        provided by the constraintIndex argument
         * 
         * @param potentialSolution - row matrix of the candidate solution(x1, x2, x3, ... values)
         * @param constraintIndex - the index of the constraint to check
         * @return true if the constraint is satisfied and false if otherwise
         * 
         * @throw std::runtime_error - if the constraintIndex argument is negative or greater than or equal the total number of constraints of the model
         */
        bool isConstraintSatisfied(Matrix potentialSolution, int constraintIndex);

        /**
         * @brief Checks if the candidate solution satisfies all the constraints of the model
         * 
         * @param potentialSolution - the candidate solution
         * @return true if the candidate solution is feasible(satisfies all the constraints) and false otherwise
         */
        bool isSolutionAdmissible(Matrix potentialSolution);

        /**
         * @brief Builds and returns the extra variables(surplus, slack and artificial) matrix, which is used to stack horizontally to the constraints' LHS matrix
         * 
         */
        Matrix extraVariablesMatrix();

        /**
         * @brief Builds all the matrices of the initial simplex tableau, and returns them encapsulated in an std::vector<Matrix> variable
         * 
         */
        std::vector<Matrix> initialSimplexTableau();

        /**
         * @brief Given the extraCj row matrix(cj row minus the objective function's coefficients), returns the extra variables' indexes in the extraCj matrix.
         *        This method is auxiliary to some other methods
         * 
         * @return std::vector<std::vector<int>> 
         */
        std::vector<std::pair<int, int>> getConstraintsIndexes(Matrix extraCj);

        /**
         * @brief Displays the current simplex tableau
         * 
         */
        void displaySimplexTableau(Matrix tableau, Matrix cb, Matrix basisIndexes, Matrix cj, Matrix b, Matrix zj, Matrix cj_minus_zj);

        /**
         * @brief Solves the LP model using the simplex method
         * 
         * @return Matrix - optimal solution
         */
        Matrix solveSimplex();

        /**
         * @brief Displays the LP model and its optimal solution(if it exists)
         * 
         */
        void displayProblem();
        
        /**
         * @brief Adds a new constraint to the LP model
         * 
         * @param newConstraint the new constraint to be added
         */
        void addConstraint(Constraint newConstraint);

        /**
         * @brief Removes the constraint with index constraintIndex
         * 
         * @param constraintIndex - the index of the constraint to remove
         */
        void removeConstraint(int constraintIndex);
        
        /**
         * @brief Checks if the output of the simplex solver is a feasible solution, that is, if all artificial variables(if present)
         *        were eliminated
         * 
         * @param candidateSolution - the potential feasible solution 
         * @return true if the candidate solution is feasible, false otherwise
         */
        bool isProblemFeasible(Matrix candidateSolution);

        /**
         * @brief Checks if the problem is unbounded, using the output of the simplex solver.
         * 
         * @param candidateSolution - the output of the simplex solver
         * @return true if the problem is unbounded, false otherwise
         */
        bool isProblemBounded(Matrix candidateSolution);

        /**
         * @brief Retrieves the optimal solution to the LP model
         * 
         */
        Matrix getOptimalSolution();

        /**
         * @brief Sets a newn optimal solution
         * 
         */
        void setOptimalSolution(Matrix newOptimalSolution);

        /**
         * @brief Retrieves the LP model type(MAX or MIN)
         * 
         */
        ProblemType getType();

        /**
         * @brief Retrieves the row matrix with the objective function coefficients
         * 
         */
        Matrix getObjectiveFunction();

        /**
         * @brief Checks if the LP problem can be simplified, and stores information about which constraints to remove 
         *        and variables to fix in the helper variable passed as an argument
         * 
         */
        bool canProblemBeSimplified(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Uses the information stored in the helper variable passed as an argument to simplify the problem
         */
        bool simplifyProblem(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Uses the information stored in the helper variable passed as an argument and the simplified problem solution
         *        to obtain the initial problem's solution
         * 
         */
        void simplifiedProblemSolution(SimplifiedConstraintsHelper* helper, Matrix simplifiedSolution);

        /**
         * @brief Checks if any variables can be fixed and stores the constraints to remove and the set values of the those fixed variables
         *        in the helper variable passed as an argument
         * 
         */
        void canVariablesBeFixed(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Checks for repeated constraints and stores the indexes of those repeated constraints in the
         *        helper variable passed as an argument
         * 
         */
        void checkForRepeatedConstraints(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Checks the helper variable passed as an argument and removes redundant information
         */
        void removeRepeatedFixedVariablesPairs(SimplifiedConstraintsHelper* helper);

        /**
         * @brief For fixed variables, creates a correspondence between the original problem's variable's indexes 
         *        and the simplified problem's variable's indexes, and stores it in the helper variable passed as an argument
         * 
         */
        void newVarsToOldVars(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Removes the constraints from the model whose indexes are stored in the helper variable argument
         * 
         */
        void removeConstraints(SimplifiedConstraintsHelper* helper);
        
        /**
         * @brief Sets a variable equal to the argument fixedVarValue, and removes it from the objective function and all constraints
         * 
         */
        void removeOneFixedVariable(uint varIndex, double fixedVarValue);

        /**
         * @brief Removes all fixed variables, as specified by the helper variable argument
         * 
         */
        void removeFixedVariables(SimplifiedConstraintsHelper* helper);

        /**
         * @brief Solves the LP model, after simplifying it(if possible)
         * 
         */
        void solveProblem();

        /**
         * @brief Checks if the status is equal to some value passed as an argument
         * 
         */
        bool operator==(ProblemStatus statusToCheck);
        
        /**
         * @brief Checks if the status is not equal to some value passed as an argument
         * 
         */
        bool operator!=(ProblemStatus statusToCheck);

        /**
         * @brief Sets the solution type field according to the optimal solution
         * 
         */
        void setSolutionType();

        // auxiliary methods for integer solutions

        /**
         * @brief Checks if all variables of the optimal solution are integers
         * 
         */
        bool isOptimalSolutionWhole();

        /**
         * @brief Copies all fields of problemToCopy and stores them in the caller
         * 
         */
        void operator=(LpProblem problemToCopy);

        /**
         * @brief Retrieves all the LP model's constraints
         * 
         */
        std::vector<Constraint> getConstraints();

        /**
         * @brief Retrieves the problem status
         * 
         */
        ProblemStatus getStatus();

};

/**
 * @brief Helper function for rounding integers that are within a margin of error(set by the epsilon variable)
 */
std::pair<bool, double> isDoubleAnInteger(double number, double epsilon = 1e-10);

#endif