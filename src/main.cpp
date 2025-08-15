#include <iostream>
#include <cmath>
#include <iomanip>

#include "../include/matrix.h"
#include "../include/constraint.h"
#include "../include/lp.h"
#include "../include/model_reader.h"
#include "../include/bb_node.h"
#include "../include/bb_tree.h"
#include "../include/bb_utils.h"

#include <string>

using namespace std;

int main(int argc, char** argv) {
    string fileName = argv[1];
    fileName += ".lp";

    string explorStratString = argv[2];
    ExplorationStrategy explorStrat;
    if(explorStratString == "BEST_OBJECTIVE_FUNCTION_VALUE") explorStrat = BEST_OBJECTIVE_FUNCTION_VALUE;
    else if(explorStratString == "EXPLORE_ALL_NODES") explorStrat = EXPLORE_ALL_NODES;
    else if(explorStratString == "WIDTH") explorStrat = WIDTH;
    else if(explorStratString == "DEPTH") explorStrat = DEPTH;
    else if(explorStratString == "RANDOM") explorStrat = RANDOM;

    string branchStratString = argv[3];
    BranchingStrategy branchStrat;
    if(branchStratString == "FIRST_INDEX") branchStrat = FIRST_INDEX;
    else if(branchStratString == "RANDOM_VAR") branchStrat = RANDOM_VAR;
    else if(branchStratString == "BEST_COEFFICIENT") branchStrat = BEST_COEFFICIENT;

    ModelFileReader reader;
    LpProblem model = reader.readModel(fileName);

    BaBNode newHeadNode(model, 0);
    BaBTree babTree(&newHeadNode);

    Matrix optimalWholeSolution = babTree.solveTree(explorStrat, branchStrat);
    babTree.displayProblem(optimalWholeSolution);

    babTree.deleteTree();

    return 0;
}