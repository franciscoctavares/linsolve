#include "cli/commands/solve.hpp"
#include <iostream>
#include "ip/bb_utils.hpp"
#include "ip/bb_tree.hpp"
#include "model_reader.hpp"
#include "lp/lp.hpp"

namespace Commands {

Solve::Solve(const std::string& newHelpMessage) : Command("solve", newHelpMessage) {
    Option helpOption({"-h", "--help"}, true, false, true, "Display this message");
    options.emplace_back(helpOption);

    Option fileOption({"-f", "--file"}, true, true, false, "The file containing the model");
    options.emplace_back(fileOption);

    Option threadsOption({std::nullopt, "--threads"}, true, false, false, "Enables multithreading");
    options.emplace_back(threadsOption);

    Option explorationOption({std::nullopt, "--explor"}, true, true, false, "The exploration strategy");
    options.emplace_back(explorationOption);

    Option branchingOption({std::nullopt, "--branch"}, true, true, false, "The branching strategy");
    options.emplace_back(branchingOption);

    Option resultsOption({std::nullopt, "--show"}, true, false, false, "Enables showing the results of the solver");
    options.emplace_back(resultsOption);
}

void Solve::prepareSettings() {
    settings.help = options[0].getIsUsed();

    if(!settings.help) {
        settings.fileName = options[1].getArg().value() + ".lp";

        settings.explorStrat = options[3].getArg().has_value() ? explorMap[options[3].getArg().value()] : explorMap["BEST_VALUE"];
        settings.branchStrat = options[4].getArg().has_value() ? branchMap[options[4].getArg().value()] : branchMap["BEST_COEFFICIENT"];
        
        settings.multithreading = options[2].getIsUsed();
        settings.showResults = options[5].getIsUsed() ? true : true;
    }
}

void Solve::runCommand() {
    prepareSettings();
    if(settings.help) {
        displayHelpMessage();
    }
    else {
        LP::LpProblem problem = ModelFileReader::readModel(settings.fileName);
        BaBTree tree(problem);
        Matrix optimalWholeSolution = tree.solveTree(settings.explorStrat, settings.branchStrat, settings.multithreading);

        if(settings.showResults) tree.displayProblem();
    }
}

}