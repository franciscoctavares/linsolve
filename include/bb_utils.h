#ifndef BB_UTILS_H
#define BB_UTILS_H

#include "matrix.h"
#include <sys/types.h>

enum BranchingStrategy {
    FIRST_INDEX,
    RANDOM_VAR,
    BEST_COEFFICIENT,
};

enum ExplorationStrategy {
    EXPLORE_ALL_NODES,
    BEST_OBJECTIVE_FUNCTION_VALUE,
    WIDTH,
    DEPTH,
    RANDOM,
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

#endif