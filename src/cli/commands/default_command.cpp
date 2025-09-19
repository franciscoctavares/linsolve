#include "cli/commands/default_command.h"
#include <iostream>
#include <utility>

#include "errors.h"
#include <format>
#include "cli/cli_utils.h"

namespace Commands {

DefaultCommand::DefaultCommand(const std::string& newHelpMessage) : Command("", newHelpMessage) {
    Option<MyVariant> helpOption({"-h", "--help"}, true, false, true);
    options.emplace_back(helpOption);
    settings.help = true;
}

void DefaultCommand::parseOptions(std::vector<std::string>& args) {
    bool isHelpUsed = options[0].flag.parseFlag(args);
    settings.help = isHelpUsed;
}

void DefaultCommand::runCommand() {
    if(settings.help) {
        displayHelpMessage();
    }
    //else throw NoArgsException(std::format("{}Error{}: using repeated flags is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
}

void DefaultCommand::displayHelpMessage() {

    std::cout << "Usage: linsolve [OPTIONS] COMMAND [ARGS]...\n\n";

    std::cout << "A CLI tool for solving Linear and Integer Programming problems.\n\n";

    std::cout << "Options:\n";
    
    for(auto& currentOption : options) {
        std::cout << "  ";
        if(currentOption.flag.getShortForm().has_value())
            std::cout << currentOption.flag.getShortForm().value();
        if(currentOption.flag.getLongForm().has_value())
            std::cout << ", " <<  currentOption.flag.getLongForm().value();

        

        //std::cout << "\tTest description\n";
    }

    std::cout << "\nCommands:\n";
    std::cout << R"(  solve         Solve a Linear or Integer Programming problem
  benchmark     Benchmark the speed and efficiency of different settings

Run 'linsolve COMMAND --help' for more information on a command.)" << std::endl;

}

}