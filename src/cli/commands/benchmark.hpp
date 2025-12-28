#pragma once

#include "cli/command.hpp"
#include "ip/bb_utils.hpp"

#include <array>
#include <tuple>
#include <cstddef>
#include <string>

namespace Commands{

struct BenchmarkSettings {
    bool help;
    std::size_t iterations;
    std::string file;
    bool multithreading;
};

struct BenchmarkPerformanceMetrics {
    std::vector<double> avg_execution_times;
    std::vector<std::pair<ExplorationStrategy, BranchingStrategy>> strats;
    std::vector<uint> explored_nodes;

    std::vector<std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>> things;
};

class Benchmark : public Command {
    public:
        Benchmark();
        void runCommand() override;

        void prepareSettings();
    private:
        std::array<ExplorationStrategy, 5> possibleExplorStrats;
        std::array<BranchingStrategy, 3> possibleBranchStrats;
        BenchmarkPerformanceMetrics metrics;

        BenchmarkSettings settings;
        void displayBenchmarkResults();
        void runBenchmark();
};

}