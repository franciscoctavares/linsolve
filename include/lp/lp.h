#ifndef LP_H
#define LP_H

#include "matrix.h"
#include "constraint.h"

#include <vector>
#include <string>
#include <tuple>

#include <limits>

#include "lp/lp_utils.h"
#include "lp/lp.h"

namespace LP {

class LpProblem {
    private:
        ProblemType type;
        Matrix objectiveFunction;
        std::vector<Constraint> constraints;
        Matrix optimalSolution;
        SolutionType solutionType;

        /**
         * @brief Checks if the LP problem can be simplified, and stores information about which constraints to remove 
         *        and variables to fix in the `helper` variable passed as an argument
         */
        bool canProblemBeSimplified(SimplifierHelper* helper);

        /**
         * @brief Uses the information stored in the `helper` variable argument to simplify the problem
         */
        bool simplifyProblem(SimplifierHelper* helper);

        /**
         * @brief Uses the information stored in the `helper` variable argument and the simplified problem solution(`simplifiedSolution`)
         *        to obtain the initial problem's solution
         */
        void simplifiedProblemSolution(SimplifierHelper& helper, LpProblem& simplifiedSolution);

        bool isOptimalSolutionWhole();
        
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
         * @brief Displays the LP model and its optimal solution on the terminal
         */
        void displayProblem();
        
        /**
         * @brief Adds `newConstraint` to the LP model
         */
        void addConstraint(const Constraint& newConstraint);

        /**
         * @brief Removes the constraint with index `constraintIndex`
         * 
         * @throw std::invalid_argument - if `constraintIndex` < 0 or `constraintIndex` >= `constraints.size()`
         */
        void removeConstraint(int constraintIndex);

        /**
         * @brief Returns `optimalSolution`
         */
        Matrix& getOptimalSolution() { return optimalSolution; }

        /**
         * @brief Returns the LP model type(MAX or MIN)
         */
        ProblemType getType() { return type; }

        /**
         * @brief Returns `objectiveFunction`
         */
        Matrix& getObjectiveFunction() { return objectiveFunction; }

        /**
         * @brief Solves the LP model
         */
        void solveProblem(SolvingMethod method);

        std::vector<Constraint>& getConstraints() { return constraints; }

        SolutionType getSolutionType() { return solutionType; }
        void setSolutionType(SolutionType newSolutionType) { solutionType = newSolutionType; }
        
        void setOptimalSolution(Matrix newOptimalSolution) { optimalSolution = newOptimalSolution; }

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
         * @brief Sets the variable whose index is `varIndex` equal to `fixedVarValue`, and removes it from the objective function and all constraints
         * 
         * @throw std::invalid_argument - if `varIndex` < 0 or `varIndex` >= `objectiveFunction.getNColumns()`
         */
        void removeFixedVariable(int varIndex, double fixedVarValue);

};

}

#endif