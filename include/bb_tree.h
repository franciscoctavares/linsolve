#ifndef BB_TREE_H
#define BB_TREE_H

#include "bb_node.h"

class BaBTree {
    private:
        BaBNode* headNode;
    public:
        //BaBTree(void) = default;
        BaBTree();
        BaBTree(BaBNode newHeadNode);
        void setHeadNode(BaBNode* newHeadNode);
        Matrix solveTree();
};

#endif