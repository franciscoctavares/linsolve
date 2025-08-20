#ifndef BB_NODE_H
#define BB_NODE_H

#include "lp.h"
#include "bb_utils.h"

enum NodeStatus {
    NOT_EVALUATED,
    EVALUATED,
    FATHOMED,
};

class BaBNode {
    private:
        LpProblem problem;
        BaBNode* leftChild;
        BaBNode* rightChild;
        NodeStatus status;
        uint depth;
    public:
        BaBNode(void) = default;

        /**
         * @brief BaBNode class constructor
         * 
         */
        BaBNode(LpProblem nodeProblem, uint newDepth);

        /**
         * @brief Copy constructor
         * 
         */
        BaBNode(const BaBNode& otherNode);

        /**
         * @brief Checks if the status of the node is equal to statusToCheck
         * 
         */
        bool operator==(NodeStatus statusToCheck);

        /**
         * @brief Checks if the status of the node's problem is equal to statusToCheck
         */
        bool operator==(ProblemStatus statusToCheck);

        /**
         * @brief Checks if the status of the node's problem is not equal to statusToCheck
         */
        bool operator!=(ProblemStatus statusToCheck);

        /**
         * @brief Retrieves the branching variable and its current value based on the branching strategy
         *        , passed as an argument
         * 
         */
        std::pair<uint, double> getBranchVariableInfo(BranchingStrategy branchStrat);

        /**
         * @brief Solves the node's LP problem
         *  
         */
        Matrix solveNode();

        /**
         * @brief Retrieves the node's LP problem
         * 
         */
        LpProblem getProblem();

        /**
         * @brief Given the branching variable and its value, creates the left branch and returns a pointer to it
         * 
         */
        BaBNode* branchLeft(uint varIndex, double varValue);

        /**
         * @brief Given the branching variable and its value, creates the right branch and returns a pointer to it
         * 
         */
        BaBNode* branchRight(uint varIndex, double varValue);

        /**
         * @brief Delete all the caller's children nodes
         * 
         */
        void deleteSubNodes();

        /**
         * @brief Sets a new node status
         * 
         */
        void operator=(NodeStatus newStatus);

        /**
         * @brief Assuming the node's problem is solved, returns the optimal solution's objective function value
         * 
         */
        double getObjectiveFunctionValue();

        /**
         * @brief Checks if the caller's solution is better than the argument node's solution
         * 
         */
        bool isBetter(BaBNode* node);

        /**
         * @brief Retrieves the node's depth
         * 
         */
        uint getDepth();
};

#endif