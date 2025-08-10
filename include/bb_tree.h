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

        /**
         * @brief For continuous solutions, checks if it is worth to branch any further, based on the incumbent solution
         * 
         * @param node 
         * @param incumbentSolution 
         * @return true 
         * @return false 
         */
        bool isNodeWorthExploring(BaBNode* node, BaBNode* incumbentSolution);

        // trying new things
        /**
         * @brief After branching a node and adding both children to the node queue, this function checks 
         *        if it is worth exploring any of them, and fathoms the children not worth exploring
         * 
         * @param nodeQueue nodes left to explore
         * @param incumbentNode the current best whole solution
         */
        void areBranchesWorthExploring(std::vector<BaBNode*>& nodeQueue, BaBNode* incumbentNode);
};

#endif