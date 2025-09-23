#ifndef COMMAND_H
#define COMMAND_H

#include "cli/option.h"

#include <string>
#include <vector>
#include <variant>
#include <cstddef>

// to represent types of possible arguments
using MyVariant = std::variant<bool, int, std::string>;

class Command {
    public:
        Command(const std::string& commandName, const std::string& newHelpMessage) : name(commandName), helpMessage(newHelpMessage) {}
        const std::string& getName() const { return name; }

        virtual void parseOptions(std::vector<std::string>& args) = 0;
        virtual void runCommand() = 0;
        virtual void displayHelpMessage() = 0;
    protected:
        std::vector<Option<MyVariant>> options;
        const std::string name;
        const std::string helpMessage;
};

#endif