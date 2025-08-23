#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>
#include "bb_utils.h"
#include "bb_tree.h"
#include "bb_node.h"

enum FileExtension {
    LP,
};

typedef struct {
    std::string fileName;
    ExplorationStrategy explorationStrat;
    BranchingStrategy branchingStrat;
    bool displayResults;
}Command;

class CLI {
    private:
        std::vector<std::string> args;
        Command command;

        void validateCommand();

        void executeCommand();

        void benchmark();

    public:
        /**
         * @brief Constructor, Takes in `argc`(number of arguments) and `argv`(arguments), and stores them in `args`, converting them to std::vector<std::string>
         */
        CLI(int argc, char** argv);
};

#endif