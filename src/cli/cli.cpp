#include "cli/cli.hpp"
#include "cli/cli_utils.hpp"

#include <filesystem>
#include <iostream>
#include <algorithm>
#include <format>
#include "errors.hpp"

// PRIVATE METHODS

int CLI::parseCommand() {
    if(args.size() == 1)
        throw NoArgsException(std::format("{}Error{}: No commands, no options and no arguments were specified.\n\n{}Sugestion{}:Run \'linsolve --help\' to get more information.", RED, RESET, MAGENTA, RESET));
    
    int typedCommandIndex = -1;
    for(std::size_t i = 0; i < commands.size(); i++) {
        if(args[1] == std::visit([](auto&& arg) {
            return arg.getName();
        }, commands[i])) {
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

        
        //if(typedCommandIndex == -2)
        //    throw UnknownCommandException(args[1]);
        if(typedCommandIndex == -1 && !isFlag(args[1])) throw UnknownCommandException(args[1]);
        else typedCommandIndex++; // commands[0] is default, "linsolve --help" for example


        std::visit([this](auto&& arg) {
            arg.parseOptions(this->args);
        }, commands[typedCommandIndex]);

        std::visit([](auto&& arg) {
            arg.runCommand();
        }, commands[typedCommandIndex]);
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

    const std::string helpMessage = R"(Usage: linsolve [OPTIONS] COMMAND [ARGS]...

A CLI tool for solving Linear and Integer Programming problems.

Options:
  -h, --help        Show this message and exit

Commands:
  solve         Solve a Linear or Integer Programming problem
  benchmark     Benchmark the speed and efficiency of different settings

Run 'linsolve COMMAND --help' for more information on a command.)";

    commands.push_back(Commands::DefaultCommand(helpMessage));
}