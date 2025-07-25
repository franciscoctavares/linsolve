#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>

#include "../include/matrix.h"
#include "../include/constraint.h"
#include "../include/lp.h"
#include "../include/model_reader.h"
#include "../include/bb_node.h"
#include "../include/bb_tree.h"
#include "../include/ip.h"

using namespace std;

pair<bool, double> isEffectivelyInteger(double x, double epsilon = 1e-12) {
    if(std::abs(x - std::round(x)) < epsilon) {
        if(x - std::round(x) < 0) return make_pair(true, ceil(x));
        else return make_pair(true, floor(x));
    }
    else return make_pair(false, x);
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    ModelFileReader reader;
    LpProblem model = reader.readModel("model.lp");
    

    BaBNode newHeadNode(model);
    BaBTree babTree;
    babTree.setHeadNode(&newHeadNode);

    Matrix optimalWholeSolution = babTree.solveTree();
    cout << "The solution to the IP model is: ";
    optimalWholeSolution.displayMatrix();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Execution time: " << elapsed.count() << " ms\n";
    return 0;
}