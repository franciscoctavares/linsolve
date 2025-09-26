#include "cli/cli_utils.hpp"

bool isFlag(const std::string& str) {
    if(str.starts_with("-")) return true;
    else if(str.starts_with("--")) return true;
    else return false;

    /*
    if(str.length() == 2 && str[0] == '-' && str[1] != '-') return true;
    else if(str.length() >= 4 && str[0] == '-' && str[1] == '-' && str[2] != '-') return true;
    else return false;
    */
}