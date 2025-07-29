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

Matrix BaBTree::solveTree() {
    std::vector<BaBNode*> nodeQueue;
    nodeQueue.push_back(headNode);

    Matrix objectiveFunction = headNode->getProblem().getObjectiveFunction();

    std::vector<Matrix> wholeSolutions;

    uint solved_nodes = 0;
    uint max_iterations = 1000;

    for(uint i = 0; i < max_iterations; i++) {
        if(nodeQueue.size() == 0) break;
        nodeQueue[0]->solveProblem();
        solved_nodes++;
        std::pair<uint, double> branchVar = nodeQueue[0]->getBranchVariableInfo();
        if(nodeQueue[0]->getProblem() == CONTINUOUS_SOLUTION) {
            nodeQueue.push_back(nodeQueue[0]->branchLeft(branchVar.first, branchVar.second));
            nodeQueue.push_back(nodeQueue[0]->branchRight(branchVar.first, branchVar.second));
        }
        else if(nodeQueue[0]->getProblem() == WHOLE_SOLUTION) wholeSolutions.push_back(nodeQueue[0]->getProblem().getOptimalSolution());
        nodeQueue.erase(nodeQueue.begin());
    }

    std::cout << solved_nodes << " nodes explored" << std::endl;

    ProblemType typeAux = headNode->getProblem().getType();

    sortWholeSolutions(wholeSolutions);


    return wholeSolutions[0];
    //return headNode->solveNode();
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

void BaBTree::sortWholeSolutions(std::vector<Matrix> wholeSolutions) {
    Matrix objectiveFunction = headNode->getProblem().getObjectiveFunction();
    ProblemType probType = headNode->getProblem().getType();
    uint n = wholeSolutions.size();

    for(Matrix m : wholeSolutions) {
        m.displayMatrix();
        std::cout << std::endl;
    }

    for(uint i = 0; i < n; i++) {
        for(uint j = 0; j < n - i - 1; j++) {
            if(probType == MAX)
                if(wholeSolutions[j].dotProduct(objectiveFunction) < wholeSolutions[j + 1].dotProduct(objectiveFunction)) {
                    Matrix aux = wholeSolutions[j];
                    wholeSolutions[j] = wholeSolutions[j + 1];
                    wholeSolutions[j + 1] = aux;
                }
            else if(probType == MIN)
                if(wholeSolutions[j].dotProduct(objectiveFunction) > wholeSolutions[j + 1].dotProduct(objectiveFunction)) {
                    Matrix aux = wholeSolutions[j];
                    wholeSolutions[j] = wholeSolutions[j + 1];
                    wholeSolutions[j + 1] = aux;
                }
        }
    }
}