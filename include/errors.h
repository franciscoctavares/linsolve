#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
#include <string>
#include <format>

#include "cli/cli_utils.h"

const std::string error_thing = std::format("{}Error{}: ", RED, RESET);
const std::string sugestion = std::format("{}Sugestion{}: run \'linsolve --help\' to get more information.", MAGENTA, RESET);

//  std::logic_error
//      BaseCLIException
//          ParsingException
//              UnknownCommandException
//              NoArgsException
//              MultipleArgsException
//          OptionException
//              FlagException
//                  NoFlagsException
//                  FlagFormatException

class BaseCLIException : public std::logic_error {
    public:
        explicit BaseCLIException(const std::string& msg) : std::logic_error(msg) {}
};

class ParsingException : public BaseCLIException {
    public:
        explicit ParsingException(const std::string& msg) : BaseCLIException(msg) {}
    protected:
        //const std::string error_thing = std::format("{}Error{}: ", RED, RESET);
        //const std::string sugestion = std::format("{}Sugestion{}: run \'linsolve --help\' to get more information.", MAGENTA, RESET);
};

class UnknownCommandException : public ParsingException {
    public:
        explicit UnknownCommandException(const std::string& unknown_command) : ParsingException(
            std::format("{}Unknown command \'{}\'.\n\n{}", error_thing, unknown_command, sugestion)
        ) {}
};

class UnknownOptionException : public ParsingException {
    public:
        explicit UnknownOptionException(const std::string& unknown_option) : ParsingException(
            std::format("{}Unknown option \'{}\'.\n\n{}", error_thing, unknown_option, sugestion)
        ) {} 
};

class WrongArgTypeException : public ParsingException {
    public:
        explicit WrongArgTypeException(const std::string& msg) : ParsingException(msg) {}
};

class NoArgsException : public ParsingException {
    public:
        explicit NoArgsException(const std::string& msg) : ParsingException(msg) {}
};

class MultipleArgsException : public ParsingException {
    public:
        explicit MultipleArgsException(const std::string& msg) : ParsingException(msg) {}
};

class OptionException : public BaseCLIException {
    public:
        explicit OptionException(const std::string& msg) : BaseCLIException(msg) {}
};

class FlagException : public OptionException {
    public:
        explicit FlagException(const std::string& msg) : OptionException(msg) {}
};

class NoFlagsException : public FlagException {
    public:
        explicit NoFlagsException(const std::string& msg) : FlagException(msg) {}
};

class FlagFormatException : public FlagException {
    public:
        explicit FlagFormatException(const std::string& msg) : FlagException(msg) {}
};

class SameFlagsException : public FlagException {
    public:
        explicit SameFlagsException(const std::string& msg) : FlagException(msg) {}
};



#endif