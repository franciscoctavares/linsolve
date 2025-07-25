#include "../include/bb_node.h"

#include <cmath>
#include <iostream>

BaBNode::BaBNode(LpProblem nodeProblem) {
    problem = nodeProblem;
    leftChild = NULL;
    rightChild = NULL;
    status = EVALUATING;
}

Matrix BaBNode::branchLeft(uint varIndex, double varValue) {
    //std::cout << "Branched left" << std::endl;
    
    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().columns(), varIndex).getElements();
    Constraint newConstraint(newLhs, "<=", floor(varValue));
    LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    leftChild = new BaBNode(newProblem);

    return leftChild->solveNode();
}

Matrix BaBNode::branchRight(uint varIndex, double varValue) {
    //std::cout << "Branched right" << std::endl;

    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().columns(), varIndex).getElements();
    Constraint newConstraint(newLhs, ">=", ceil(varValue));
    LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    rightChild = new BaBNode(newProblem);

    return rightChild->solveNode();
}

bool BaBNode::operator==(NodeStatus statusToCheck) {
    return (status == statusToCheck) ? true : false;
}

std::pair<uint, double> BaBNode::getBranchVariableInfo() {
    Matrix currentSolution = problem.getOptimalSolution();

    for(uint i = 0; i < currentSolution.columns(); i++) {
        if(floor(currentSolution.getElement(0, i)) != currentSolution.getElement(0, i))
            return std::make_pair(i, currentSolution.getElement(0, i));
    }
}

Matrix BaBNode::compareChildrenSolutions(Matrix leftChildBestSol, Matrix rightChildBestSol) {
    Matrix leftSolution = leftChild->problem.getOptimalSolution();
    Matrix rightSolution = rightChild->problem.getOptimalSolution();
    Matrix objFun = problem.getObjectiveFunction();

    /*
    std::cout << "left branch solution: ";
    leftSolution.displayMatrix();
    std::cout << "right branch solution: ";
    rightSolution.displayMatrix();
    std::cout << std::endl;
    */

    Matrix unboundedSol = Matrix({INFINITY}, 1, 1);
    Matrix infeasibleSol = Matrix({0}, 1, 1);

    if(leftChildBestSol == unboundedSol || leftChildBestSol == infeasibleSol) {
        if(rightChildBestSol == unboundedSol || rightChildBestSol == infeasibleSol) return leftChildBestSol;
        else return rightChildBestSol;
    }
    else if(rightChildBestSol == unboundedSol || rightChildBestSol == infeasibleSol) return leftChildBestSol;
    else {
        double leftObjFunVal = leftChildBestSol.dotProduct(objFun);
        double rightObjFunVal = rightChildBestSol.dotProduct(objFun);
        if(problem.getType() == MAX) return (leftObjFunVal > rightObjFunVal) ? leftChildBestSol : rightChildBestSol;
        else if(problem.getType() == MIN) return (leftObjFunVal < rightObjFunVal) ? leftChildBestSol : rightChildBestSol;
    }
    /*
    //return objFun;
    // if both left and right branches are whole solutions
    if(leftChild->problem == WHOLE_SOLUTION && rightChild->problem == WHOLE_SOLUTION) {
        double leftObjFunVal = leftSolution.dotProduct(objFun);
        double rightObjFunVal = rightSolution.dotProduct(objFun);
        if(problem.getType() == MAX) return (leftObjFunVal > rightObjFunVal) ? leftSolution : rightSolution;
        else if(problem.getType() == MIN) return (leftObjFunVal < rightObjFunVal) ? leftSolution : rightSolution;
    }
    // if left branch is a whole solution and right branch is unbounded or infeasible
    else if(leftChild->problem == WHOLE_SOLUTION && rightChild->problem != WHOLE_SOLUTION) return leftSolution;
    //else if(leftChild->problem == WHOLE_SOLUTION && (rightChild->problem == INFEASIBLE || rightChild->problem == UNBOUNDED)) return leftSolution;
    // if left branch is unbounded or infeasible and right branch is a whole solution
    else if(leftChild->problem != WHOLE_SOLUTION && rightChild->problem == WHOLE_SOLUTION) return rightSolution;
    //else if((leftChild->problem == INFEASIBLE || leftChild->problem == UNBOUNDED) && rightChild->problem == WHOLE_SOLUTION) return rightSolution;
    // if neither branch is a whole solution
    else return leftSolution;
    */


}

Matrix BaBNode::solveNode() {
    problem.solveProblem();
    Matrix currentSolution = problem.getOptimalSolution();

    //if(problem == CONTINUOUS_SOLUTION) std::cout << "Continuous solution!" << std::endl;
    //else if(problem == WHOLE_SOLUTION) std::cout << "Whole solution!" << std::endl;

    if(problem == WHOLE_SOLUTION || problem == INFEASIBLE || problem == UNBOUNDED) return currentSolution;

    std::pair<uint, double> branchVarInfo = getBranchVariableInfo();

    Matrix leftChildBestSol = branchLeft(branchVarInfo.first, branchVarInfo.second);
    Matrix rightChildBestSol = branchRight(branchVarInfo.first, branchVarInfo.second);
    
    //Matrix bestSol = zeros(1, 1);
    Matrix bestSol = compareChildrenSolutions(leftChildBestSol, rightChildBestSol);

    if(leftChild->problem == WHOLE_SOLUTION || leftChild->problem == INFEASIBLE || leftChild->problem == UNBOUNDED) delete leftChild;
    if(rightChild->problem == WHOLE_SOLUTION || rightChild->problem == INFEASIBLE || rightChild->problem == UNBOUNDED) delete rightChild;

    return bestSol;

}