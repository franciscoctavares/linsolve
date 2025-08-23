#include "../include/cli.h"

#include <filesystem>
#include <iostream>

#include "../include/model_reader.h"

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

        if(args[2] == "BEST_VALUE") command.explorationStrat = BEST_VALUE;
        else if(args[2] == "EXPLORE_ALL_NODES") command.explorationStrat = EXPLORE_ALL_NODES;
        else if(args[2] == "WIDTH") command.explorationStrat = WIDTH;
        else if(args[2] == "DEPTH") command.explorationStrat = DEPTH;
        else if(args[2] == "RANDOM_NODE") command.explorationStrat = RANDOM_NODE;
        else throw std::invalid_argument("Invalid exploration strategy");

        if(args[3] == "FIRST_INDEX") command.branchingStrat = FIRST_INDEX;
        else if(args[3] == "RANDOM_VAR") command.branchingStrat = RANDOM_VAR;
        else if(args[3] == "BEST_COEFFICIENT") command.branchingStrat = BEST_COEFFICIENT;
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
    LpProblem initialProblem = ModelFileReader::readModel("bench.lp");
    size_t n = 100;
    double avg_execution_time = 0;
    double total_execution_time = 0;

    for(int i = 0; i < n; i++) {
        BaBTree tree(initialProblem);
        Matrix optimalWholeSolution = tree.solveTree(BEST_VALUE, BEST_COEFFICIENT);
        
        avg_execution_time += tree.getMetrics().execution_time / n;
        total_execution_time += tree.getMetrics().execution_time;

        tree.deleteTree();
    }

    std::cout << "Iterations: " << n << std::endl;

    std::cout << "Total execution time: ";
    if(total_execution_time > 1000) std::cout << total_execution_time / 1000 << " s" << std::endl;
    else if(total_execution_time < 1) std::cout << total_execution_time * 1000 << " us" << std::endl;
    else std::cout << total_execution_time << " ms" << std::endl;

    std::cout << "Average execution time: ";
    if(avg_execution_time > 1000) std::cout << avg_execution_time / 1000 << " s" << std::endl;
    else if(avg_execution_time < 1) std::cout << avg_execution_time * 1000 << " us" << std::endl;
    else std::cout << avg_execution_time << " ms" << std::endl;
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