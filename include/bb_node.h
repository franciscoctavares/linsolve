#ifndef BB_NODE_H
#define BB_NODE_H

#include "lp.h"

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

    public:
        BaBNode(void) = default;
        BaBNode(LpProblem nodeProblem);

        /**
         * @brief returns true if the status of the caller's node is equal to statusToCheck 
         * 
         */
        bool operator==(NodeStatus statusToCheck);

        bool operator==(ProblemStatus statusToCheck);
        bool operator!=(ProblemStatus statusToCheck);

        std::pair<uint, double> getBranchVariableInfo();

        /**
         * @brief Solves the node's LP problem
         *  
         */
        Matrix solveNode();

        LpProblem getProblem();

        // new branch methods
        BaBNode* branchLeft(uint varIndex, double varValue);
        BaBNode* branchRight(uint varIndex, double varValue);
        void deleteSubNodes();
        void setNodeStatus(NodeStatus newStatus);
        double getObjectiveFunctionValue(); // assumes the node's problem is solved
        bool isBetter(BaBNode node);
};

#endif