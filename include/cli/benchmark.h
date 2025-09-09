#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <vector>
#include <cstddef>

#include "bb_utils.h"

#include <string>
#include <tuple>

typedef struct {
    std::vector<double> avg_execution_times;
    std::vector<std::pair<ExplorationStrategy, BranchingStrategy>> strats;
    std::vector<uint> explored_nodes;

    std::vector<std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>> things;
}BenchmarkPerformanceMetrics;


class Benchmark {
    private:
        std::size_t iterations;
        std::vector<ExplorationStrategy> possibleExplorStrats;
        std::vector<BranchingStrategy> possibleBranchStrats;
        BenchmarkPerformanceMetrics metrics;

        void displayBenchmarkResults();

    public:
        Benchmark(std::size_t numIterations = 10);

        void runBenchmark();
};

#endif