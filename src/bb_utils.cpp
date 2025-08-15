#include "../include/bb_utils.h"

#include <cmath>

bool isNumberAnInteger(double number, double epsilon) {
    return std::abs(number - std::round(number)) < epsilon;
}