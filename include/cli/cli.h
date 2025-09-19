#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>
#include <variant>

#include "bb_utils.h"
#include "bb_tree.h"
#include "bb_node.h"

#include "cli/option.h"
#include "cli/command.h"
#include "cli/commands/benchmark.h"
#include "cli/commands/default_command.h"

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