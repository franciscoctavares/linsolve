#include "cli/commands/benchmark.hpp"
#include "single_include/tabulate/tabulate.hpp"
#include <algorithm>
#include "lp/lp.hpp"
#include "model_reader.hpp"
#include "ip/bb_tree.hpp"

namespace Commands {

Benchmark::Benchmark() : Command("benchmark", "Benchmark different exploration and branching strategies") {
    Option helpOption({"-h", "--help"}, true, false, true, "Display this message");
    options.emplace_back(helpOption);

    Option fileOption({"-f", "--file"}, true, true, false, "The file containing the model");
    options.emplace_back(fileOption);

    Option threadsOption({std::nullopt, "--threads"}, true, false, false, "Enables multithreading");
    options.emplace_back(threadsOption);

    //Option iterationsOption({std::nullopt, "--iter"}, true, true, false, "Number of iterations");
    //options.emplace_back(iterationsOption);

    possibleExplorStrats = explorMap.getAllStrats();
    possibleBranchStrats = branchMap.getAllStrats();
}

void Benchmark::prepareSettings() {
    if(options[0].getIsUsed())
        settings.help = true;

    settings.iterations = 10;

    if(options[1].getIsUsed())
        settings.file = options[1].getArg().value() + ".lp";

    if(options[2].getIsUsed())
        settings.multithreading = true;
}

void Benchmark::displayBenchmarkResults() {
    tabulate::Table results;

    // sort the metrics by ascending order of average execution time
    std::sort(metrics.things.begin(), metrics.things.end(), [](const std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>& a,
                                                               const std::tuple<double, std::pair<ExplorationStrategy, BranchingStrategy>, uint>& b) {
                                                                return std::get<0>(a) < std::get<0>(b);
                                                               });

    results.add_row({"Exploration strategy", "Branching strategy", "Explored nodes", "Average execution time"});

    for(std::size_t i = 0; i < metrics.things.size(); i++) {
        std::string time_str;
        if(std::get<0>(metrics.things[i]) > 1000) time_str = std::to_string(std::get<0>(metrics.things[i]) / 1000) + " s";
        else if(std::get<0>(metrics.things[i]) < 1) time_str = std::to_string(std::get<0>(metrics.things[i]) * 1000) + " \u03BCs";
        else time_str = std::to_string(std::get<0>(metrics.things[i])) + " ms";

        std::string currentExplorStratString = static_cast<std::string>(explorMap[std::get<1>(metrics.things[i]).first]);
        std::string currentBranchStratString = static_cast<std::string>(branchMap[std::get<1>(metrics.things[i]).second]);
        std::string currentExploredNodesString = std::to_string(std::get<2>(metrics.things[i]));
        results.add_row({currentExplorStratString, currentBranchStratString, currentExploredNodesString, time_str});
    }

    std::cout << results << std::endl;
}

void Benchmark::runBenchmark() {
    double avg_execution_time;
    double avg_explored_nodes;
    uint currentIndex = 0;
    uint deterministic_nodes = 0;

    for(std::size_t i = 0; i < possibleExplorStrats.size(); i++) {
        ExplorationStrategy currentExplorStrat = possibleExplorStrats[i];
        for(std::size_t j = 0; j < possibleBranchStrats.size(); j++) {
            BranchingStrategy currentBranchStrat = possibleBranchStrats[j];

            avg_execution_time = 0;
            avg_explored_nodes = 0;

            for(std::size_t k = 0; k < settings.iterations; k++) {
                LP::LpProblem initialProblem = ModelFileReader::readModel(settings.file);
                std::vector<VariableType> varTypes;
                for(uint i = 0; i < initialProblem.getObjectiveFunction().getNColumns(); i++) varTypes.push_back(VariableType::INTEGER);

                BaBTree tree(initialProblem, varTypes);
                bool multithreading = false;
                Matrix optimalWholeSolution = tree.solveTree(currentExplorStrat, currentBranchStrat, multithreading);

                avg_execution_time += tree.getMetrics().execution_time / settings.iterations;
                avg_explored_nodes += ((double)tree.getMetrics().explored_nodes / (double)settings.iterations);

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

    displayBenchmarkResults();
}

void Benchmark::runCommand() {
    prepareSettings();

    if(settings.help)
        displayHelpMessage();
    else
        runBenchmark();

}

}
