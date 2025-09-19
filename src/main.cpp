#include "cli/cli.h"

#include <string>
#include <vector>
#include <iostream>

int main(int argc, char** argv) {
    CLI interface(argc, argv);
    interface.run();
    return 0;
}