#include "bb_node.hpp"
#include "lp/lp_utils.hpp"

#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>
#include <format>
#include <cstddef>

BaBNode::BaBNode(const LP::LpProblem& nodeProblem, std::size_t newDepth) {
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

std::pair<uint, double> BaBNode::getBranchVariableInfo(BranchingStrategy branchStrat) {
    Matrix currentSolution = problem.getOptimalSolution();
    
    std::pair<uint, double> branchVarInfo;

    if(branchStrat == BranchingStrategy::FIRST_INDEX) {
        for(uint i = 0; i < currentSolution.getNColumns(); i++) {
            if(!LP::isNumberAnInteger(currentSolution.getElement(0, i))) {
                branchVarInfo = std::make_pair(i, currentSolution.getElement(0, i));
                break;
            }
        }
    }
    else if(branchStrat == BranchingStrategy::RANDOM_VAR) {
        std::vector<uint> contVarsIndexes;
        for(uint i = 0; i < currentSolution.getNColumns(); i++) {
            if(!LP::isNumberAnInteger(currentSolution.getElement(0, i))) contVarsIndexes.push_back(i);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, contVarsIndexes.size() - 1);

        int num = dist(gen);

        branchVarInfo = std::make_pair(contVarsIndexes[num], currentSolution.getElement(0, contVarsIndexes[num]));
    }
    else if(branchStrat == BranchingStrategy::BEST_COEFFICIENT) {

        Matrix objFun = problem.getObjectiveFunction();
        
        
        std::vector<std::pair<uint, double>> continuousVars;


        for(uint i = 0; i < currentSolution.getNColumns(); i++) {
            if(!LP::isNumberAnInteger(currentSolution.getElement(0, i))) continuousVars.push_back({i, objFun.getElement(0, i)});
        }

        if(problem.getType() == LP::MAX) {
            std::sort(continuousVars.begin(), continuousVars.end(), [](std::pair<uint, double>& a, std::pair<uint, double>& b) {
                return a.second > b.second;
            });
        }
        else
            std::sort(continuousVars.begin(), continuousVars.end(), [](std::pair<uint, double>& a, std::pair<uint, double>& b) {
                return a.second < b.second;
            });

        return std::make_pair(continuousVars[0].first, currentSolution.getElement(0, continuousVars[0].first));
    }

    return branchVarInfo;
}

Matrix BaBNode::solveNode() {
    problem.solveProblem(LP::SIMPLEX);
    status = EVALUATED;
    return problem.getOptimalSolution();
}

BaBNode* BaBNode::branchLeft(int varIndex, double varValue) {
    if(varIndex < 0 || static_cast<std::size_t>(varIndex) >= problem.getObjectiveFunction().getNColumns()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using branchRight: the model has " << problem.getObjectiveFunction().getNColumns()
                 << " variables, but the user tried to branch variable with index " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().getNColumns(), varIndex).getElements();
    Constraint newConstraint(newLhs, "<=", floor(varValue));
    LP::LpProblem newProblem = problem;
    newProblem.addConstraint(newConstraint);
    leftChild = new BaBNode(newProblem, depth + 1);

    return leftChild;
}

BaBNode* BaBNode::branchRight(int varIndex, double varValue) {
    if(varIndex < 0 || static_cast<std::size_t>(varIndex) >= problem.getObjectiveFunction().getNColumns()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using branchRight: the model has " << problem.getObjectiveFunction().getNColumns()
                 << " variables, but the user tried to branch variable with index " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }

    std::vector<double> newLhs = basisVector(problem.getObjectiveFunction().getNColumns(), varIndex).getElements();
    Constraint newConstraint(newLhs, ">=", ceil(varValue));
    LP::LpProblem newProblem = problem;
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

double BaBNode::getObjectiveFunctionValue() {
    return problem.getOptimalSolution().dotProduct(problem.getObjectiveFunction());
}

bool BaBNode::isBetter(BaBNode* otherNode) {
    if(otherNode == NULL) return false;

    if(problem.getType() == LP::MAX) {
        return getObjectiveFunctionValue() > otherNode->getObjectiveFunctionValue();
    }
    else return getObjectiveFunctionValue() < otherNode->getObjectiveFunctionValue();
}