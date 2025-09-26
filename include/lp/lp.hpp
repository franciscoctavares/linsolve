#ifndef LP_H
#define LP_H

#include "matrix.hpp"
#include "constraint.hpp"

#include <vector>
#include <string>
#include <tuple>

#include <limits>

#include "lp/lp_utils.hpp"
#include "lp/lp.hpp"

namespace LP {

class LpProblem {
    private:
        ProblemType type;
        Matrix objectiveFunction;
        std::vector<Constraint> constraints;
        Matrix optimalSolution;
        SolutionType solutionType;
        
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

        const Matrix& getObjectiveFunction() const { return objectiveFunction; }

        Matrix& getOptimalSolution() { return optimalSolution; }

        const std::vector<Constraint>& getConstraints() const { return constraints; }
        
        ProblemType getType() const { return type; }

        void solveProblem(SolvingMethod method);

        SolutionType getSolutionType() const { return solutionType; }

        void setSolutionType(SolutionType newSolutionType) { solutionType = newSolutionType; }
        
        void setOptimalSolution(const Matrix& newOptimalSolution) { optimalSolution = newOptimalSolution; }

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