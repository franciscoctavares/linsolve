#include "../include/bb_node.h"

#include <cmath>
#include <iostream>

BaBNode::BaBNode(LpProblem nodeProblem) {
    problem = nodeProblem;
    leftChild = NULL;
    rightChild = NULL;
    status = NOT_EVALUATED;
}

bool BaBNode::operator==(NodeStatus statusToCheck) {
    return (status == statusToCheck) ? true : false;
}

bool BaBNode::operator==(ProblemStatus statusToCheck) {
    return problem == statusToCheck;
}

bool BaBNode::operator!=(ProblemStatus statusToCheck) {
    return problem != statusToCheck;
}

std::pair<uint, double> BaBNode::getBranchVariableInfo() {
    Matrix currentSolution = problem.getOptimalSolution();

    std::pair<uint, double> branchVarInfo;

    for(uint i = 0; i < currentSolution.columns(); i++) {
        if(floor(currentSolution.getElement(0, i)) != currentSolution.getElement(0, i)) {
            branchVarInfo = std::make_pair(i, currentSolution.getElement(0, i));
            break;
        }
    }

    return branchVarInfo;
}

Matrix BaBNode::solveNode() {
    problem.solveProblem();
    status = EVALUATED;
    return problem.getOptimalSolution();
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

void BaBNode::deleteSubNodes() {
    if(*leftChild == FATHOMED || leftChild == NULL) delete leftChild;
    else leftChild->deleteSubNodes();

    if(*rightChild == FATHOMED || rightChild == NULL) delete rightChild;
    else rightChild->deleteSubNodes();
}

void BaBNode::setNodeStatus(NodeStatus newStatus) {
    status = newStatus;
}

void BaBNode::operator=(NodeStatus newStatus) {
    status = newStatus;
}

double BaBNode::getObjectiveFunctionValue() {
    return problem.getOptimalSolution().dotProduct(problem.getObjectiveFunction());
}

bool BaBNode::isBetter(BaBNode* node) {
    ProblemType probType = problem.getType();

    if(node == NULL) return false;

    return (probType == MAX) ? getObjectiveFunctionValue() > node->getObjectiveFunctionValue() : getObjectiveFunctionValue() < node->getObjectiveFunctionValue();
}