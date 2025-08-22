#ifndef BB_TREE_H
#define BB_TREE_H

#include "bb_node.h"
#include "bb_utils.h"

class BaBTree {
    private:
        BaBNode* headNode;
        PerformanceMetrics metrics;

        /**
         * @brief Fathoms(cuts) all necessary leaf nodes and updates `incumbentSolution` if necessary
         */
        void fathomLeafNodes(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy, BaBNode*& incumbentSolution);

        /**
         * @brief Updates `incumbentSolution` if `candidate` has a better objective function value
         */
        void updateIncumbentSolution(BaBNode* candidate, BaBNode*& incumbentSolution);

        /**
         * @brief Solves all of the node queue's LP problems which aren't already solved
         */
        void solveNodeQueue(std::vector<BaBNode*>& nodeQueue, uint& solvedNodes);

        /**
         * @brief Sorts the `nodeQueue`, according to an exploration strategy, meaning that, after sorting,
         *        the first element of `nodeQueue` will be the next node to explore
         */
        void sortNodeQueue(std::vector<BaBNode*>& nodeQueue, ExplorationStrategy strategy);

    public:
        /**
         * @brief Standard constructor
         */
        BaBTree(BaBNode* newHeadNode = nullptr);

        /**
         * @brief Given an exploration strategy and a branching strategy, solve the IP model using the Branch and Bound method
         */
        Matrix solveTree(ExplorationStrategy explorationStrat, BranchingStrategy branchingStrat);

        /**
         * @brief Displays the `optimalWholeSolution` to the IP model, along with a few performance metrics
         */
        void displayProblem(Matrix optimalWholeSolution);

        /**
         * @brief Deletes all tree nodes
         */
        void deleteTree();
};

#endif