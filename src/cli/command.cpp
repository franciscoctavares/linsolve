#include "cli/command.hpp"
#include "errors.hpp"
#include <iostream>
#include <format>

void Command::parseOptions(std::vector<std::string>& args) {
    bool isAnythingUsed = false;
    bool isThisOptionValid = false;

    for(std::size_t i = 2; i < args.size(); i++) {
        isThisOptionValid = false;
        for(std::size_t j = 0; j < options.size(); j++) {
            if(args[i] == options[j].getFlags().first || args[i] == options[j].getFlags().second) {
                //std::cout << "-h/--help is used\n";
                isAnythingUsed = true;
                isThisOptionValid = true;
                //std::cout << "Its the same as -h or --help" << std::endl;
                if(options[j].getIsUsed())
                    throw SameFlagsException();

                //flagDetected = true;
                options[j].setIsUsed(true);
                //std::cout << "Got here1\n";

                if(options[j].getIsExclusive() && i < args.size() - 1)
                    throw ExclusiveOptionException(std::format("{}/{}", options[j].getFlags().first.value(), options[j].getFlags().second.value()));

                if(options[j].getArgForce()) {
                    if(options[j].getArgForceType()) {
                        if(i == args.size() - 1) throw NoArgsException("An argument is required, yet none were provided.");

                        options[j].setArg(args[i + 1]);
                        //std::cout << "stored argument\n";
                        i++;
                    }
                    else {
                        if(i < args.size() - 1) throw NoArgsException("There aren't supposed to be any arguments, but some were provided");
                        //else if(!isFlag(args[i + 1])) throw NoArgsException("There aren't supposed to be any arguments, but some were provided");
                    }
                }
            }
        }
        //if(!isThisOptionValid) throw NoArgsException(std::format("The option {} is not valid", args[i]));
        if(!isThisOptionValid) throw InvalidOptionException(args[i]);
    }

    //if(!isAnythingUsed) throw NoArgsException("No options nor arguments were passed to this command.\n");
    if(!isAnythingUsed) throw NoArgsException(std::format("{}Error{}: No options nor arguments were passed to this command.\n\n{}Sugestion{}:Run \'linsolve --help\' to get more information.", RED, RESET, MAGENTA, RESET));
}

void Command::displayHelpMessage() {
    std::cout << std::format("Usage: linsolve {} [OPTIONS] [ARGS]...\n\n", name);

    std::cout << "A command line tool for solving Linear and Integer Programming problems.\n\n";

    /*
    std::cout << "Commands:\n";
    std::cout << "  solve\t\tSolve a Linear or Integer Programming problem\n";
    std::cout << "  benchmark\tBenchmark the speed and efficiency of different settings\n";
    */

    std::cout << "\nOptions:\n";
    for(Option& currentOption : options) {
        std::cout << "  ";
        if(currentOption.getFlags().first.has_value())
            std::cout << currentOption.getFlags().first.value();
        if(currentOption.getFlags().second.has_value()) {
            if(currentOption.getFlags().first.has_value()) std::cout << ", ";
            std::cout << currentOption.getFlags().second.value();
        }

        std::cout << "\t" << currentOption.getDescription() << "\n";

        //std::cout << "\n";
    }

    //std::cout << std::format("\nRun 'linsolve {} --help' for more information on a specific command.\n", name);
}