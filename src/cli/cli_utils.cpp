#include "cli/cli_utils.hpp"

bool isFlag(const std::string& str) {
    return str.starts_with("-") || str.starts_with("--");
}