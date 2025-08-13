#ifndef BB_UTILS_H
#define BB_UTILS_H

enum BranchingStrategy {
    FIRST_INDEX,
};

enum ExplorationStrategy {
    BEST_OBJECTIVE_FUNCTION_VALUE,
    EXPLORE_ALL_NODES,
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

#endif