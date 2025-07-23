#include <iostream>
#include <vector>
#include "../include/matrix.h"
#include "../include/constraint.h"
#include "../include/lp.h"
#include "../include/model_reader.h"
#include <algorithm>

#include "../include/ip.h"

using namespace std;

int main() {
    ModelFileReader reader;
    LpProblem model = reader.readModel("model_t.lp");
    //model.displayProblem();

    IpProblem ipModel(model);
    ipModel.solveBranchAndBound();
    return 0;
}