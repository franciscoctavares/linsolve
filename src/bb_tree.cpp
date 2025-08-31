#include "bb_tree.h"

#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>

// PRIVATE METHODS

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
            if(strategy != ExplorationStrategy::EXPLORE_ALL_NODES) {
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

void BaBTree::solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes) {
    for(uint i = 0; i < nodeQueue.size(); i++) {
        if(*nodeQueue[i] == NOT_EVALUATED) {
            nodeQueue[i]->solveNode();
            solvedNodes++;
        }
    }
}

void BaBTree::sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy) {
    if(strategy == ExplorationStrategy::BEST_VALUE) {
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            if(node1->getProblem().getType() == MAX) return node1->getObjectiveFunctionValue() > node2->getObjectiveFunctionValue();
            else return node1->getObjectiveFunctionValue() < node2->getObjectiveFunctionValue();
        });
    }
    else if(strategy == ExplorationStrategy::RANDOM_NODE) {
        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(nodeQueue.begin(), nodeQueue.end(), g);
    }
    else if(strategy == ExplorationStrategy::WIDTH) {
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            return node1->getDepth() < node2->getDepth();
        });
    }
    else if(strategy == ExplorationStrategy::DEPTH) {
        std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
            return node1->getDepth() > node2->getDepth();
        });
    }
}

// PUBLIC METHODS

BaBTree::BaBTree(LpProblem initialProblem) {
    headNode = new BaBNode(initialProblem, 0);
}

Matrix BaBTree::solveTree(ExplorationStrategy explorationStrat, BranchingStrategy branchingStrat) {
    auto start = std::chrono::steady_clock::now();

    std::vector<BaBNode*> nodeQueue;
    uint solvedNodes = 0;
    BaBNode* incumbentSolution = NULL;

    headNode->solveNode();
    //std::cout << "Solved a node, Z = " << headNode->getObjectiveFunctionValue() <<std::endl;
    solvedNodes++;

    if(*headNode == CONTINUOUS_SOLUTION) {
        std::pair<uint, double> branchVarInfo = headNode->getBranchVariableInfo(branchingStrat);
        nodeQueue.push_back(headNode->branchLeft(branchVarInfo.first, branchVarInfo.second));
        nodeQueue.push_back(headNode->branchRight(branchVarInfo.first, branchVarInfo.second));
    }
    else if(*headNode == WHOLE_SOLUTION) {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        metrics.execution_time = elapsed.count();
        metrics.explored_nodes = solvedNodes;
        metrics.optimalWholeSolution = headNode->getProblem().getOptimalSolution();
        metrics.optimalSolutionDepth = headNode->getDepth();
    
        return headNode->getProblem().getOptimalSolution();
    }

    do {
        solveNodeQueue(nodeQueue, solvedNodes);

        fathomLeafNodes(nodeQueue, explorationStrat, incumbentSolution);

        if(nodeQueue.size() == 0) break;
        sortNodeQueue(nodeQueue, explorationStrat);

        if(*nodeQueue[0] == CONTINUOUS_SOLUTION) {
            std::pair<uint, double> branchVarInfo = nodeQueue[0]->getBranchVariableInfo(branchingStrat);
            nodeQueue.push_back(nodeQueue[0]->branchLeft(branchVarInfo.first, branchVarInfo.second));
            nodeQueue.push_back(nodeQueue[0]->branchRight(branchVarInfo.first, branchVarInfo.second));
        }

        if(nodeQueue.size() != 0) nodeQueue.erase(nodeQueue.begin());

    }while(nodeQueue.size() > 0);

    auto end = std::chrono::steady_clock::now();
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
    for(uint i = 0; i < metrics.optimalWholeSolution.getNColumns(); i++) {
        std::cout << metrics.optimalWholeSolution.getElement(0, i);
        if(i < metrics.optimalWholeSolution.getNColumns() - 1) std::cout << ", ";
    }
    std::cout << "), Z = " << metrics.optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction()) << std::endl;
}

void BaBTree::deleteTree() {
    headNode->deleteSubNodes();

    delete headNode;
}

PerformanceMetrics BaBTree::getMetrics() {
    return metrics;
}


