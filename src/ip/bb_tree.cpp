#include "bb_tree.hpp"

#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>

#include <format>
#include "tabulate.hpp"

#include <thread>

// PRIVATE METHODS

void BaBTree::fathomLeafNodes(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy, BaBNode*& incumbentSolution) {
	for(int i = nodeQueue.size() - 1; i >= 0; i--) {
		if(*nodeQueue[i] == LP::UNBOUNDED || *nodeQueue[i] == LP::INFEASIBLE) {
            //std::cout << "Node is non admissible" << std::endl;
			*nodeQueue[i] = FATHOMED;
			nodeQueue.erase(nodeQueue.begin() + i);
		}
		else if(*nodeQueue[i] == LP::WHOLE_SOLUTION) {
            //std::cout << "Node is whole" << std::endl;
			updateIncumbentSolution(nodeQueue[i], incumbentSolution);
			*nodeQueue[i] = FATHOMED;
			nodeQueue.erase(nodeQueue.begin() + i);
		}
		else if(*nodeQueue[i] == LP::CONTINUOUS_SOLUTION) {
            //std::cout << "Node is continuous" << std::endl;
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
	if(incumbentSolution == nullptr) incumbentSolution = candidate;
	else {
		if(candidate->isBetter(incumbentSolution)) incumbentSolution = candidate;
	}
}

void BaBTree::solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes, bool multithreading) {
    if(multithreading) {
        //std::cout << "Solving with multithreading..." << std::endl;
        std::thread t1(&BaBNode::solveNode, nodeQueue[nodeQueue.size() - 2]);
        std::thread t2(&BaBNode::solveNode, nodeQueue[nodeQueue.size() - 1]);

        t1.join();
        t2.join();
    }
    else {
        //std::cout << "Solving without multithreading..." << std::endl;
        nodeQueue[nodeQueue.size() - 2]->solveNode();
        nodeQueue[nodeQueue.size() - 1]->solveNode();
    }

	solvedNodes += 2;
}

void BaBTree::sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy) {
	if(strategy == ExplorationStrategy::BEST_VALUE) {
		std::sort(nodeQueue.begin(), nodeQueue.end(), [](BaBNode*& node1, BaBNode*& node2) {
			if(node1->getProblem().getType() == LP::MAX) return node1->getObjectiveFunctionValue() > node2->getObjectiveFunctionValue();
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

BaBTree::BaBTree(LP::LpProblem initialProblem) {
	headNode = new BaBNode(initialProblem, 0);
}

Matrix BaBTree::solveTree(ExplorationStrategy explorationStrat, BranchingStrategy branchingStrat, bool multithreading) {
	auto start = std::chrono::steady_clock::now();

	std::vector<BaBNode*> nodeQueue;
	uint solvedNodes = 0;
	BaBNode* incumbentSolution = NULL;

    //std::cout << "Gonna try to solve the head node..." << std::endl;
	headNode->solveNode();
    //std::cout << "Solved the head node..." << std::endl;
	solvedNodes++;

	if(*headNode == LP::CONTINUOUS_SOLUTION) {
        //std::cout << "Head node is a continuous solution" << std::endl;
        //headNode->getProblem().getOptimalSolution().displayMatrix();
		std::pair<uint, double> branchVarInfo = headNode->getBranchVariableInfo(branchingStrat);

        //headNode->getProblem().getOptimalSolution().displayMatrix();
        //std::cout << std::endl;
        //std::cout << std::format("Branching var = {}, Branching var value = {}", branchVarInfo.first, branchVarInfo.second) << std::endl;

		nodeQueue.push_back(headNode->branchLeft(branchVarInfo.first, branchVarInfo.second));
		nodeQueue.push_back(headNode->branchRight(branchVarInfo.first, branchVarInfo.second));
	}
	else if(*headNode == LP::WHOLE_SOLUTION) {
        //std::cout << "Head node is a whole solution" << std::endl;
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
        //std::cout << "Just finished solving nodes and so far, " << solvedNodes << " nodes were explored" << std::endl;

		fathomLeafNodes(nodeQueue, explorationStrat, incumbentSolution);
        //std::cout << "Just finished fathoming nodes" << std::endl;

		if(nodeQueue.size() == 0) break;
		sortNodeQueue(nodeQueue, explorationStrat);
        //std::cout << "Just finished sorting nodes" << std::endl;

		if(*nodeQueue[0] == LP::CONTINUOUS_SOLUTION) {
            //std::cout << "Going to branch..." << std::endl;
			std::pair<uint, double> branchVarInfo = nodeQueue[0]->getBranchVariableInfo(branchingStrat);
            //nodeQueue[0]->getProblem().getOptimalSolution().displayMatrix();
            //std::cout << std::endl;
            //std::cout << std::format("Branching var = {}, Branching var value = {}", branchVarInfo.first, branchVarInfo.second) << std::endl;
            //std::cout << "Got branching info" << std::endl;
			nodeQueue.push_back(nodeQueue[0]->branchLeft(branchVarInfo.first, branchVarInfo.second));
			nodeQueue.push_back(nodeQueue[0]->branchRight(branchVarInfo.first, branchVarInfo.second));
		}

		if(nodeQueue.size() != 0) nodeQueue.erase(nodeQueue.begin());

	}while(nodeQueue.size() > 0);

    //std::cout << std::endl << std::endl;

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
            std::cout << std::format("{:d}", static_cast<int>(metrics.optimalWholeSolution.getElement(0, i)));
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
