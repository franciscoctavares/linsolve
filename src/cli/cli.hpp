#pragma once

#include <string>
#include <vector>
#include <variant>

#include "cli/option.hpp"
#include "cli/command.hpp"
#include "cli/commands/default_command.hpp"

#include <memory>


class CLI {
    public:
        CLI(int argc, char** argv);
        void run();
    private:
        std::vector<std::string> args;
        std::vector<std::unique_ptr<Command>> commands;

        int parseCommand(); 
};