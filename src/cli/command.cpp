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
                isAnythingUsed = true;
                isThisOptionValid = true;

                if(options[j].getIsUsed())
                    throw SameFlagsException();

                options[j].setIsUsed(true);

                if(options[j].getIsExclusive() && i < args.size() - 1)
                    throw ExclusiveOptionException(std::format("{}/{}", options[j].getFlags().first.value(), options[j].getFlags().second.value()));

                if(options[j].getArgForce()) {
                    if(options[j].getArgForceType()) {
                        if(i == args.size() - 1) throw NoArgsException("An argument is required, yet none were provided.");

                        options[j].setArg(args[i + 1]);
                        i++;
                    }
                    else {
                        if(i < args.size() -1 && !isFlag(args[i + 1])) throw NoArgsException("There aren't supposed to be any arguments, but some were provided");
                    }
                }
            }
        }
        if(!isThisOptionValid) throw InvalidOptionException(args[i]);
    }

    if(!isAnythingUsed) throw NoArgsException(std::format("{}Error{}: No options nor arguments were passed to this command.\n\n{}Sugestion{}:Run \'linsolve --help\' to get more information.", RED, RESET, MAGENTA, RESET));
}

void Command::displayHelpMessage() {
    std::cout << std::format("Usage: linsolve {} [OPTIONS] [ARGS]...\n\n", name);

    //std::cout << "A command line tool for solving Linear and Integer Programming problems.\n\n";
    std::cout << std::format("{}\n\n", description);

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
    }
}