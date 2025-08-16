#ifndef BB_TREE_H
#define BB_TREE_H

#include "bb_node.h"
#include "bb_utils.h"

class BaBTree {
    private:
        BaBNode* headNode;
        PerformanceMetrics metrics;
    public:
        /**
         * @brief BaBTree class constructor
         * 
         */
        BaBTree(BaBNode* newHeadNode);

        /**
         * @brief Sets a new headNode for the tree
         * 
         */
        void setHeadNode(BaBNode* newHeadNode);

        /**
         * @brief Given an exploration strategy and a branching strategy, solve the IP model using the branch and bound method
         * 
         */
        Matrix solveTree(ExplorationStrategy explorStrat, BranchingStrategy branchStrat);

        /**
         * @brief Displays the solution to the IP model, along with a few performance metrics
         * 
         */
        void displayProblem(Matrix optimalWholeSolution);

        /**
         * @brief Deletes all tree nodes
         * 
         */
        void deleteTree();

        /**
         * @brief Sorts the node queue(passed as an argument), according to an exploration strategy
         * 
         */
        void sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy);

        /**
         * @brief Solves all of the node queue's LP problems which aren't already solved
         * 
         */
        void solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes);

        /**
         * @brief Fathoms(cuts) all necessary leaf nodes and updates the incumbent solution if necessary
         * 
         */
        void fathomLeafNodes(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy, BaBNode*& incumbentSolution);

        /**
         * @brief Updates the incumbent solution if the candidate is better than it
         * 
         */
        void updateIncumbentSolution(BaBNode* candidate, BaBNode*& incumbentSolution);
};

#endif