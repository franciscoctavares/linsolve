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
         * @brief Standard constructor
         */
        BaBNode(const LpProblem& nodeProblem, uint newDepth);

        /**
         * @brief Copy constructor
         */
        BaBNode(const BaBNode& otherNode);

        /**
         * @brief Checks if the node's `status` is equal to `statusToCheck`
         */
        bool operator==(NodeStatus statusToCheck) { return status == statusToCheck; }

        /**
         * @brief Checks if the `status` of the node's problem is equal to `statusToCheck`
         */
        bool operator==(ProblemStatus statusToCheck) { return problem.getStatus() == statusToCheck; }

        /**
         * @brief Checks if the status of the node's problem is not equal to statusToCheck
         */
        bool operator!=(ProblemStatus statusToCheck) { return problem.getStatus() != statusToCheck; }

        /**
         * @brief Computes and returns the branching variable and its current value based on the branching strategy(`branchStrat`)
         */
        std::pair<uint, double> getBranchVariableInfo(BranchingStrategy branchStrat);

        /**
         * @brief Solves the node's LP `problem`
         */
        Matrix solveNode();

        /**
         * @brief Returns a reference to the node's LP problem
         */
        LpProblem& getProblem() { return problem; }

        /**
         * @brief Given the branching variable and its value, creates the left branch
         * 
         * @throw std::invalid_argument - if `varIndex` < 0 or `varIndex` >= `problem.getObjectiveFunction().getNColumns()`
         * 
         * @return pointer to the newly created node
         */
        BaBNode* branchLeft(int varIndex, double varValue);

        /**
         * @brief Given the branching variable and its value, creates the right branch
         * 
         * @throw std::invalid_argument - if `varIndex` < 0 or `varIndex` >= `problem.getObjectiveFunction().getNColumns()`
         * 
         * @return pointer to the newly created node
         */
        BaBNode* branchRight(int varIndex, double varValue);

        /**
         * @brief Delete all the caller's children nodes
         */
        void deleteSubNodes();

        /**
         * @brief Sets a new node `status`
         */
        void operator=(NodeStatus newStatus) { status = newStatus; }

        /**
         * @brief Assuming the node's `problem` is solved, returns the optimal solution's objective function value
         */
        double getObjectiveFunctionValue();

        /**
         * @brief Checks if the caller's solution is better than the argument node's solution
         */
        bool isBetter(BaBNode* node);

        /**
         * @brief Return the node's `depth`
         */
        uint getDepth() { return depth; }
};

#endif