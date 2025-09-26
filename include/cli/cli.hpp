#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>
#include <variant>

#include "bb_utils.hpp"
#include "bb_tree.hpp"
#include "bb_node.hpp"

#include "cli/option.hpp"
#include "cli/command.hpp"
#include "cli/commands/benchmark.hpp"
#include "cli/commands/default_command.hpp"

using CommandTypes = std::variant<Commands::DefaultCommand>;


class CLI {
    public:
        CLI(int argc, char** argv);
        void run();
    private:
        std::vector<std::string> args;
        std::vector<CommandTypes> commands;

        int parseCommand(); 
};

#endif