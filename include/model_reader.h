#ifndef FILE_H
#define FILE_H

#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include "lp.h"


class ModelFileReader {
    private:

        /**
         * @brief Reads the problem's type and the objective function coefficients from the file whose name is fileName
         */
        static std::pair<std::string, std::vector<double>> readObjectiveFunction(std::string fileName);

        /**
         * @brief Reads the problem's constraints from the file whose name is fileName
         */
        static std::vector<std::tuple<std::vector<double>, std::string, double>> readConstraints(std::string fileName, unsigned varsNumber);
    public:

        /**
         * @brief Constructor for the ModelFileReader class
         * 
         */
        ModelFileReader();
        
        /**
         * @brief Given the path of the model file, reads a linear programming problem and returns an LpProblem object with the model
         * 
         * @param fileName the path to the model file
         * @return LpProblem - the model read, encapsulated in the LpProblem class
         */
        static LpProblem readModel(std::string fileName);
};

#endif