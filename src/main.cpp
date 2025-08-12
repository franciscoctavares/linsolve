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

using namespace std;

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    ModelFileReader reader;
    LpProblem model = reader.readModel("model_test.lp");

    BaBNode newHeadNode(model);
    BaBTree babTree(&newHeadNode);

    Matrix optimalWholeSolution = babTree.solveTree(EXPLORE_ALL_NODES);
    cout << "The solution to the IP model is(Z = " << optimalWholeSolution.dotProduct(model.getObjectiveFunction()) << "): ";
    optimalWholeSolution.displayMatrix();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Execution time: " << elapsed.count() << " ms\n";

    babTree.deleteTree();
    return 0;
}