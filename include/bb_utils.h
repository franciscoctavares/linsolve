#ifndef BB_UTILS_H
#define BB_UTILS_H

#include "matrix.h"
#include <sys/types.h>
#include <vector>
#include <string>


enum class BranchingStrategy {
    FIRST_INDEX,
    RANDOM_VAR,
    BEST_COEFFICIENT,
};

enum class ExplorationStrategy {
    EXPLORE_ALL_NODES,
    BEST_VALUE,
    WIDTH,
    DEPTH,
    RANDOM_NODE,
};

struct PerformanceMetrics {
    uint explored_nodes;
    double execution_time;
    Matrix optimalWholeSolution;
    uint optimalSolutionDepth;
};

std::vector<ExplorationStrategy> getPossibleExplorationStrategies();
std::vector<BranchingStrategy> getPossibleBranchingStrategies();

std::string convertExplorStratToString(ExplorationStrategy strat);
std::string convertBranchStratToString(BranchingStrategy strat);

#endif