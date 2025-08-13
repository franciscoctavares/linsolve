#include <iostream>
#include <cmath>
#include <iomanip>

#include "../include/matrix.h"
#include "../include/constraint.h"
#include "../include/lp.h"
#include "../include/model_reader.h"
#include "../include/bb_node.h"
#include "../include/bb_tree.h"

using namespace std;

int main() {
    ModelFileReader reader;
    LpProblem model = reader.readModel("model_3.lp");

    BaBNode newHeadNode(model, 0);
    BaBTree babTree(&newHeadNode);

    Matrix optimalWholeSolution = babTree.solveTree(DEPTH);
    babTree.displayProblem(optimalWholeSolution);

    babTree.deleteTree();
    return 0;
}