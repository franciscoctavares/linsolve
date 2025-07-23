#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <vector>
#include <string>
#include <sstream>

enum ConstraintType {
    LESS_THAN_OR_EQUAL,      // <=
    EQUAL,                   // =
    GREATER_THAN_OR_EQUAL    // >=
};

class Constraint {
    private:
        std::vector<double> lhs;
        ConstraintType type;
        double rhs;
    public:
        Constraint(void) = default;
        Constraint(std::vector<double>, std::string, double);
        std::vector<double> getLhs();
        ConstraintType getType();
        double getRhs();
        double setRhs(double newRhs);

        /**
         * @brief Checks if the LHS coefficients, the constraint type and the RHS of two constraints are equal
         * 
         * @return true if the constraints are the same, false otherwise
         */
        bool operator==(Constraint otherConstraint);

        void removeFixedVariable(uint varIndex, double varValue);
};

#endif