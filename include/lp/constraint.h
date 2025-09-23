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

        const std::vector<double>& getLhs() const { return lhs; }

        ConstraintType getType() const { return type; }

        double getRhs() const { return rhs; }

        bool operator==(const Constraint& otherConstraint) const { return lhs == otherConstraint.lhs && type == otherConstraint.type && rhs == otherConstraint.rhs; }

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