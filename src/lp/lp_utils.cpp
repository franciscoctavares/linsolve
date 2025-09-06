#include "lp/lp_utils.h"

#include <cmath>
#include <iostream>

namespace LP {

bool isNumberAnInteger(double number, double epsilon) {
    return std::fabs(number - std::round(number)) < epsilon;
}

void SimplifierHelper::displayConstraintsToRemove() {
    if(constraintsToRemove.size() == 0) {
        std::cout << "There are no constraints to remove" << std::endl;
    }

    if(fixedVariables.size() == 0) {
        std::cout << "There are no fixed variables" << std::endl;
    }

    if(pairsOfVars.size() == 0) {
        std::cout << "There are no pairs of vars" << std::endl;
    }
    else std::cout << "There are some pairs of vars" << std::endl;

    std::cout << "Going to remove the following constraints: ";
    for(int i = 0; i < constraintsToRemove.size(); i++) {
        std::cout << constraintsToRemove[i];
        if(i < constraintsToRemove.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

}