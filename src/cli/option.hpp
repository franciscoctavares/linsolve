#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <utility>
#include "errors.hpp"

class Option {
    public:
        constexpr Option(const std::pair<std::optional<std::string_view>, std::optional<std::string_view>> newFlags, bool argReq, bool argForce, bool exclusive, const std::string_view optionDescription)
            : flags(newFlags), description(optionDescription), argForce(argReq), argForceType(argForce), isExclusive(exclusive), isUsed(false), arg(std::nullopt) {

            if(!flags.first.has_value() && !flags.second.has_value())
                throw NoFlagsException("A flag must have a short form or a long form. You provided none");
            
            // short form flag
            if(flags.first.has_value()) {
                if(flags.first.value().length() != 2)
                    throw FlagFormatException("Short form flags must have 2 characters only");

                if(!flags.first.value().starts_with("-"))
                    throw FlagFormatException("Short form flags must start with \'-\'");

                if(flags.first.value()[1] < 'a' || flags.first.value()[1] > 'z')
                    throw FlagFormatException("Short form flags must have a lowercase letter as their second character");
            }

            if(flags.second.has_value()) {
                if(flags.second.value().length() <= 4)
                    throw FlagFormatException("Long form flags must start with \'--\' and have at least 2 letters after that");

                if(!flags.second.value().starts_with("--"))
                    throw FlagFormatException("Long form flags must start with \'--\'");

                for(std::size_t i = 2; i < flags.second.value().length(); i++) {
                    if(flags.second.value()[i] < 'a' || flags.second.value()[i] > 'z')
                        throw FlagFormatException("Long form flags must have all lowercase letters after the starting \'--\'");
                }
            }
        }

        const std::pair<std::optional<std::string_view>, std::optional<std::string_view>>& getFlags() const { return flags; }
        const std::string_view& getDescription() const { return description; }
        bool getArgForce() const { return argForce; }
        bool getArgForceType() const { return argForceType; }
        bool getIsExclusive() const { return isExclusive; }
        bool getIsUsed() const { return isUsed; }
        std::optional<std::string> getArg() const { return arg; }

        void setIsUsed(const bool used) { isUsed = used; }
        void setArg(std::optional<std::string> newArg = std::nullopt) { arg = newArg; }

    private:
        const std::pair<std::optional<std::string_view>, std::optional<std::string_view>> flags; // example - {"-h", "--help"}
        const std::string_view description;

        const bool argForce;
        const bool argForceType;
        const bool isExclusive;

        bool isUsed;
        std::optional<std::string> arg;
};