#include "constraint.h"
#include <sstream>

Constraint::Constraint(std::vector<double>& newLhs, std::string newConstraintType, double newRhs) {
    lhs = newLhs;
    if(newConstraintType == "<=") type = LESS_THAN_OR_EQUAL;
    else if(newConstraintType == "=") type = EQUAL;
    else if(newConstraintType == ">=") type = GREATER_THAN_OR_EQUAL;
    else throw std::invalid_argument("Invalid type of constraint. A constraint can only be of the 3 following types: <=, >= or =");
    rhs = newRhs;
}

bool Constraint::operator==(const Constraint& otherConstraint) {
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

void Constraint::removeFixedVariable(int varIndex, double varValue) {
    if(varIndex < 0 || varIndex >= lhs.size()) {
        std::ostringstream errorMsg;
        errorMsg << "Error using removeFixedVariable: varIndex must be between 0 and " << lhs.size() - 1 << ", but was provided the value " << varIndex;
        throw std::invalid_argument(errorMsg.str());
    }
    //if(varIndex > lhs.size() - 1) std::cout << "varIndex out of bounds!" << std::endl;
    double coeff = lhs[varIndex];
    rhs -= coeff * varValue;
    lhs.erase(lhs.begin() + varIndex);
}