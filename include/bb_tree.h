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
        Matrix solveTree(ExplorationStrategy explorStrat);
        void displayProblem(Matrix optimalWholeSolution);

        // new branch methods
        void deleteTree();
        void sortWholeSolutions(std::vector<Matrix>& wholeSolutions);
        void sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy);

        void solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes);
        void fathomLeafNodes(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy, BaBNode*& incumbentSolution);
        void updateIncumbentSolution(BaBNode* candidate, BaBNode*& incumbentSolution);
};

#endif