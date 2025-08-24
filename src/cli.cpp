#include "../include/cli.h"

#include <filesystem>
#include <iostream>

#include "../include/model_reader.h"
#include "../include/cli/benchmark.h"

// PRIVATE METHODS

void CLI::validateCommand() {

    if(args.size() == 2) {
        if(args[1] != "--benchmark") throw std::invalid_argument("Invalid number of arguments");
    }
    else {
        if(args.size() != 5) {
            throw std::invalid_argument("Invalid number of arguments size 5");
        }
        
        // file doesn't exist
        if(!std::filesystem::exists(args[1] + ".lp")) {
            throw std::invalid_argument("Model file does not exist");
        }
        else command.fileName = args[1] + ".lp";

        if(args[2] == "BEST_VALUE") command.explorationStrat = ExplorationStrategy::BEST_VALUE;
        else if(args[2] == "EXPLORE_ALL_NODES") command.explorationStrat = ExplorationStrategy::EXPLORE_ALL_NODES;
        else if(args[2] == "WIDTH") command.explorationStrat = ExplorationStrategy::WIDTH;
        else if(args[2] == "DEPTH") command.explorationStrat = ExplorationStrategy::DEPTH;
        else if(args[2] == "RANDOM_NODE") command.explorationStrat = ExplorationStrategy::RANDOM_NODE;
        else throw std::invalid_argument("Invalid exploration strategy");

        if(args[3] == "FIRST_INDEX") command.branchingStrat = BranchingStrategy::FIRST_INDEX;
        else if(args[3] == "RANDOM_VAR") command.branchingStrat = BranchingStrategy::RANDOM_VAR;
        else if(args[3] == "BEST_COEFFICIENT") command.branchingStrat = BranchingStrategy::BEST_COEFFICIENT;
        else throw std::invalid_argument("Invalid branching strategy");

        if(args[4] == "--show") command.displayResults = true;
        else if(args[4] == "--quiet") command.displayResults = false;
        else throw std::invalid_argument("Invalid option: " + args[4]);
    }

}

void CLI::executeCommand() {
    if(args.size() == 2 && args[1] == "--benchmark") {
        benchmark();
        return;
    }

    LpProblem initialProblem = ModelFileReader::readModel(command.fileName);

    BaBTree tree(initialProblem);
    Matrix optimalWholeSolution = tree.solveTree(command.explorationStrat, command.branchingStrat);

    if(command.displayResults) tree.displayProblem(optimalWholeSolution);

    tree.deleteTree();
}

void CLI::benchmark() {
    Benchmark bench;
    bench.runBenchmark();
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