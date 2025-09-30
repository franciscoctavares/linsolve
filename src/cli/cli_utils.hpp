#pragma once

#include <string>
#include <cstddef>
#include <format>
#include <stdexcept>


#define foreground_color_rgb(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"

#define RESET   "\033[0m"

#define RED     foreground_color_rgb(255, 0, 0)
#define GREEN   foreground_color_rgb(0, 255, 0)
#define YELLOW  foreground_color_rgb(255, 255, 0)
#define BLUE    foreground_color_rgb(0, 0, 255)
#define MAGENTA foreground_color_rgb(255, 0, 255)
#define CYAN    foreground_color_rgb(0, 255, 255)
#define WHITE   foreground_color_rgb(255, 255, 255)
#define ORANGE  foreground_color_rgb(255, 165, 0)
#define TEAL    foreground_color_rgb(0, 128, 128)
#define PURPLE  foreground_color_rgb(128, 0, 128)
#define PINK    foreground_color_rgb(255, 192, 203)
#define GOLD    foreground_color_rgb(255, 215, 0)
#define SILVER  foreground_color_rgb(192, 192, 192)
#define NAVY    foreground_color_rgb(0, 0, 128)
#define OLIVE   foreground_color_rgb(128, 128, 0)

#define MAGENTA_ANSI "\033[35m"

class ArgumentCastingException : public std::logic_error {
    public:
        explicit ArgumentCastingException(const std::string& arg) : std::logic_error(
            std::format("{}An error ocurred while converting the argument {} to the correct type.", std::format("{}Error{}: ", RED, RESET), arg)
        ) {}
};

bool isFlag(const std::string& str);

template<typename T>
T convertStringToArg(const std::string& arg) {
    if constexpr(std::is_same<T, int>::value) {
        try {
            return std::stoi(arg);
        }
        catch(const std::exception& e) {
            throw ArgumentCastingException(arg);
        }
    }
}