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
        // Experimental stuff
        Matrix bestChildSolution;

    public:
        BaBNode(void) = default;
        BaBNode(LpProblem nodeProblem);
        //Matrix branchLeft(uint varIndex, double varValue);
        //Matrix branchRight(uint varIndex, double varValue);

        /**
         * @brief returns true if the status of the caller's node is equal to statusToCheck 
         * 
         */
        bool operator==(NodeStatus statusToCheck);

        std::pair<uint, double> getBranchVariableInfo();

        Matrix compareChildrenSolutions(Matrix leftChildBestSol, Matrix rightChildBestSol);

        /**
         * @brief Solves the node's current problem and all it's subproblems, and returns the best whole solution
         * 
         */
        Matrix solveNode();

        LpProblem getProblem();

        // new branch methods
        BaBNode* branchLeft(uint varIndex, double varValue);
        BaBNode* branchRight(uint varIndex, double varValue);
        Matrix solveProblem();
        void deleteSubNodes();
        void setNodeStatus(NodeStatus newStatus);
        double getObjectiveFunctionValue(); // assumes the node's problem is solved
};

#endif