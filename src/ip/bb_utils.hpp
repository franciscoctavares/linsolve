#pragma once

#include "matrix.hpp"
#include <sys/types.h>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <cstddef>
#include <array>

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

template<typename T, std::size_t N>
class StratMap {
    public:
        constexpr StratMap(const std::array<std::pair<std::string_view, T>, N> newMap) : map(newMap) {}

        constexpr T operator[](const std::string_view& key) const {
            for(const std::pair<std::string_view, T>& currentPair : map) {
                if(currentPair.first == key) return currentPair.second;
            }

            throw std::invalid_argument("Key not found");
        }

        constexpr std::string_view operator[](T value) const {
            for(const std::pair<std::string_view, T>& currentPair : map) {
                if(currentPair.second == value) return currentPair.first;
            }

            throw std::invalid_argument("Value not found");
        }

        std::array<T, N> getAllStrats() const {
            std::array<T, N> resultMap;
            for(std::size_t i = 0; i < map.size(); i++) {
                resultMap[i] = map[i].second;
            }
            return resultMap;
        }
    private:
        const std::array<std::pair<std::string_view, T>, N> map;
};

inline constexpr StratMap<ExplorationStrategy, 5> explorMap({{
                                        {"EXPLORE_ALL_NODES", ExplorationStrategy::EXPLORE_ALL_NODES},
                                        {"BEST_VALUE", ExplorationStrategy::BEST_VALUE},
                                        {"WIDTH", ExplorationStrategy::WIDTH},
                                        {"DEPTH", ExplorationStrategy::DEPTH},
                                        {"RANDOM_NODE", ExplorationStrategy::RANDOM_NODE}
                                        }});

inline constexpr StratMap<BranchingStrategy, 3> branchMap({{
                                        {"FIRST_INDEX", BranchingStrategy::FIRST_INDEX},
                                        {"RANDOM_VAR", BranchingStrategy::RANDOM_VAR},
                                        {"BEST_COEFFICIENT", BranchingStrategy::BEST_COEFFICIENT}
                                        }});