#include "cli/option.hpp"
#include "errors.hpp"
#include "cli/cli_utils.hpp"

template<>
Option<MyVariant>::Option(const std::pair<std::optional<std::string>, std::optional<std::string>> newFlags, bool argReq, bool argForce, bool exclusive) 
    : flags(newFlags), argForce(argReq), argForceType(argForce), isExclusive(exclusive) {

    if(!flags.first.has_value() && !flags.second.has_value())
        throw NoFlagsException("A flag must have a short form or a long form. You provided none");
    
    // short form flag
    if(flags.first.has_value()) {
        if(flags.first.value().length() != 2)
            throw FlagFormatException("Short form flags must have 2 characters only");

        if(!flags.first.value().starts_with("-"))
            throw FlagFormatException("Short form flags must start with \'-\'");

        if(flags.first.value()[1] < 'a' || flags.first.value()[1] > 'z')
            throw FlagFormatException("Short form flags must have a lowercase letter as their second character");
    }

    if(flags.second.has_value()) {
        if(flags.second.value().length() <= 4)
            throw FlagFormatException("Long form flags must start with \'--\' and have at least 2 letters after that");

        if(!flags.second.value().starts_with("--"))
            throw FlagFormatException("Long form flags must start with \'--\'");

        for(std::size_t i = 2; i < flags.second.value().length(); i++) {
            if(flags.second.value()[i] < 'a' || flags.second.value()[i] > 'z')
                throw FlagFormatException("Long form flags must have all lowercase letters after the starting \'--\'");
        }
    }
}