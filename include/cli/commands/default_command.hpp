#ifndef DEFAULT_COMMAND_H
#define DEFAULT_COMMAND_H

#include "cli/command.hpp"

namespace Commands {

struct DefaultSettings {
    bool help;
};

// when input is something like "linsolve --help"
class DefaultCommand : public Command {
    public:
        DefaultCommand(const std::string& newHelpMessage);
        void parseOptions(std::vector<std::string>& args) override;
        void runCommand() override;
        void displayHelpMessage() override;
    private:
        DefaultSettings settings;
};

}

#endif