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

typedef struct {
    uint explored_nodes;
    double execution_time;
    Matrix optimalWholeSolution;
    uint optimalSolutionDepth;
}PerformanceMetrics;

/**
 * @brief Checks if the the number argument is within a margin of error(epsilon argument) of being an integer
 * 
 */
bool isNumberAnInteger(double number, double epsilon = 1e-10);

std::vector<ExplorationStrategy> getPossibleExplorationStrategies();
std::vector<BranchingStrategy> getPossibleBranchingStrategies();

std::string convertExplorStratToString(ExplorationStrategy strat);
std::string convertBranchStratToString(BranchingStrategy strat);

#endif