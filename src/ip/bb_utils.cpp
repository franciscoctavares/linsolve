#include "bb_utils.hpp"

#include <cmath>
#include <stdexcept>

bool isNumberAnInteger(double number, double epsilon) {
    return std::abs(number - std::round(number)) < epsilon;
}