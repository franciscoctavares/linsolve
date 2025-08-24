#include "../../include/cli/benchmark.h"
#include "../../include/tabulate.hpp"
#include "../../include/lp.h"
#include "../../include/model_reader.h"
#include "../../include/bb_tree.h"

// PRIVATE METHODS

void Benchmark::displayBenchmarkResults(uint metricsIndex) {
    tabulate::Table results;

    results.add_row({"Exploration strategy", "Branching strategy", "Average execution time"});

    //results.add_row({"Iterations", std::to_string(iterations)});
    //results.add_row({"Exploration strategy", convertExplorStratToString(metrics.strats[metricsIndex].first)});
    //results.add_row({"Branching strategy", convertBranchStratToString(metrics.strats[metricsIndex].second)});
    //results.add_row({"Explored nodes", std::to_string(metrics.explored_nodes[metricsIndex])});

    for(int i = 0; i < metrics.avg_execution_times.size(); i++) {
        std::string time_str;
        if(metrics.avg_execution_times[i] > 1000) time_str = std::to_string(metrics.avg_execution_times[i] / 1000) + " s";
        else if(metrics.avg_execution_times[i] < 1) time_str = std::to_string(metrics.avg_execution_times[i] * 1000) + " us";
        else time_str = std::to_string(metrics.avg_execution_times[i]) + " ms";
        results.add_row({convertExplorStratToString(metrics.strats[i].first), convertBranchStratToString(metrics.strats[i].second), time_str});
    }

    //std::string time_str;
    //if(metrics.avg_execution_times[metricsIndex] > 1000) time_str = std::to_string(metrics.avg_execution_times[metricsIndex] / 1000) + " s";
    //else if(metrics.avg_execution_times[metricsIndex] < 1) time_str = std::to_string(metrics.avg_execution_times[metricsIndex] * 1000) + " us";
    //else time_str = std::to_string(metrics.avg_execution_times[metricsIndex]) + " ms";

    //results.add_row({"Average execution time", time_str});

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
                avg_explored_nodes += (double)(tree.getMetrics().explored_nodes / iterations);

                tree.deleteTree();

                if(currentBranchStrat != BranchingStrategy::RANDOM_VAR && currentExplorStrat != ExplorationStrategy::RANDOM_NODE) {
                    deterministic_nodes = tree.getMetrics().explored_nodes;
                }
            }

            metrics.strats.push_back(std::make_pair(currentExplorStrat, currentBranchStrat));
            metrics.avg_execution_times.push_back(avg_execution_time);
            if(currentBranchStrat != BranchingStrategy::RANDOM_VAR && currentExplorStrat != ExplorationStrategy::RANDOM_NODE) {
                metrics.explored_nodes.push_back(deterministic_nodes);
            }
            else metrics.explored_nodes.push_back(avg_explored_nodes);

            //if(currentExplorStrat != ExplorationStrategy::EXPLORE_ALL_NODES) displayBenchmarkResults(currentIndex);
            currentIndex++;
        }
    }

    displayBenchmarkResults(0);
}