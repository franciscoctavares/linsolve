#ifndef CLI_UTILS_H
#define CLI_UTILS_H

#include <string>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

bool isFlag(const std::string& str);

/*
template<typename T>
T convertFromString(const std::string& str) {
    if constexpr (std::is_integral<T>::value) {
        return std::stoi(str);
    }
    else if constexpr (std::is_floating_point<T>::value) {
        return std::stod(str);
    }
    else if constexpr (std::is_same<T, bool>::value) {
        if(str == "true" || str == "True") return true;
        else if(str == "false" || str == "False") return false;
    }
    else if constexpr (std::is_same<T, std::string>::value) {
        return str;
    }
    else
        throw WrongArgTypeException("Argument is of the wrong type");
}
*/

#endif