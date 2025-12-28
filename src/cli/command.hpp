#pragma once

#include "cli/option.hpp"

#include <string>
#include <vector>
#include <variant>
#include <cstddef>

#include <array>

#include "cli/cli_utils.hpp"

class Command {
    public:
        constexpr Command(const std::string_view commandName, const std::string_view commandDescription) : name(commandName), description(commandDescription) {}
        
        const std::string_view getName() const { return name; }
        const std::string_view getDescription() const { return description; }

        virtual void parseOptions(std::vector<std::string>& args);
        virtual void displayHelpMessage();
        virtual void runCommand() = 0;
    protected:
        std::vector<Option> options;
        const std::string_view name;
        const std::string_view description;
};