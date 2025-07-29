#include "../include/bb_node.h"

#include <cmath>
#include <iostream>

BaBNode::BaBNode(LpProblem nodeProblem) {
    problem = nodeProblem;
    leftChild = NULL;
    rightChild = NULL;
    status = EVALUATING;
}

/*
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
*/

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
}

Matrix BaBNode::solveNode() {
    problem.solveProblem();
    Matrix currentSolution = problem.getOptimalSolution();

    //if(problem == CONTINUOUS_SOLUTION) std::cout << "Continuous solution!" << std::endl;
    //else if(problem == WHOLE_SOLUTION) std::cout << "Whole solution!" << std::endl;

    if(problem == WHOLE_SOLUTION || problem == INFEASIBLE || problem == UNBOUNDED) return currentSolution;

    std::pair<uint, double> branchVarInfo = getBranchVariableInfo();

    //Matrix leftChildBestSol = branchLeft(branchVarInfo.first, branchVarInfo.second);
    //Matrix rightChildBestSol = branchRight(branchVarInfo.first, branchVarInfo.second);
    
    //Matrix bestSol = compareChildrenSolutions(leftChildBestSol, rightChildBestSol);
    Matrix bestSol = zeros(1, problem.getObjectiveFunction().columns());

    if(leftChild->problem == WHOLE_SOLUTION || leftChild->problem == INFEASIBLE || leftChild->problem == UNBOUNDED) delete leftChild;
    if(rightChild->problem == WHOLE_SOLUTION || rightChild->problem == INFEASIBLE || rightChild->problem == UNBOUNDED) delete rightChild;

    return bestSol;
}

LpProblem BaBNode::getProblem() {
    return problem;
}

BaBNode* BaBNode::branchLeft(uint varIndex, double varValue) {
    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().columns(), varIndex).getElements();
    Constraint newConstraint(newLhs, "<=", floor(varValue));
    LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    leftChild = new BaBNode(newProblem);

    return leftChild;
}

BaBNode* BaBNode::branchRight(uint varIndex, double varValue) {
    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().columns(), varIndex).getElements();
    Constraint newConstraint(newLhs, ">=", ceil(varValue));
    LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    rightChild = new BaBNode(newProblem);

    return rightChild;
}

Matrix BaBNode::solveProblem() {
    problem.solveProblem();
    return problem.getOptimalSolution();
}

void BaBNode::deleteSubNodes() {
    if(leftChild == NULL) delete leftChild;
    else leftChild->deleteSubNodes();

    if(rightChild == NULL) delete rightChild;
    else rightChild->deleteSubNodes();
}