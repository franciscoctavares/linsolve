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
    private:
        std::vector<double> lhs;
        ConstraintType type;
        double rhs;
    public:
        Constraint(void) = default;

        /**
         * @brief Constraint class constructor
         */
        Constraint(std::vector<double>& newLhs, std::string newConstraintType, double newRhs);

        /////////////// GETTERS ///////////////

        /**
         * @brief Returns a reference to the LHS(left hand side) of the contraint
         */
        std::vector<double>& getLhs() { return lhs; }

        /**
         * @brief Returns the type(<=, >= or =) of the constraint
         */
        ConstraintType getType() { return type; }

        /**
         * @brief Returns the RHS(right hand side) of the constraint
         */
        double getRhs() { return rhs; }

        ///////////////////////////////////////

        /**
         * @brief Checks if the LHS coefficients, the constraint type and the RHS of two constraints are equal
         * 
         * @return true if the constraints are the same, false otherwise
         */
        bool operator==(const Constraint& otherConstraint);

        /**
         * @brief Removes a variable(whose index is `varIndex`) that has been set to a specific value(`varValue`)
         */
        void removeFixedVariable(int varIndex, double varValue);
};

#endif