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

        /**
         * @brief Constraint class constructor
         * 
         */
        Constraint(std::vector<double> newLhs, std::string newConstraintType, double newRhs);

        /**
         * @brief Retrieves the LHS(left hand side) of the contraint
         * 
         */
        std::vector<double> getLhs();

        /**
         * @brief Retrieves the type(<=, >= or =) of the constraint
         * 
         */
        ConstraintType getType();

        /**
         * @brief Retrieves the RHS(right hand side) of the constraint
         * 
         */
        double getRhs();

        /**
         * @brief Sets a new value for the RHS(right hand side) of the constraint
         * 
         */
        void setRhs(double newRhs);

        /**
         * @brief Checks if the LHS coefficients, the constraint type and the RHS of two constraints are equal
         * 
         * @return true if the constraints are the same, false otherwise
         */
        bool operator==(Constraint otherConstraint);

        /**
         * @brief Removes a variable that has been fixed to a specific value(passed as an argument)
         * 
         * @param varIndex 
         * @param varValue 
         */
        void removeFixedVariable(uint varIndex, double varValue);
};

#endif