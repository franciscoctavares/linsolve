#pragma once

#include <string>
#include <optional>
#include <utility>

class Option {
    public:
        Option(const std::pair<std::optional<std::string>, std::optional<std::string>> newFlags, bool argReq, bool argForce, bool exclusive, const std::string& optionDescription);

        const std::pair<std::optional<std::string>, std::optional<std::string>>& getFlags() const { return flags; }
        const std::string& getDescription() const { return description; }
        bool getArgForce() const { return argForce; }
        bool getArgForceType() const { return argForceType; }
        bool getIsExclusive() const { return isExclusive; }
        bool getIsUsed() const { return isUsed; }
        std::optional<std::string> getArg() const { return arg; }

        void setIsUsed(const bool used) { isUsed = used; }
        void setArg(std::optional<std::string> newArg = std::nullopt) { arg = newArg; }

    private:
        const std::pair<std::optional<std::string>, std::optional<std::string>> flags; // example - {"-h", "--help"}
        const std::string description;

        const bool argForce;
        const bool argForceType;
        const bool isExclusive;

        bool isUsed;
        std::optional<std::string> arg;
};