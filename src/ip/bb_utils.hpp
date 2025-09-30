#pragma once

#include "matrix.hpp"
#include <sys/types.h>
#include <vector>
#include <string>
#include <stdexcept>

enum class BranchingStrategy {
    FIRST_INDEX,
    RANDOM_VAR,
    BEST_COEFFICIENT,
};

enum class ExplorationStrategy {
    EXPLORE_ALL_NODES,
    BEST_VALUE,
    WIDTH,
    DEPTH,
    RANDOM_NODE,
};

struct PerformanceMetrics {
    uint explored_nodes;
    double execution_time;
    Matrix optimalWholeSolution;
    uint optimalSolutionDepth;
};

template<typename T>
class StratMap {
    public:
        StratMap(std::vector<std::pair<std::string, T>> newMap) { map = newMap; }

        void insert(std::pair<std::string, T> newPair) { map.insert(newPair); }

        T operator[](const std::string& key) const {
            for(const std::pair<std::string, T>& currentPair : map) {
                if(currentPair.first == key) return currentPair.second;
            }

            throw std::invalid_argument("Key not found");
        }

        std::string operator[](T value) const {
            for(const std::pair<std::string, T>& currentPair : map) {
                if(currentPair.second == value) return currentPair.first;
            }

            throw std::invalid_argument("Value not found");
        }

        std::vector<T> getAllStrats() {
            std::vector<T> resultMap;
            for(const std::pair<std::string, T>& currentPair : map) {
                resultMap.push_back(currentPair.second);
            }
            return resultMap;
        }
    private:
        std::vector<std::pair<std::string, T>> map;
};

inline StratMap<ExplorationStrategy> explorMap({{"EXPLORE_ALL_NODES", ExplorationStrategy::EXPLORE_ALL_NODES},
                                         {"BEST_VALUE", ExplorationStrategy::BEST_VALUE},
                                         {"WIDTH", ExplorationStrategy::WIDTH},
                                         {"DEPTH", ExplorationStrategy::DEPTH},
                                         {"RANDOM_NODE", ExplorationStrategy::RANDOM_NODE}});

inline StratMap<BranchingStrategy> branchMap({{"FIRST_INDEX", BranchingStrategy::FIRST_INDEX},
                                       {"RANDOM_VAR", BranchingStrategy::RANDOM_VAR},
                                       {"BEST_COEFFICIENT", BranchingStrategy::BEST_COEFFICIENT}});