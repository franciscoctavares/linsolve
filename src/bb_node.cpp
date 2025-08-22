#include "../include/bb_node.h"

#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>

BaBNode::BaBNode(LpProblem nodeProblem, uint newDepth) {
    problem = nodeProblem;
    leftChild = nullptr;
    rightChild = nullptr;
    status = NOT_EVALUATED;
    depth = newDepth;
}

BaBNode::BaBNode(const BaBNode& otherNode) {
    problem = otherNode.problem;
    leftChild = otherNode.leftChild;
    rightChild = otherNode.rightChild;
    status = otherNode.status;
    depth = otherNode.depth;
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

std::pair<uint, double> BaBNode::getBranchVariableInfo(BranchingStrategy branchStrat) {
    Matrix currentSolution = problem.getOptimalSolution();

    std::pair<uint, double> branchVarInfo;

    if(branchStrat == FIRST_INDEX) {
        for(uint i = 0; i < currentSolution.getNColumns(); i++) {
            if(floor(currentSolution.getElement(0, i)) != currentSolution.getElement(0, i)) {
                branchVarInfo = std::make_pair(i, currentSolution.getElement(0, i));
                break;
            }
        }
    }
    else if(branchStrat == RANDOM_VAR) {
        std::vector<uint> contVarsIndexes;
        for(uint i = 0; i < currentSolution.getNColumns(); i++) {
            if(!isNumberAnInteger(currentSolution.getElement(0, i))) contVarsIndexes.push_back(i);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, contVarsIndexes.size() - 1);

        int num = dist(gen);

        branchVarInfo = std::make_pair(contVarsIndexes[num], currentSolution.getElement(0, contVarsIndexes[num]));
    }
    else if(branchStrat == BEST_COEFFICIENT) {
        std::vector<uint> contVarsIndexes;
        for(uint i = 0; i < currentSolution.getNColumns(); i++) {
            if(!isNumberAnInteger(currentSolution.getElement(0, i))) contVarsIndexes.push_back(i);
        }

        Matrix objFun = problem.getObjectiveFunction();

        std::vector<double> contVarsCoeffs;
        for(uint i = 0; i < contVarsIndexes.size(); i++) {
            contVarsCoeffs.push_back(objFun.getElement(0, contVarsIndexes[i]));
        }

        if(problem.getType() == MAX) {
            size_t max_index;
            auto max_it = std::max_element(contVarsCoeffs.begin(), contVarsCoeffs.end());
            if (max_it != contVarsCoeffs.end()) {
                max_index = std::distance(contVarsCoeffs.begin(), max_it);
            }

            branchVarInfo = std::make_pair(contVarsIndexes[max_index], currentSolution.getElement(0, contVarsIndexes[max_index]));
        }
        else {
            size_t min_index;
            auto min_it = std::min_element(contVarsCoeffs.begin(), contVarsCoeffs.end());
            if (min_it != contVarsCoeffs.end()) {
                min_index = std::distance(contVarsCoeffs.begin(), min_it);
            }

            branchVarInfo = std::make_pair(contVarsIndexes[min_index], currentSolution.getElement(0, contVarsIndexes[min_index]));
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

BaBNode* BaBNode::branchLeft(int varIndex, double varValue) {
    if(varIndex < 0 || varIndex >= problem.getObjectiveFunction().getNColumns()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using branchRight: the model has " << problem.getObjectiveFunction().getNColumns()
                 << " variables, but the user tried to branch variable with index " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().getNColumns(), varIndex).getElements();
    Constraint newConstraint(newLhs, "<=", floor(varValue));
    LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    leftChild = new BaBNode(newProblem, depth + 1);

    return leftChild;
}

BaBNode* BaBNode::branchRight(int varIndex, double varValue) {
    if(varIndex < 0 || varIndex >= problem.getObjectiveFunction().getNColumns()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using branchRight: the model has " << problem.getObjectiveFunction().getNColumns()
                 << " variables, but the user tried to branch variable with index " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().getNColumns(), varIndex).getElements();
    Constraint newConstraint(newLhs, ">=", ceil(varValue));
    LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    rightChild = new BaBNode(newProblem, depth + 1);

    return rightChild;
}

void BaBNode::deleteSubNodes() {
    if(leftChild != NULL) {
        if(*leftChild == FATHOMED) delete leftChild;
        else leftChild->deleteSubNodes();
    }

    if(rightChild != NULL) {
        if(*rightChild == FATHOMED) delete rightChild;
        else rightChild->deleteSubNodes();
    }
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

uint BaBNode::getDepth() {
    return depth;
}