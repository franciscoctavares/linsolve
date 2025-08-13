#include "../include/bb_tree.h"

#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include <random>
#include <chrono>

BaBTree::BaBTree(BaBNode* newHeadNode) {
    headNode = newHeadNode;
}

void BaBTree::setHeadNode(BaBNode* newHeadNode) {
    headNode = newHeadNode;
}

Matrix BaBTree::solveTree(ExplorationStrategy explorStrat) {
    auto start = std::chrono::high_resolution_clock::now();

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
        //std::cout << "So far, " << solvedNodes << " nodes have been explored" << std::endl;
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


    //std::cout << solvedNodes << " solved nodes" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    metrics.execution_time = elapsed.count();
    metrics.explored_nodes = solvedNodes;
    metrics.optimalWholeSolution = incumbentSolution->getProblem().getOptimalSolution();
    metrics.optimalSolutionDepth = incumbentSolution->getDepth();

    return incumbentSolution->getProblem().getOptimalSolution();
}

void BaBTree::displayProblem(Matrix optimalWholeSolution) {
    std::cout << "Explored nodes: " << metrics.explored_nodes << std::endl;
    std::cout << "The optimal solution is located at depth " << metrics.optimalSolutionDepth << std::endl;
    std::cout << "Execution time: "; //<< metrics.execution_time << " ms" << std::endl;
    if(metrics.execution_time > 1000) std::cout << metrics.execution_time / 1000 << " s" << std::endl;
    else if(metrics.execution_time < 1) std::cout << metrics.execution_time * 1000 << " us" << std::endl;
    else std::cout << metrics.execution_time << " ms" << std::endl;

    std::cout << "The optimal solution is: (";
    for(uint i = 0; i < metrics.optimalWholeSolution.columns(); i++) {
        std::cout << metrics.optimalWholeSolution.getElement(0, i);
        if(i < metrics.optimalWholeSolution.columns() - 1) std::cout << ", ";
    }
    std::cout << "), Z = " << metrics.optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction()) << std::endl;
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
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            if(node1->getProblem().getType() == MAX) return node1->getObjectiveFunctionValue() > node2->getObjectiveFunctionValue();
            else return node1->getObjectiveFunctionValue() < node2->getObjectiveFunctionValue();
        });
    }
    else if(strategy == RANDOM) {
        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(nodeQueue.begin(), nodeQueue.end(), g);
    }
    else if(strategy == WIDTH) {
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            return node1->getDepth() < node2->getDepth();
        });
    }
    else if(strategy == DEPTH) {
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            return node1->getDepth() > node2->getDepth();
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