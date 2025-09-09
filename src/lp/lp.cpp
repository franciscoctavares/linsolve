#include "lp/lp.h"
#include "lp/simplex.h"
#include "lp/lp_simplifier.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <format>
#include <cstddef>

// PRIVATE METHODS

namespace LP {

void LpProblem::simplifiedProblemSolution(SimplifierHelper& helper,  LpProblem& simplifiedProblem) {

    Matrix unboundedSol = Matrix({INFINITY}, 1, 1);
    Matrix infeasibleSol = Matrix({0}, 1, 1);

    if(simplifiedProblem.getOptimalSolution() == infeasibleSol) {
        solutionType = INFEASIBLE;
        optimalSolution = infeasibleSol;
        std::cout << "Both problems are infeasible" << std::endl;
        return;
    }
    else if(simplifiedProblem.getOptimalSolution() == unboundedSol) {
        solutionType = UNBOUNDED;
        optimalSolution = unboundedSol;
        std::cout << "Both problems are Unbounded" << std::endl;
        return;
    }

    if(helper.fixedVariables.size() == 0) optimalSolution = simplifiedProblem.getOptimalSolution();
    else {
        //std::cout << std::format("Some variables were fixed...({},{})", simplifiedProblem.getOptimalSolution().getNColumns(), helper.fixedVariables.size()) << std::endl;
        Matrix actualSolution = zeros(1, simplifiedProblem.getOptimalSolution().getNColumns() + helper.fixedVariables.size());

        for(const std::pair<uint, uint>& pairsOfVars : helper.pairsOfVars) {
            actualSolution.setElement(0, pairsOfVars.second, simplifiedProblem.getOptimalSolution().getElement(0, pairsOfVars.first));
        }

        for(const std::pair<uint, double>& fixedVars : helper.fixedVariables) {
            actualSolution.setElement(0, fixedVars.first, fixedVars.second);
        }

        optimalSolution = actualSolution;
    }

    if(simplifiedProblem.getSolutionType() == CONTINUOUS_SOLUTION) {
        solutionType = CONTINUOUS_SOLUTION;
        //std::cout << "Original problem is Continuous" << std::endl;
    }
    else if(simplifiedProblem.getSolutionType() == WHOLE_SOLUTION && helper.fixedVariables.size() != 0) {
        solutionType = WHOLE_SOLUTION;
        //std::cout << "Original problem is Whole" << std::endl;
    }

}

bool LpProblem::isOptimalSolutionWhole() {
    for(std::size_t i = 0; i < optimalSolution.getNColumns(); i++) {
        if(!isNumberAnInteger(optimalSolution.getElement(0, i)))
            return false;
    }
    return true;
}

// PUBLIC METHODS

LpProblem::LpProblem(ProblemType modelType, std::vector<double> newObjectiveFunction, std::vector<Constraint> newConstraints) {
    type = modelType;
    objectiveFunction = Matrix(newObjectiveFunction, 1, newObjectiveFunction.size());
    constraints = newConstraints;
    optimalSolution = zeros(1, objectiveFunction.getNColumns());
}

LpProblem::LpProblem(const LpProblem& problem) {
    type = problem.type;
    objectiveFunction = problem.objectiveFunction;
    constraints = problem.constraints;
    optimalSolution = problem.optimalSolution;
}

LpProblem& LpProblem::operator=(const LpProblem& otherProblem) {
    if(this != &otherProblem) {
        type = otherProblem.type;
        objectiveFunction = otherProblem.objectiveFunction;
        constraints = otherProblem.constraints;
        optimalSolution = otherProblem.optimalSolution;
    }
    return *this;
}

void LpProblem::displayProblem() {
    // Objective function
    if(type == MAX) std::cout << "max z: ";
    else if(type == MIN) std::cout << "min z: ";

    for(std::size_t i = 0; i < objectiveFunction.getNColumns(); i++) {
        double currentElement = objectiveFunction.getElement(0, i);

        if(currentElement < 0) std::cout << "- ";
        else if(currentElement > 0 && i > 0) std::cout << "+ ";

        if(currentElement != 1 && !isNumberAnInteger(currentElement))
            std::cout << std::format("{:.3f}\n", currentElement);
            //std::cout << std::setprecision(3) << std::fixed << objectiveFunction.getElement(0, i);
        else if(currentElement != 1 && isNumberAnInteger(currentElement))
            std::cout << std::format("{:d}", static_cast<std::size_t>(currentElement));
            //std::cout << unsigned(currentElement);
        std::cout << "x" << i + 1;

        if(i < objectiveFunction.getNColumns() - 1) std::cout << " ";
    }
    std::cout << std::endl << std::endl;;

    // Constraints
    std::cout << "subject to:" << std::endl;
    for(std::size_t i = 0; i < constraints.size(); i++) {
        bool hasWritten = false;
        std::vector<double> lhs = constraints[i].getLhs();
        ConstraintType currentType = constraints[i].getType();
        double rhs = constraints[i].getRhs();
        for(std::size_t j = 0; j < lhs.size(); j++) {
            if(lhs[j] != 0) {
                if(lhs[j] < 0) std::cout << "- ";
                else if(lhs[j] > 0 && j > 0 && hasWritten) std::cout << "+ ";

                if(lhs[j] != 1 && floor(lhs[j]) != lhs[j]) std::cout << std::setprecision(3) << std::fixed << fabs(lhs[j]);
                else if(lhs[j] != 1 && floor(lhs[j]) == lhs[j]) std::cout << unsigned(lhs[j]);

                std::cout << "x" << j + 1;
                hasWritten = true;
            }

            if(j < lhs.size() - 1) std::cout << " ";
        }

        if(currentType == LESS_THAN_OR_EQUAL) std::cout << " <= ";
        else if(currentType == GREATER_THAN_OR_EQUAL) std::cout << " >= ";
        else if(currentType == EQUAL) std::cout << " = ";

        if(rhs < 0) std::cout << "-";
        if(floor(rhs) == rhs) std::cout << unsigned(rhs);
        else std::cout << std::setprecision(3) << std::fixed << fabs(rhs);
        std::cout << std::endl;
    }
    std::cout << std::endl;

    if(solutionType == INFEASIBLE) std::cout << "The problem is infeasible" << std::endl;
    else if(solutionType == UNBOUNDED) std::cout << "The problem is unbounded" << std::endl;
    else {
        std::cout << "The optimal solution is (";
        for(std::size_t i = 0; i < optimalSolution.getNColumns(); i++) {
            std::cout << "x" << i + 1;
            if(i < optimalSolution.getNColumns() - 1) std::cout << ", ";
        }
        std::cout << ") = (";
        for(std::size_t i = 0; i < optimalSolution.getNColumns(); i++) {
            if(floor(optimalSolution.getElement(0, i)) == optimalSolution.getElement(0, i)) std::cout << unsigned(optimalSolution.getElement(0, i));
            else std::cout << std::setprecision(3) << std::fixed << optimalSolution.getElement(0, i);
            if(i < optimalSolution.getNColumns() - 1) std::cout << ", ";
        }
        std::cout << "), and Z = ";
        //if(floor(optimalSolution.dotProduct(objectiveFunction)) == optimalSolution.dotProduct(objectiveFunction))
        if(isNumberAnInteger(optimalSolution.dotProduct(objectiveFunction)))
            std::cout << unsigned(optimalSolution.dotProduct(objectiveFunction)) << std::endl;
        else std::cout << std::setprecision(3) << std::fixed << optimalSolution.dotProduct(objectiveFunction) << std::endl;
    }
}

void LpProblem::addConstraint(const Constraint& newConstraint) {
    constraints.push_back(newConstraint);
}

void LpProblem::removeConstraint(int constraintIndex) {
    if(constraintIndex < 0 || static_cast<std::size_t>(constraintIndex) >= constraints.size()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model only has " << constraints.size() << " constraints, but user tried to access constraint with index " << constraintIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    constraints.erase(constraints.begin() + constraintIndex);
}

void LpProblem::solveProblem(SolvingMethod method) {
    if(method == SIMPLEX) {
        LpProblem originalProblem(type, objectiveFunction.getElements(), constraints);

        SimplexSolver solver(originalProblem);

        std::pair<Matrix, SolutionType> results = solver.runSolver();

        optimalSolution = results.first;
        solutionType = results.second;
    }
}

bool LpProblem::isConstraintSatisfied(Matrix potentialSolution, int constraintIndex) {
    if(constraintIndex < 0 || static_cast<std::size_t>(constraintIndex) >= constraints.size()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model only has " << constraints.size() << " constraints, but user tried to access constraint with index " << constraintIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    Matrix constraintLhs(constraints[constraintIndex].getLhs(), 1, constraints[constraintIndex].getLhs().size());
    ConstraintType restType = constraints[constraintIndex].getType();
    double rhs = constraints[constraintIndex].getRhs();
    double value = potentialSolution.dotProduct(constraintLhs);
    
    if(restType == LESS_THAN_OR_EQUAL) return (value <= rhs) ? true : false;    // <=
    else if(restType == EQUAL) return (value == rhs) ? true : false;            // =
    else return (value >= rhs) ? true : false;                                  // >=
}

bool LpProblem::isSolutionAdmissible(Matrix potentialSolution) {
    for(std::size_t i = 0; i < constraints.size(); i++) {
        if(!isConstraintSatisfied(potentialSolution, i)) {
            //std::cout << "Constraint " << i << " not satisfied" << std::endl;
            return false;
        }
    }

    // non negativity
    for(std::size_t i = 0; i < potentialSolution.getNColumns(); i++) {
        if(potentialSolution.getElement(0, i) < 0) return false;
    }

    return true;
}

void LpProblem::removeFixedVariable(int varIndex, double fixedVarValue) {
    if(varIndex < 0 || static_cast<std::size_t>(varIndex) >= objectiveFunction.getNColumns()) {
        std::ostringstream errorMsg;
        errorMsg << "The LP model has " << objectiveFunction.getNColumns() << " variables, but the user tried to remove the variable with index " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    objectiveFunction.removeColumn(varIndex);

    for(std::size_t i = 0; i < constraints.size(); i++) {
        constraints[i].removeFixedVariable(varIndex, fixedVarValue);
    }
}

}