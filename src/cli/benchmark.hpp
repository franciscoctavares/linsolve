#pragma once

#include <vector>
#include <cstddef>

#include "ip/bb_utils.hpp"

#include <string>
#include <tuple>

struct BenchmarkPerformanceMetrics {
    std::vector<double> avg_execution_times;
    std::vector<std::pair<ExplorationStrategy, BranchingStrategy>> strats;
    std::vector<uint> explored_nodes;

    std::vector<std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>> things;
};


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