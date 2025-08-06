#ifndef BB_TREE_H
#define BB_TREE_H

#include "bb_node.h"
#include "bb_utils.h"

class BaBTree {
    private:
        BaBNode* headNode;
    public:
        BaBTree(BaBNode* newHeadNode);
        void setHeadNode(BaBNode* newHeadNode);
        Matrix solveTree();
        void displayProblem(Matrix optimalWholeSolution);

        // new branch methods
        std::vector<Matrix> newSolveTree();
        void deleteTree();
        void sortWholeSolutions(std::vector<Matrix>& wholeSolutions);
        void sortNodeQueue(std::vector<BaBNode*>& nodeQueue, BranchingStrategy strategy);
        void cutNodes(std::vector<BaBNode*>& nodeQueue, BaBNode* incumbentNode);
        bool isNodeWorthExploring(BaBNode* node, BaBNode* incumbentSolution);
};

#endif