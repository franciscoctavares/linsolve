#include <iostream>
#include <cmath>
#include <iomanip>

#include "../include/matrix.h"
#include "../include/constraint.h"
#include "../include/lp.h"
#include "../include/model_reader.h"
#include "../include/bb_node.h"
#include "../include/bb_tree.h"

#include <string>

using namespace std;

int main(int argc, char** argv) {
    string fileName = argv[1];
    fileName += ".lp";
    
    string explorStratString = argv[2];
    ExplorationStrategy strat;
    if(explorStratString == "BEST_OBJECTIVE_FUNCTION_VALUE") strat = BEST_OBJECTIVE_FUNCTION_VALUE;
    else if(explorStratString == "EXPLORE_ALL_NODES") strat = EXPLORE_ALL_NODES;
    else if(explorStratString == "WIDTH") strat = WIDTH;
    else if(explorStratString == "DEPTH") strat = DEPTH;
    else if(explorStratString == "RANDOM") strat = RANDOM;

    ModelFileReader reader;
    LpProblem model = reader.readModel(fileName);

    BaBNode newHeadNode(model, 0);
    BaBTree babTree(&newHeadNode);

    Matrix optimalWholeSolution = babTree.solveTree(strat);
    babTree.displayProblem(optimalWholeSolution);

    babTree.deleteTree();
    return 0;
}