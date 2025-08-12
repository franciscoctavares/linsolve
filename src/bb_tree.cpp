#include "../include/bb_tree.h"

#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>

BaBTree::BaBTree(BaBNode* newHeadNode) {
    headNode = newHeadNode;
}

void BaBTree::setHeadNode(BaBNode* newHeadNode) {
    headNode = newHeadNode;
}

Matrix BaBTree::solveTree(ExplorationStrategy explorStrat) {
    std::vector<BaBNode*> nodeQueue;
    uint solvedNodes = 0;
    BaBNode* incumbentSolution = NULL;

    headNode->solveNode();
    solvedNodes++;

    if(*headNode == CONTINUOUS_SOLUTION) {
        std::pair<uint, double> branchVarInfo = headNode->getBranchVariableInfo();
        nodeQueue.push_back(headNode->branchLeft(branchVarInfo.first, branchVarInfo.second));
        nodeQueue.push_back(headNode->branchRight(branchVarInfo.first, branchVarInfo.second));
    }

    do {
        solveNodeQueue(nodeQueue, solvedNodes);

        fathomLeafNodes(nodeQueue, explorStrat, incumbentSolution);

        sortNodeQueue(nodeQueue, explorStrat);

        if(*nodeQueue[0] == CONTINUOUS_SOLUTION) {
            std::pair<uint, double> branchVarInfo = nodeQueue[0]->getBranchVariableInfo();
            nodeQueue.push_back(nodeQueue[0]->branchLeft(branchVarInfo.first, branchVarInfo.second));
            nodeQueue.push_back(nodeQueue[0]->branchRight(branchVarInfo.first, branchVarInfo.second));
        }
        if(nodeQueue.size() != 0) nodeQueue.erase(nodeQueue.begin());

    }while(nodeQueue.size() > 0);


    std::cout << solvedNodes << " solved nodes" << std::endl;

    return incumbentSolution->getProblem().getOptimalSolution();
}

void BaBTree::displayProblem(Matrix optimalWholeSolution) {
    // Objective function
    if(headNode->getProblem().getType() == MAX) std::cout << "max z: ";
    else if(headNode->getProblem().getType() == MIN) std::cout << "min z: ";

    for(int i = 0; i < headNode->getProblem().getObjectiveFunction().columns(); i++) {
        if(headNode->getProblem().getObjectiveFunction().getElement(0, i) < 0) std::cout << "- ";
        else if(headNode->getProblem().getObjectiveFunction().getElement(0, i) > 0 && i > 0) std::cout << "+ ";

        if(headNode->getProblem().getObjectiveFunction().getElement(0, i) != 1 && floor(headNode->getProblem().getObjectiveFunction().getElement(0, i)) != headNode->getProblem().getObjectiveFunction().getElement(0, i)) 
            std::cout << std::setprecision(3) << std::fixed << headNode->getProblem().getObjectiveFunction().getElement(0, i);
        else if(headNode->getProblem().getObjectiveFunction().getElement(0, i) != 1 && 
                floor(headNode->getProblem().getObjectiveFunction().getElement(0, i) != 1) == headNode->getProblem().getObjectiveFunction().getElement(0, i) != 1)
            std::cout << unsigned(headNode->getProblem().getObjectiveFunction().getElement(0, i));
        std::cout << "x" << i + 1;

        if(i < headNode->getProblem().getObjectiveFunction().columns() - 1) std::cout << " ";
    }
    std::cout << std::endl << std::endl;;

    // Constraints
    std::cout << "subject to:" << std::endl;
    for(int i = 0; i < headNode->getProblem().getConstraints().size(); i++) {
        bool hasWritten = false;
        std::vector<double> lhs = headNode->getProblem().getConstraints()[i].getLhs();
        ConstraintType currentType = headNode->getProblem().getConstraints()[i].getType();
        double rhs = headNode->getProblem().getConstraints()[i].getRhs();
        for(int j = 0; j < lhs.size(); j++) {
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
    //std::cout << std::endl;

    //std::cout << "With ";
    for(uint i = 0; i < optimalWholeSolution.columns(); i++) {
        std::cout << "x" << i + 1;
        if(i < optimalWholeSolution.columns() - 1) std::cout << ", ";
    }
    std::cout << " must be integers" << std::endl << std::endl;

    if(headNode->getProblem().getStatus() == NOT_YET_SOLVED) std::cout << "The problem was not solved yet" << std::endl;
    else if(headNode->getProblem().getStatus() == INFEASIBLE) std::cout << "The problem is infeasible" << std::endl;
    else if(headNode->getProblem().getStatus() == UNBOUNDED) std::cout << "The problem is unbounded" << std::endl;
    else {
        std::cout << "The optimal solution is (";
        for(int i = 0; i < optimalWholeSolution.columns(); i++) {
            std::cout << "x" << i + 1;
            if(i < optimalWholeSolution.columns() - 1) std::cout << ", ";
        }
        std::cout << ") = (";
        for(int i = 0; i < optimalWholeSolution.columns(); i++) {
            if(floor(optimalWholeSolution.getElement(0, i)) == optimalWholeSolution.getElement(0, i)) std::cout << unsigned(optimalWholeSolution.getElement(0, i));
            else std::cout << std::setprecision(3) << std::fixed << optimalWholeSolution.getElement(0, i);
            if(i < optimalWholeSolution.columns() - 1) std::cout << ", ";
        }
        std::cout << "), and Z = ";
        if(floor(optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction())) == optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction())) 
            std::cout << unsigned(optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction())) << std::endl;
        else std::cout << std::setprecision(3) << std::fixed << optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction()) << std::endl;
    }
}

void BaBTree::deleteTree() {
    headNode->deleteSubNodes();
}

void BaBTree::sortWholeSolutions(std::vector<Matrix>& wholeSolutions) {
    Matrix objectiveFunction = headNode->getProblem().getObjectiveFunction();
    ProblemType probType = headNode->getProblem().getType();
    uint n = wholeSolutions.size();

    for(uint i = 0; i < n; i++) {
        for(uint j = 0; j < n - i - 1; j++) {
            if(probType == MAX)
                if(wholeSolutions[j].dotProduct(objectiveFunction) <= wholeSolutions[j + 1].dotProduct(objectiveFunction)) {
                    Matrix aux = wholeSolutions[j];
                    wholeSolutions[j] = wholeSolutions[j + 1];
                    wholeSolutions[j + 1] = aux;
                }
            else if(probType == MIN)
                if(wholeSolutions[j].dotProduct(objectiveFunction) >= wholeSolutions[j + 1].dotProduct(objectiveFunction)) {
                    Matrix aux = wholeSolutions[j];
                    wholeSolutions[j] = wholeSolutions[j + 1];
                    wholeSolutions[j + 1] = aux;
                }
        }
    }
}

void BaBTree::sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy) {
    ProblemType probType = headNode->getProblem().getType();
    if(strategy == BEST_OBJECTIVE_FUNCTION_VALUE) {
        /*
        uint n = nodeQueue.size();
        for(uint i = 0; i < n; i++) {
            for(uint j = 0; j < n - i - 1; j++) {
                if(probType == MAX) {
                    if(nodeQueue[j]->getObjectiveFunctionValue() < nodeQueue[j + 1]->getObjectiveFunctionValue()) {
                        BaBNode* aux = nodeQueue[j];
                        nodeQueue[j] = nodeQueue[j + 1];
                        nodeQueue[j + 1] = aux;
                    }
                }
                else if(probType == MIN) {
                    if(nodeQueue[j]->getObjectiveFunctionValue() > nodeQueue[j + 1]->getObjectiveFunctionValue()) {
                        BaBNode* aux = nodeQueue[j];
                        nodeQueue[j] = nodeQueue[j + 1];
                        nodeQueue[j + 1] = aux; 
                    }
                }
            }
        }
        */
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            if(node1->getProblem().getType() == MAX) return node1->getObjectiveFunctionValue() > node2->getObjectiveFunctionValue();
            else return node1->getObjectiveFunctionValue() < node2->getObjectiveFunctionValue();
        });
    }
}

void BaBTree::solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes) {
    for(uint i = 0; i < nodeQueue.size(); i++) {
        if(*nodeQueue[i] == NOT_EVALUATED) {
            nodeQueue[i]->solveNode();
            solvedNodes++;
        }
    }
}

void BaBTree::fathomLeafNodes(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy, BaBNode*& incumbentSolution) {
    for(int i = nodeQueue.size() - 1; i >= 0; i--) {
        if(*nodeQueue[i] == UNBOUNDED || *nodeQueue[i] == INFEASIBLE) {
            *nodeQueue[i] = FATHOMED;
            nodeQueue.erase(nodeQueue.begin() + i);
        }
        else if(*nodeQueue[i] == WHOLE_SOLUTION) {
            updateIncumbentSolution(nodeQueue[i], incumbentSolution);
            *nodeQueue[i] = FATHOMED;
            nodeQueue.erase(nodeQueue.begin() + i);
        }
        else if(*nodeQueue[i] == CONTINUOUS_SOLUTION) {
            if(strategy != EXPLORE_ALL_NODES) {
                if(incumbentSolution != NULL && !nodeQueue[i]->isBetter(incumbentSolution)) {
                    *nodeQueue[i] = FATHOMED;
                    nodeQueue.erase(nodeQueue.begin() + i);
                }
            }
        }
    }
}

void BaBTree::updateIncumbentSolution(BaBNode* candidate, BaBNode*& incumbentSolution) {
   if(incumbentSolution == NULL || candidate->isBetter(incumbentSolution)) incumbentSolution = candidate;
}