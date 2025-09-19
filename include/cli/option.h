#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <optional>

#include "cli/flag.h"

template<typename T>
struct Option {
    const Flag flag;
    const bool isArgRequired;
    const bool argForceRequired;
    const bool isExclusive;

    Option(const Flag& newFlag, bool argReq, bool argForce, bool exclusive) : flag(newFlag), isArgRequired(argReq), argForceRequired(argForce), isExclusive(exclusive) {}
};

#endif