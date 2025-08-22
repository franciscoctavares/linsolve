#include "../include/cli.h"

#include <filesystem>
#include <iostream>

#include "../include/model_reader.h"

// PRIVATE METHODS

void CLI::validateCommand() {
    if(args.size() != 4) {
        throw std::invalid_argument("Invalid number of arguments");
    }
    
    // file doesn't exist
    if(!std::filesystem::exists(args[1] + ".lp")) {
        throw std::invalid_argument("Model file does not exist");
    }
    else command.fileName = args[1] + ".lp";

    if(args[2] == "BEST_OBJECTIVE_FUNCTION_VALUE") command.explorationStrat = BEST_OBJECTIVE_FUNCTION_VALUE;
    else if(args[2] == "EXPLORE_ALL_NODES") command.explorationStrat = EXPLORE_ALL_NODES;
    else if(args[2] == "WIDTH") command.explorationStrat = WIDTH;
    else if(args[2] == "DEPTH") command.explorationStrat = DEPTH;
    else if(args[2] == "RANDOM") command.explorationStrat = RANDOM;
    else throw std::invalid_argument("Invalid exploration strategy");

    if(args[3] == "FIRST_INDEX") command.branchingStrat = FIRST_INDEX;
    else if(args[3] == "RANDOM_VAR") command.branchingStrat = RANDOM_VAR;
    else if(args[3] == "BEST_COEFFICIENT") command.branchingStrat = BEST_COEFFICIENT;
    else throw std::invalid_argument("Invalid branching strategy");

}

void CLI::executeCommand() {
    LpProblem initialProblem = ModelFileReader::readModel(command.fileName);

    BaBTree tree(initialProblem);
    Matrix optimalWholeSolution = tree.solveTree(command.explorationStrat, command.branchingStrat);

    tree.displayProblem(optimalWholeSolution);

    tree.deleteTree();
}

// PUBLIC METHODS

CLI::CLI(int argc, char** argv) {
    for(int i = 0; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }

    try {
        validateCommand();
        executeCommand();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}