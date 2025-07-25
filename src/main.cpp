#include <iostream>

#include "../include/matrix.h"
#include "../include/constraint.h"
#include "../include/lp.h"
#include "../include/model_reader.h"
#include "../include/bb_node.h"
#include "../include/bb_tree.h"

using namespace std;

int main() {
    ModelFileReader reader;
    LpProblem model = reader.readModel("model_test.lp");

    BaBNode newHeadNode(model);
    BaBTree babTree;
    babTree.setHeadNode(&newHeadNode);

    Matrix optimalWholeSolution = babTree.solveTree();
    cout << "The solution to the IP model is: ";
    optimalWholeSolution.displayMatrix();
    return 0;
}