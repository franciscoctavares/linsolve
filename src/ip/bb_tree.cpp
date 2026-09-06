#include "bb_tree.hpp"

#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>

#include <format>
#include "single_include/tabulate/tabulate.hpp"

#include <thread>

// PRIVATE METHODS

void BaBTree::fathomLeafNodes(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy, BaBNode*& incumbentSolution) {
	for(int i = nodeQueue.size() - 1; i >= 0; i--) {
		if(*nodeQueue[i] == LP::UNBOUNDED || *nodeQueue[i] == LP::INFEASIBLE) {
			*nodeQueue[i] = FATHOMED;
			nodeQueue.erase(nodeQueue.begin() + i);
		}
		else if(nodeQueue[i]->isNodeDone(varTypes)) {
			updateIncumbentSolution(nodeQueue[i], incumbentSolution);
			*nodeQueue[i] = FATHOMED;
			nodeQueue.erase(nodeQueue.begin() + i);
		}
		else {
            if(!nodeQueue[i]->isNodeValid()) {
				*nodeQueue[i] = FATHOMED;
				nodeQueue.erase(nodeQueue.begin() + i);
                continue;
            }

			if(strategy != ExplorationStrategy::EXPLORE_ALL_NODES) {
				if(incumbentSolution != NULL && !nodeQueue[i]->isBetter(incumbentSolution)) {
					*nodeQueue[i] = FATHOMED;
					nodeQueue.erase(nodeQueue.begin() + i);
				}
			}
		}
		/*
		else if(*nodeQueue[i] == LP::WHOLE_SOLUTION) {
			updateIncumbentSolution(nodeQueue[i], incumbentSolution);
			*nodeQueue[i] = FATHOMED;
			nodeQueue.erase(nodeQueue.begin() + i);
		}
		else if(*nodeQueue[i] == LP::CONTINUOUS_SOLUTION) {
            if(!nodeQueue[i]->isNodeValid()) {
				*nodeQueue[i] = FATHOMED;
				nodeQueue.erase(nodeQueue.begin() + i);
                continue;
            }

			if(strategy != ExplorationStrategy::EXPLORE_ALL_NODES) {
				if(incumbentSolution != NULL && !nodeQueue[i]->isBetter(incumbentSolution)) {
					*nodeQueue[i] = FATHOMED;
					nodeQueue.erase(nodeQueue.begin() + i);
				}
			}
		}
		*/
	}
}

void BaBTree::updateIncumbentSolution(BaBNode* candidate, BaBNode*& incumbentSolution) {
	if(incumbentSolution == nullptr) incumbentSolution = candidate;
	else {
		if(candidate->isBetter(incumbentSolution)) incumbentSolution = candidate;
	}
}

void BaBTree::solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes, bool multithreading) {
    if(multithreading) {
        std::thread t1(&BaBNode::solveNode, nodeQueue[nodeQueue.size() - 2]);
        std::thread t2(&BaBNode::solveNode, nodeQueue[nodeQueue.size() - 1]);

        t1.join();
        t2.join();
    }
    else {
		//nodeQueue[nodeQueue.size() - 2]->getProblem().displayProblem();
		//std::cout << "\n\n";
		//nodeQueue[nodeQueue.size() - 1]->getProblem().displayProblem();
        nodeQueue[nodeQueue.size() - 2]->solveNode();
        nodeQueue[nodeQueue.size() - 1]->solveNode();
    }

	solvedNodes += 2;
}

void BaBTree::sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy) {
    switch (strategy) {
        case ExplorationStrategy::BEST_VALUE:
            std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
			    if(node1->getProblem().getType() == LP::MAX) return node1->getObjectiveFunctionValue() > node2->getObjectiveFunctionValue();
			    else return node1->getObjectiveFunctionValue() < node2->getObjectiveFunctionValue();
		    });
            break;
        case ExplorationStrategy::RANDOM_NODE: {
            std::random_device rd;
		    std::mt19937 g(rd());
		    std::shuffle(nodeQueue.begin(), nodeQueue.end(), g);
            break;
        }
        case ExplorationStrategy::WIDTH:
            std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
			    return node1->getDepth() < node2->getDepth();
	        });
            break;
        case ExplorationStrategy::DEPTH:
            std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
			    return node1->getDepth() > node2->getDepth();
		    });
            break;
        default:
            break;
    }
}

// PUBLIC METHODS

BaBTree::BaBTree(LP::LpProblem initialProblem, std::vector<VariableType> newVarTypes) {
	headNode = new BaBNode(initialProblem, 0);
	varTypes = newVarTypes;
}

Matrix BaBTree::solveTree(ExplorationStrategy explorationStrat, BranchingStrategy branchingStrat, bool multithreading) {
	auto start = std::chrono::steady_clock::now();

	std::vector<BaBNode*> nodeQueue;
	uint solvedNodes = 0;
	BaBNode* incumbentSolution = NULL;

	headNode->solveNode();
	solvedNodes++;

	/*
	if(*headNode == LP::CONTINUOUS_SOLUTION) {
		uint branchVarIndex = headNode->getBranchVariableInfo(branchingStrat, varTypes);

		nodeQueue.push_back(headNode->branchLeft(branchVarIndex, varTypes[branchVarIndex]));
		nodeQueue.push_back(headNode->branchRight(branchVarIndex, varTypes[branchVarIndex]));
	}
	else if(*headNode == LP::WHOLE_SOLUTION) {
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;

		metrics.execution_time = elapsed.count();
		metrics.explored_nodes = solvedNodes;
		metrics.optimalWholeSolution = headNode->getProblem().getOptimalSolution();
		metrics.optimalSolutionDepth = headNode->getDepth();
	
		return headNode->getProblem().getOptimalSolution();
	}
	*/

	if(!headNode->isNodeDone(varTypes)) {
		uint branchVarIndex = headNode->getBranchVariableInfo(branchingStrat, varTypes);

		nodeQueue.push_back(headNode->branchLeft(branchVarIndex, varTypes[branchVarIndex]));
		nodeQueue.push_back(headNode->branchRight(branchVarIndex, varTypes[branchVarIndex]));
	}
	else {
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;

		metrics.execution_time = elapsed.count();
		metrics.explored_nodes = solvedNodes;
		metrics.optimalWholeSolution = headNode->getProblem().getOptimalSolution();
		metrics.optimalSolutionDepth = headNode->getDepth();
	
		return headNode->getProblem().getOptimalSolution();
	}

	do {
		solveNodeQueue(nodeQueue, solvedNodes, multithreading);

		fathomLeafNodes(nodeQueue, explorationStrat, incumbentSolution);

		if(nodeQueue.size() == 0) break;

		sortNodeQueue(nodeQueue, explorationStrat);

		/*
		if(*nodeQueue[0] == LP::CONTINUOUS_SOLUTION) {
			uint branchVarIndex = nodeQueue[0]->getBranchVariableInfo(branchingStrat, varTypes);
			nodeQueue.push_back(nodeQueue[0]->branchLeft(branchVarIndex, varTypes[branchVarIndex]));
			nodeQueue.push_back(nodeQueue[0]->branchRight(branchVarIndex, varTypes[branchVarIndex]));
		}
		*/
		if(!nodeQueue[0]->isNodeDone(varTypes)) {
			uint branchVarIndex = nodeQueue[0]->getBranchVariableInfo(branchingStrat, varTypes);
			nodeQueue.push_back(nodeQueue[0]->branchLeft(branchVarIndex, varTypes[branchVarIndex]));
			nodeQueue.push_back(nodeQueue[0]->branchRight(branchVarIndex, varTypes[branchVarIndex]));	
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

void BaBTree::displayProblem() {
    tabulate::Table results;

    results.add_row({"Explored nodes", std::format("{}", metrics.explored_nodes)});
    results.add_row({"Optimal solution depth", std::format("{}", metrics.optimalSolutionDepth)});

    std::string time_str;

	if(metrics.execution_time > 1000) {
        time_str = std::format("{:.3f}", metrics.execution_time / 1000);
        time_str += " s";
    }
	else if(metrics.execution_time < 1) {
        time_str = std::format("{:.3f}", metrics.execution_time * 1000);
        time_str += " us";
    }
	else {
        time_str = std::format("{:.3f}", metrics.execution_time);
        time_str += " ms";
    }

    results.add_row({"Execution time", time_str});

    std::cout << results << std::endl;

	std::cout << "The optimal solution is: (";
	for(uint i = 0; i < metrics.optimalWholeSolution.getNColumns(); i++) {
        if(LP::isNumberAnInteger(metrics.optimalWholeSolution.getElement(0, i))) {
            std::cout << std::format("{:d}", static_cast<int>(std::round(metrics.optimalWholeSolution.getElement(0, i))));
        }
		else {
            std::cout << std::format("{:.3f}", metrics.optimalWholeSolution.getElement(0, i));
        }
		if(i < metrics.optimalWholeSolution.getNColumns() - 1) std::cout << ", ";
	}
	std::cout << "), Z = " << metrics.optimalWholeSolution.dotProduct(headNode->getProblem().getObjectiveFunction()) << std::endl;
}

void BaBTree::deleteTree() {
	headNode->deleteSubNodes();

	delete headNode;
}
