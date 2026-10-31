#include "cli/cli.hpp"
#include <iostream>

#include "lp/lp.hpp"
#include "model_reader.hpp"
#include <string>
#include <vector>
#include "ip/bb_tree.hpp"

int main(int argc, char** argv) {
    CLI interface(argc, argv);
    interface.run();
    
    //LP::LpProblem problem = ModelFileReader::readModel("../models/model.lp");

    //std::vector<VariableType> varTypes = {VariableType::BINARY, VariableType::BINARY, VariableType::BINARY};
    //BaBTree tree(problem, varTypes);

    //tree.solveTree(ExplorationStrategy::BEST_VALUE, BranchingStrategy::BEST_COEFFICIENT, false);
    //tree.displayProblem();

    return 0;
}