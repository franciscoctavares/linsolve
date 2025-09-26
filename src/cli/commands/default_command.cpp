#include "cli/commands/default_command.hpp"
#include <iostream>
#include <utility>

#include "errors.hpp"
#include <format>
#include "cli/cli_utils.hpp"
#include "cli/option.hpp"

namespace Commands {

DefaultCommand::DefaultCommand(const std::string& newHelpMessage) : Command("", newHelpMessage) {
    Option<MyVariant> helpOption(std::make_pair("-h", "--help"), true, false, true);
    options.emplace_back(helpOption);
    settings.help = true;
}

void DefaultCommand::parseOptions(std::vector<std::string>& args) {
    /*
    bool isHelpUsed = options[0].flag.parseFlag(args);
    settings.help = isHelpUsed;

    for(std::size_t i = 2; i < args.size(); i++) {
        if(!isFlag(args[i])) throw NotAFlagException("linsolve", args[i]);

        
    }
    */
    //bool flagDetected;

    for(std::size_t i = 2; i < args.size(); i++) {
        //flagDetected = false;
        for(std::size_t j = 0; j < options.size(); j++) {
            if(args[i] == options[j].getFlags().first || args[i] == options[j].getFlags().second) {
                if(options[j].getIsUsed())
                    throw SameFlagsException();

                //flagDetected = true;
                options[j].setIsUsed(true);

                if(options[j].getIsExclusive() && i < args.size() - 1)
                    throw ExclusiveOptionException(std::format("{}/{}", options[j].getFlags().first.value(), options[j].getFlags().second.value()));

                if(options[j].getArgForce()) {
                    if(options[j].getArgForceType()) {
                        if(i == args.size() - 1) throw NoArgsException("An argument is required, none were provided.");

                        options[j].setArg();
                    }
                    else {
                        std::cout << "No argument is supposed to be here\n";
                    }
                }
            }
        }
    }
}

void DefaultCommand::runCommand() {
    if(options[0].getIsUsed()) {
        displayHelpMessage();
    }
    //else throw NoArgsException(std::format("{}Error{}: using repeated flags is not allowed.\n\n{}Sugestion{}: Run \'linsolve --help\' to get more information.", RED, RESET, CYAN, RESET));
}

void DefaultCommand::displayHelpMessage() {

    std::cout << "Usage: linsolve [OPTIONS] COMMAND [ARGS]...\n\n";

    std::cout << "A CLI tool for solving Linear and Integer Programming problems.\n\n";

    std::cout << "Commands:\n";
    std::cout << "  solve\t\tSolve a Linear or Integer Programming problem\n";
    std::cout << "  benchmark\tBenchmark the speed and efficiency of different settings\n";

    std::cout << "\nOptions:\n";
    for(Option<MyVariant>& currentOption : options) {
        std::cout << "  ";
        if(currentOption.getFlags().first.has_value())
            std::cout << currentOption.getFlags().first.value();
        if(currentOption.getFlags().second.has_value())
            std::cout << ", " <<  currentOption.getFlags().second.value();

        std::cout << "\n";
    }

    std::cout << "\nRun 'linsolve COMMAND --help' for more information on a specific command.\n";
}

}