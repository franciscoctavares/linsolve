#pragma once

#include "cli/option.hpp"

#include <string>
#include <vector>
#include <variant>
#include <cstddef>

#include "cli/cli_utils.hpp"

class Command {
    public:
        constexpr Command(const std::string& commandName, const std::string& commandDescription) : name(commandName), description(commandDescription) {}
        const std::string& getName() const { return name; }

        virtual void parseOptions(std::vector<std::string>& args);
        virtual void runCommand() = 0;
        virtual void displayHelpMessage();
    protected:
        std::vector<Option> options;
        const std::string name;
        const std::string description;
};