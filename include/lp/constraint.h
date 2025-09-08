#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <vector>
#include <string>

enum ConstraintType {
    LESS_THAN_OR_EQUAL,      // <=
    EQUAL,                   // =
    GREATER_THAN_OR_EQUAL    // >=
};

class Constraint {
    public:
        Constraint(void) = default;

        Constraint(std::vector<double>& newLhs, const std::string& newConstraintType, double newRhs);

        std::vector<double>& getLhs() { return lhs; }

        ConstraintType getType() { return type; }

        double getRhs() { return rhs; }

        bool operator==(const Constraint& otherConstraint);

        /**
         * @brief Removes a variable(whose index is `varIndex`) that has been set to a specific value(`varValue`)
         * 
         * @throw std::invalid_argument - if `varIndex` < 0 or `varIndex` >= `lhs.size()`
         */
        void removeFixedVariable(int varIndex, double varValue);

    private:
        std::vector<double> lhs;
        ConstraintType type;
        double rhs;
};

#endif