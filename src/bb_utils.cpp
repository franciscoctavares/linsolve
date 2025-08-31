#include "bb_utils.h"

#include <cmath>

bool isNumberAnInteger(double number, double epsilon) {
    return std::abs(number - std::round(number)) < epsilon;
}

std::vector<ExplorationStrategy> getPossibleExplorationStrategies() {
    return {
        ExplorationStrategy::EXPLORE_ALL_NODES,
        ExplorationStrategy::BEST_VALUE,
        ExplorationStrategy::WIDTH,
        ExplorationStrategy::DEPTH,
        ExplorationStrategy::RANDOM_NODE
    };
}

std::vector<BranchingStrategy> getPossibleBranchingStrategies() {
    return {
        BranchingStrategy::FIRST_INDEX,
        BranchingStrategy::RANDOM_VAR,
        BranchingStrategy::BEST_COEFFICIENT
    };
}

std::string convertExplorStratToString(ExplorationStrategy strat) {
    std::string returnValue;
    switch (strat) {
    case ExplorationStrategy::EXPLORE_ALL_NODES:
        returnValue =  "EXPLORE_ALL_NODES";
        break;
    case ExplorationStrategy::BEST_VALUE:
        returnValue = "BEST_VALUE";
        break;
    case ExplorationStrategy::WIDTH:
        returnValue = "WIDTH";
        break;
    case ExplorationStrategy::DEPTH:
        returnValue = "DEPTH";
        break;
    case ExplorationStrategy::RANDOM_NODE:
        returnValue = "RANDOM_NODE";
        break;
    }

    return returnValue;
}

std::string convertBranchStratToString(BranchingStrategy strat) {
    std::string returnValue;
    switch (strat)
    {
    case BranchingStrategy::FIRST_INDEX:
        returnValue = "FIRST_INDEX";
        break;
    case BranchingStrategy::RANDOM_VAR:
        returnValue = "RANDOM_VAR";
        break;
    case BranchingStrategy::BEST_COEFFICIENT:
        returnValue = "BEST_COEFFICIENT";
        break;
    }

    return returnValue;
}