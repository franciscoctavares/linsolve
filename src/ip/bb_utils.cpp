#include "bb_utils.hpp"

#include <cmath>

bool isNumberAnInteger(double number, double epsilon = 1e-10) {
    return std::abs(number - std::round(number)) < epsilon;
}

bool doesVariableComplyWithIntegralityConstraints(double var, VariableType type) {
    switch(type) {
        case VariableType::CONTINUOUS:
            return true;
        case VariableType::INTEGER:
            return isNumberAnInteger(var);
        case VariableType::BINARY:
            return isNumberAnInteger(var) && (std::round(var) == 0 || std::round(var) == 1);
    }
    
    return false;
    /*
    if(type == VariableType::CONTINUOUS) return true;
    else if(type == VariableType::INTEGER) return isNumberAnInteger(var);
    else if(type == VariableType::BINARY) return isNumberAnInteger(var) && (std::round(var) == 0 || std::round(var) == 1);
    */
}