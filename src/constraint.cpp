#include "../include/constraint.h"

#include <iostream>

Constraint::Constraint(std::vector<double> newLhs, std::string newConstraintType, double newRhs) {
    lhs = newLhs;
    if(newConstraintType == "<=") type = LESS_THAN_OR_EQUAL;
    else if(newConstraintType == "=") type = EQUAL;
    else if(newConstraintType == ">=") type = GREATER_THAN_OR_EQUAL;
    else throw std::runtime_error("Invalid type of constraint. A constraint can only be of the 3 following types: <=, >= or =");
    rhs = newRhs;
}

std::vector<double> Constraint::getLhs() {
    return lhs;
}

ConstraintType Constraint::getType() {
    return type;
}

double Constraint::getRhs() {
    return rhs;
}

double Constraint::setRhs(double newRhs) {
    rhs = newRhs;
}

bool Constraint::operator==(Constraint otherConstraint) {
    if(lhs.size() != otherConstraint.lhs.size()) return false;

    // Check LHS coefficients
    for(int i = 0; i < lhs.size(); i++) {
        if(lhs[i] != otherConstraint.lhs[i]) return false;
    }

    // Check constraint type
    if(type != otherConstraint.type) return false;

    // Check RHS value
    if(rhs != otherConstraint.rhs) return false;

    return true;
}

void Constraint::removeFixedVariable(uint varIndex, double varValue) {
    if(varIndex > lhs.size() - 1) std::cout << "varIndex out of bounds!" << std::endl;
    double coeff = lhs[varIndex];
    rhs -= coeff * varValue;
    lhs.erase(lhs.begin() + varIndex);
}