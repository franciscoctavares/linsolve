#pragma once

#include "cli/command.hpp"

namespace Commands {

struct DefaultSettings {
    bool help;
};

// when input is something like "linsolve --help"
class DefaultCommand : public Command {
    public:
        constexpr DefaultCommand(const std::string& newHelpMessage) : Command("", newHelpMessage) {
            Option helpOption({"-h", "--help"}, true, false, true, "Displays information about a command");
            options.emplace_back(helpOption);
            settings.help = false;
        }

        void parseOptions(std::vector<std::string>& args) override;
        void runCommand() override;
        void displayHelpMessage() override;
    private:
        DefaultSettings settings;
};

}