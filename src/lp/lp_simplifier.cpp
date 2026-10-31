#include "lp/lp_simplifier.hpp"

#include <algorithm>
#include <iostream>
#include <format>
#include <cstddef>

namespace LP {

// PRIVATE METHODS

void LPSimplifier::getBasisConstraintsInfo(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    std::vector<std::pair<uint, uint>> basisConstraintsInfo; // .first is the constraint index, .second is the index i in xi <=/=/>= k
    
    // store information of all constraints of the form xi <=/=/>= k in basisConstraintsInfo
    for(std::size_t i = 0; i < problemToSimplify.getConstraints().size(); i++) {
        int basisVarIndex = isBasisVector(problemToSimplify.getConstraints()[i].getLhs());
        if(basisVarIndex != -1) helper.basisConstraintsInfo.emplace_back(i, basisVarIndex);
    }
}

void LPSimplifier::searchForXIEqualsKConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    for(const std::pair<uint, uint>& basisConstraint : helper.basisConstraintsInfo) {
        const uint constraintIndex = basisConstraint.first;
        const uint basisVariable = basisConstraint.second; // index i of non zero element in basis vector

        if(problemToSimplify.getConstraints()[constraintIndex].getType() == EQUAL) {
            helper.constraintsToRemove.push_back(constraintIndex);
            helper.fixedVariables.emplace_back(basisVariable, problemToSimplify.getConstraints()[basisConstraint.first].getRhs());
        }
    }
}

void LPSimplifier::searchForXIInequalityConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    for(std::size_t i = 0; i < helper.basisConstraintsInfo.size(); i++) {
        const uint constraintIndex_I = helper.basisConstraintsInfo[i].first;
        const uint basisVariable_I = helper.basisConstraintsInfo[i].second;

        for(std::size_t j = i + 1; j < helper.basisConstraintsInfo.size(); j++) {
            const uint constraintIndex_J = helper.basisConstraintsInfo[j].first;
            const uint basisVariable_J = helper.basisConstraintsInfo[j].second;

            int currentBasisConstraint_j = helper.basisConstraintsInfo[j].first;
            if(problemToSimplify.getConstraints()[constraintIndex_I].getLhs() == problemToSimplify.getConstraints()[constraintIndex_J].getLhs() &&
               problemToSimplify.getConstraints()[constraintIndex_I].getType() != problemToSimplify.getConstraints()[constraintIndex_J].getType() &&
               problemToSimplify.getConstraints()[constraintIndex_I].getRhs() == problemToSimplify.getConstraints()[constraintIndex_J].getRhs() &&
               (problemToSimplify.getConstraints()[constraintIndex_I].getType() != EQUAL && problemToSimplify.getConstraints()[constraintIndex_J].getType() != EQUAL)) {
                    helper.fixedVariables.emplace_back(basisVariable_I, problemToSimplify.getConstraints()[constraintIndex_I].getRhs());
                    helper.constraintsToRemove.push_back(constraintIndex_I);
                    helper.constraintsToRemove.push_back(constraintIndex_J);
            }
        }
    }
}

void LPSimplifier::searchForXIZeroVariables(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    for(const std::pair<uint, uint>& basisConstraint : helper.basisConstraintsInfo) {
        const uint constraintIndex = basisConstraint.first;
        const uint basisVariable = basisConstraint.second; // index i of non zero element in basis vector

        if(problemToSimplify.getConstraints()[constraintIndex].getType() == LESS_THAN_OR_EQUAL && problemToSimplify.getConstraints()[constraintIndex].getRhs() == 0) {
            helper.fixedVariables.emplace_back(basisVariable, 0);
            helper.constraintsToRemove.push_back(constraintIndex);
        }
    }
}

void LPSimplifier::searchForFixedVariables(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    getBasisConstraintsInfo(problemToSimplify, helper);

    searchForXIEqualsKConstraints(problemToSimplify, helper);

    searchForXIInequalityConstraints(problemToSimplify, helper);
    
    searchForXIZeroVariables(problemToSimplify, helper);
}

void LPSimplifier::searchForRepeatedConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    for(std::size_t i = 0; i < problemToSimplify.getConstraints().size(); i++) {
        for(std::size_t j = i + 1; j < problemToSimplify.getConstraints().size(); j++) {
            if(problemToSimplify.getConstraints()[i] == problemToSimplify.getConstraints()[j])
                helper.constraintsToRemove.push_back(j);
        }
    }
}

void LPSimplifier::searchForReducedFeasibleRegion(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    LpProblem h = problemToSimplify;
    SimplifierHelper newHelper = helper;
}

void LPSimplifier::newVarsToOldVars(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    std::vector<uint> fixedVars;
    for(std::pair<uint, double>& k : helper.fixedVariables) {
        fixedVars.push_back(k.first);
    }

    std::vector<std::pair<uint, uint>> pairsOfVars;
    
    std::vector<uint> freeVars;
    for(uint i = 0; i < problemToSimplify.getObjectiveFunction().getNColumns(); i++) {
        if(std::find(fixedVars.begin(), fixedVars.end(), i) == fixedVars.end()) {
            freeVars.push_back(i);
        }
    }

    for(uint i = 0; i < freeVars.size(); i++) {
        //pairsOfVars.emplace_back(i, freeVars[i]);
        helper.pairsOfVars.emplace_back(i, freeVars[i]);
    }

    //helper.pairsOfVars = pairsOfVars;
}

void LPSimplifier::removeRedundantInformationFromHelper(SimplifierHelper& helper) {
    // constraints to remove
    /*
    std::vector<uint> repeatedConstraintsIndexes;
    for(int i = 0; i < helper.constraintsToRemove.size(); i++) {
        for(int j = i + 1; j < helper.constraintsToRemove.size(); j++) {
            if(helper.constraintsToRemove[i] == helper.constraintsToRemove[j])
                repeatedConstraintsIndexes.push_back(j);
        }
    }

    std::sort(repeatedConstraintsIndexes.begin(), repeatedConstraintsIndexes.end(), [](uint& a, uint& b) {
        return a > b;
    });

    for(int i = 0; i < repeatedConstraintsIndexes.size(); i++) {
        helper.constraintsToRemove.erase(helper.constraintsToRemove.begin() + repeatedConstraintsIndexes[i]);
    }
    */
    
    // constraints to remove - NEW STRATEGY
    std::sort(helper.constraintsToRemove.begin(), helper.constraintsToRemove.end());
    auto last = std::unique(helper.constraintsToRemove.begin(), helper.constraintsToRemove.end());
    helper.constraintsToRemove.erase(last, helper.constraintsToRemove.end());
    /*
    std::sort(helper.constraintsToRemove.begin(), helper.constraintsToRemove.end(), [](uint& a, uint& b) {
        return a > b;
    });
    */

    // fixed variables
    /*
    std::vector<uint> repeatedFixedVariablesIndexes;
    for(int i = 0; i < helper.fixedVariables.size(); i++) {
        for(int j = i + 1; j < helper.fixedVariables.size(); j++) {
            if(helper.fixedVariables[i] == helper.fixedVariables[j])
                repeatedFixedVariablesIndexes.push_back(j);
        }
    }

    for(int i = 0; i < repeatedFixedVariablesIndexes.size(); i++) {
        helper.fixedVariables.erase(helper.fixedVariables.begin() + repeatedFixedVariablesIndexes[i]);
    }
    std::sort(helper.fixedVariables.begin(), helper.fixedVariables.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    */
    std::sort(helper.fixedVariables.begin(), helper.fixedVariables.end());
    auto stuff = std::unique(helper.fixedVariables.begin(), helper.fixedVariables.end());
    helper.fixedVariables.erase(stuff, helper.fixedVariables.end());

}

// PUBLIC METHODS

SimplifierHelper LPSimplifier::computeSimplifierHelper(LpProblem& originalProblem) {
    SimplifierHelper helper;

    searchForFixedVariables(originalProblem, helper);

    searchForRepeatedConstraints(originalProblem, helper);

    removeRedundantInformationFromHelper(helper);

    if(helper.fixedVariables.size() > 0) {
        newVarsToOldVars(originalProblem, helper);
    }

    return helper;
}

void LPSimplifier::simplifyProblem(LpProblem& problemToSimplify, SimplifierHelper& helper) {    
    if(helper.isHelperEmpty()) {
        return;
    }

    // remove all necessary constraints
    if(helper.constraintsToRemove.size() > 0) {
        for(int i = helper.constraintsToRemove.size() - 1; i >= 0; i--) {
            problemToSimplify.removeConstraint(helper.constraintsToRemove[i]);
        }
    }

    // remove all fixed variables
    if(helper.fixedVariables.size() > 0) {
        for(int i =  helper.fixedVariables.size() - 1; i >= 0; i--) {
            //std::cout << "removing variable " << helper.fixedVariables[i].first << ", with value " << helper.fixedVariables[i].second << "\n";
            problemToSimplify.removeFixedVariable(helper.fixedVariables[i].first, helper.fixedVariables[i].second);
        }
    }
}

}