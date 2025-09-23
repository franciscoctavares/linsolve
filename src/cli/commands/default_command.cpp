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
    /*
    bool isHelpUsed = options[0].flag.parseFlag(args);
    settings.help = isHelpUsed;

    for(std::size_t i = 2; i < args.size(); i++) {
        if(!isFlag(args[i])) throw NotAFlagException("linsolve", args[i]);

        
    }
    */
    bool flagDetected;

    for(std::size_t i = 2; i < args.size(); i++) {
        flagDetected = false;
        for(std::size_t j = 0; j < options.size(); j++) {
            if(args[i] == options[j].getFlags().first || args[i] == options[j].getFlags().second) {
                if(options[j].getIsUsed())
                    throw SameFlagsException();

                flagDetected = true;
                options[j].setIsUsed(true);

                if(options[j].getIsExclusive() && i < args.size() - 1)
                    throw ExclusiveOptionException(std::format("{}/{}", options[j].getFlags().first, options[j].getFlags().second));

                if(options[j].getArgForce()) {
                    if(options[j].getArgForceType()) {
                        if(i == args.size() - 1) throw
                    }
                    else {

                    }
                }
            }
        }
    }
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

    std::cout << "Commands:\n";
    std::cout << "  solve\t\tSolve a Linear or Integer Programming problem\n";
    std::cout << "  benchmark\tBenchmark the speed and efficiency of different settings\n";

    std::cout << "\nOptions:\n";
    for(Option<MyVariant>& currentOption : options) {
        std::cout << "  ";
        if(currentOption.flag.getShortForm().has_value())
            std::cout << currentOption.flag.getShortForm().value();
        if(currentOption.flag.getLongForm().has_value())
            std::cout << ", " <<  currentOption.flag.getLongForm().value();

        std::cout << "\n";
    }

    std::cout << "\nRun 'linsolve COMMAND --help' for more information on a specific command.\n";
}

}