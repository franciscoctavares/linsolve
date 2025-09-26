#pragma once

#include <sys/types.h>
#include <vector>
#include <utility>

namespace LP {

#define M 1e6

enum ProblemType {
    MIN,
    MAX
};

struct SimplifierHelper {
    std::vector<std::pair<uint, double>> fixedVariables;
    std::vector<uint> constraintsToRemove;
    std::vector<std::pair<uint, uint>> pairsOfVars;

    bool isHelperEmpty() { return fixedVariables.size() == 0 && constraintsToRemove.size() == 0 && pairsOfVars.size() == 0; }

    void displayConstraintsToRemove();
};

enum SolvingMethod {
    SIMPLEX,
};

enum SolutionType {
    CONTINUOUS_SOLUTION,
    WHOLE_SOLUTION,
    INFEASIBLE,
    UNBOUNDED  
};

/**
 * @brief Checks if the the number argument is within a margin of error(epsilon argument) of being an integer
 * 
 */
bool isNumberAnInteger(double number, double epsilon = 1e-10);

}