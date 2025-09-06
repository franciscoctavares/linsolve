#include <iostream>

#include "matrix.h"
#include "lp/constraint.h"
#include "lp/lp.h"
#include "model_reader.h"
#include "bb_node.h"
#include "bb_tree.h"
#include "bb_utils.h"
#include "cli/cli.h"
#include "tabulate.hpp"

#include <fstream>
#include <iostream>

#include "lp/lp_simplifier.h"

using namespace std;

int main(int argc, char** argv) {
    CLI interface(argc, argv);
    return 0;
}