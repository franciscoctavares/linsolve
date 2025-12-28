#include "cli/cli.hpp"
#include <iostream>

int main(int argc, char** argv) {
    CLI interface(argc, argv);
    interface.run();
    return 0;
}