#include "cli/flag.h"
#include "errors.h"

#include <unordered_map>
#include <vector>
#include <format>

#include "cli/cli_utils.h"

Flag::Flag(const std::optional<std::string> shortF, const std::optional<std::string> longF) : shortForm(shortF), longForm(longF) {
    if(!shortForm.has_value() && !longForm.has_value())
        throw NoFlagsException("A flag must have a short form or a long form. You provided none");

    if(shortForm.has_value()) {
        if(shortForm.value().length() != 2)
            throw FlagFormatException("Short form flags must have 2 characters only");

        if(shortForm.value()[0] != '-')
            throw FlagFormatException("Short form flags must have \'-\' as their first character");

        if(shortForm.value()[1] < 'a' || shortForm.value()[1] > 'z')
            throw FlagFormatException("Short form flags must have a lowercase letter as their second character");
    }

    if(longForm.has_value()) {
        if(longForm.value().length() <= 4)
            throw FlagFormatException("Long form flags must start with \'--\' and have at least 2 letters after that");

        if(longForm.value()[0] != '-' || longForm.value()[1] != '-')
            throw FlagFormatException("Long form flags must start with \'--\'");

        for(std::size_t i = 2; i < longForm.value().length(); i++) {
            if(longForm.value()[i] < 'a' || longForm.value()[i] > 'z')
                throw FlagFormatException("Long form flags must have all lowercase letters after the starting \'--\'");
        }
    }
}

bool Flag::parseFlag(const std::vector<std::string>& args) const {
    std::unordered_map<std::string, int> counts;
    
    if(shortForm.has_value()) counts[shortForm.value()] = 0;
    if(longForm.has_value()) counts[longForm.value()] = 0;

    for(const std::string& str : args) {
        if(str == shortForm.value()) counts[shortForm.value()]++;
        else if(str == longForm.value()) counts[longForm.value()]++;
    }

    if(shortForm.has_value() && longForm.has_value()) {
        if(counts[shortForm.value()] > 0 && counts[longForm.value()] > 0)
            throw SameFlagsException(std::format("{}Error{}: using both flag versions('-' and '--') is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
        else if(counts[shortForm.value()] > 1)
            throw SameFlagsException(std::format("{}Error{}: using repeated flags is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
        else if(counts[longForm.value()] > 1)
            throw SameFlagsException(std::format("{}Error{}: using repeated flags is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
        else if(counts[shortForm.value()] == 0 && counts[longForm.value()] == 0)
            throw UnknownOptionException("hallo");
        else return true;
    }
    else if(shortForm.has_value()) {
        if(counts[shortForm.value()] > 1)
            throw SameFlagsException(std::format("{}Error{}: using repeated flags is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
        else if(counts[shortForm.value()] == 0) return false;
        else return true;
    }
    else {
        if(counts[longForm.value()] > 1)
            throw SameFlagsException(std::format("{}Error{}: using repeated flags is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
        else if(counts[longForm.value()] == 0) return false;
        else return true;
    }
}