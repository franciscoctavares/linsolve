#pragma once

#include "cli/command.hpp"
#include <string>
#include "ip/bb_utils.hpp"

namespace Commands {

struct SolverSettings {
    bool help;

    std::string fileName;
    ExplorationStrategy explorStrat;
    BranchingStrategy branchStrat;
    bool multithreading;
    bool showResults;
};

class Solve : public Command {
    public:
        Solve(const std::string& newHelpMessage);
        void prepareSettings();
        void runCommand() override;
    private:
        SolverSettings settings;
};

}