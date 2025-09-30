#pragma once

#include <string>
#include <vector>
#include <memory>
#include "cli/command.hpp"

class CLI {
    public:
        CLI(int argc, char** argv);
        void run();
    private:
        std::vector<std::string> args;
        std::vector<std::unique_ptr<Command>> commands;

        int parseCommand();
};