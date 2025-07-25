#include "../include/bb_tree.h"

BaBTree::BaBTree() {
    headNode = NULL;
}

void BaBTree::setHeadNode(BaBNode* newHeadNode) {
    headNode = newHeadNode;
}

Matrix BaBTree::solveTree() {
    return headNode->solveNode();
}