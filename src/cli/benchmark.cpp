#include "cli/benchmark.h"
#include "tabulate.hpp"
#include "lp.h"
#include "model_reader.h"
#include "bb_tree.h"

// PRIVATE METHODS

void Benchmark::displayBenchmarkResults(uint metricsIndex) {
    tabulate::Table results;

    // sort the metrics by ascending order of average execution time
    std::sort(metrics.things.begin(), metrics.things.end(), [](const std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>& a,
                                                               const std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>& b) {
                                                                return std::get<0>(a) < std::get<0>(b);
                                                               });

    results.add_row({"Exploration strategy", "Branching strategy", "Explored nodes", "Average execution time"});

    for(int i = 0; i < metrics.things.size(); i++) {
        std::string time_str;
        if(std::get<0>(metrics.things[i]) > 1000) time_str = std::to_string(std::get<0>(metrics.things[i]) / 1000) + " s";
        else if(std::get<0>(metrics.things[i]) < 1) time_str = std::to_string(std::get<0>(metrics.things[i]) * 1000) + " \u03BCs";
        else time_str = std::to_string(std::get<0>(metrics.things[i])) + " ms";

        std::string currentExplorStratString = convertExplorStratToString(std::get<1>(metrics.things[i]).first);
        std::string currentBranchStratString = convertBranchStratToString(std::get<1>(metrics.things[i]).second);
        std::string currentExploredNodesString = std::to_string(std::get<2>(metrics.things[i]));
        results.add_row({currentExplorStratString, currentBranchStratString, currentExploredNodesString, time_str});
    }

    std::cout << results << std::endl;
}

// PUBLIC METHODS

Benchmark::Benchmark(uint numIterations) {
    iterations = numIterations;
    possibleExplorStrats = getPossibleExplorationStrategies();
    possibleBranchStrats = getPossibleBranchingStrategies();
}

void Benchmark::runBenchmark() {
    double avg_execution_time;
    double avg_explored_nodes;
    uint currentIndex = 0;
    uint deterministic_nodes;

    for(int i = 0; i < possibleExplorStrats.size(); i++) {
        ExplorationStrategy currentExplorStrat = possibleExplorStrats[i];
        for(int j = 0; j < possibleBranchStrats.size(); j++) {
            BranchingStrategy currentBranchStrat = possibleBranchStrats[j];

            avg_execution_time = 0;
            avg_explored_nodes = 0;

            for(int k = 0; k < iterations; k++) {
                LpProblem initialProblem = ModelFileReader::readModel("bench.lp");
                BaBTree tree(initialProblem);
                Matrix optimalWholeSolution = tree.solveTree(currentExplorStrat, currentBranchStrat);

                avg_execution_time += tree.getMetrics().execution_time / iterations;
                avg_explored_nodes += ((double)tree.getMetrics().explored_nodes / (double)iterations);

                tree.deleteTree();

                if(currentBranchStrat != BranchingStrategy::RANDOM_VAR && currentExplorStrat != ExplorationStrategy::RANDOM_NODE) {
                    deterministic_nodes = tree.getMetrics().explored_nodes;
                }
            }

            metrics.strats.push_back(std::make_pair(currentExplorStrat, currentBranchStrat));
            metrics.avg_execution_times.push_back(avg_execution_time);

            
            if(currentBranchStrat != BranchingStrategy::RANDOM_VAR && currentExplorStrat != ExplorationStrategy::RANDOM_NODE) {
                metrics.explored_nodes.push_back(deterministic_nodes);
                metrics.things.push_back(std::make_tuple(avg_execution_time, std::make_pair(currentExplorStrat, currentBranchStrat), deterministic_nodes));
            }
            else {
                metrics.explored_nodes.push_back(avg_explored_nodes);
                metrics.things.push_back(std::make_tuple(avg_execution_time, std::make_pair(currentExplorStrat, currentBranchStrat), avg_explored_nodes));
            }

            currentIndex++;
        }
    }

    displayBenchmarkResults(0);
}