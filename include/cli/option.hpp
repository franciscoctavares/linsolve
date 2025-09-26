#pragma once

#include <string>
#include <optional>
#include <utility>

template<typename T>
class Option {
    public:
        Option(const std::pair<std::optional<std::string>, std::optional<std::string>> newFlags, bool argReq, bool argForce, bool exclusive);

        const std::pair<std::optional<std::string>, std::optional<std::string>>& getFlags() const { return flags; }
        bool getArgForce() const { return argForce; }
        bool getArgForceType() const { return argForceType; }
        bool getIsExclusive() const { return isExclusive; }
        bool getIsUsed() const { return isUsed; }
        std::optional<T>& getArg() const { return arg; }

        void setIsUsed(const bool used) { isUsed = used; }
        void setArg(std::optional<T> newArg = std::nullopt) { arg = newArg; }

    private:
        const std::pair<std::optional<std::string>, std::optional<std::string>> flags; // example - .first -> `-h`, .second -> `--help`

        const bool argForce;
        const bool argForceType;
        const bool isExclusive;

        bool isUsed;
        std::optional<T> arg;


};