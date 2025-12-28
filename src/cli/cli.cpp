#include "cli/cli.hpp"
#include "cli/cli_utils.hpp"

#include <filesystem>
#include <iostream>
#include <algorithm>
#include <format>
#include "errors.hpp"

#include "cli/commands/default_command.hpp"
#include "cli/commands/solve.hpp"
#include "cli/commands/benchmark.hpp"

// PRIVATE METHODS

int CLI::parseCommand() {
    if(args.size() == 1)
        throw NoArgsException(std::format("{}Error{}: No commands, no options and no arguments were specified.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, MAGENTA_ANSI, RESET));
    
    int typedCommandIndex = -1;
    for(std::size_t i = 0; i < commands.size(); i++) {
        if(args[1] == commands[i]->getName()) {
            if(typedCommandIndex != -1)
                throw MultipleArgsException(std::format("{}Error{}: Multiple commands typed.\n\nRun \'linsolve --help\' to get more information.", RED, RESET));
            else
                typedCommandIndex = i;
        }
    }

    return typedCommandIndex;
}

void CLI::run() {
    try {
        int typedCommandIndex = parseCommand();

        if(typedCommandIndex == -1 && !isFlag(args[1])) throw UnknownCommandException(args[1]);
        else if(typedCommandIndex == -1 && isFlag(args[1])) typedCommandIndex++; // commands[0] is default, "linsolve --help" for example
        //else typedCommandIndex++; // commands[0] is default, "linsolve --help" for example

        //std::cout << "Typed command is " << commands[typedCommandIndex]->getName() << std::endl;
        commands[typedCommandIndex]->parseOptions(args);
        commands[typedCommandIndex]->runCommand();
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

// PUBLIC METHODS

CLI::CLI(int argc, char** argv) {
    for(int i = 0; i < argc; i++) {
        args.emplace_back(std::string(argv[i]));
    }

    commands.push_back(std::make_unique<Commands::DefaultCommand>("A command line tool to solve Linear and Integer programming problems"));
    commands.push_back(std::make_unique<Commands::Solve>("Solve a Linear or Integer programming problem"));
    commands.push_back(std::make_unique<Commands::Benchmark>());
}