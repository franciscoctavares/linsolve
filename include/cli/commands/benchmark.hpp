#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "cli/command.hpp"
#include <string>
#include <cstddef>

namespace Commands {
  
struct BenchmarkSettings {
    std::string modelFilePath;
    std::size_t iterations;
    bool multithreading;
};

class Benchmark : public Command {
    public:
        Benchmark(const std::string & commandName, const std::string& newHelpMessage) : Command(commandName, newHelpMessage) {}

        void runCommand() override;
};

}

#endif