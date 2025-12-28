#include "bb_utils.hpp"

#include <cmath>

bool isNumberAnInteger(double number, double epsilon) {
    return std::abs(number - std::round(number)) < epsilon;
}