#include "lp/lp_simplifier.hpp"

#include <algorithm>
#include <iostream>
#include <format>
#include <cstddef>

namespace LP {

// PRIVATE METHODS

void LPSimplifier::searchForFixedVariables(LpProblem& problemToSimplify, SimplifierHelper& helper) {
    std::vector<std::pair<uint, uint>> basisConstraintsInfo; // .first is the constraint index, .second is the index i in xi <=/=/>= k
    
    // store information of all constraints of the form xi <=/=/>= k in basisConstraintsInfo
    for(std::size_t i = 0; i < problemToSimplify.getConstraints().size(); i++) {
        int basisVarIndex = isBasisVector(problemToSimplify.getConstraints()[i].getLhs());
        if(basisVarIndex != -1) basisConstraintsInfo.emplace_back(i, basisVarIndex);
    }

    for(std::size_t i = 0; i < basisConstraintsInfo.size(); i++) {
        int currentBasisConstraint_i = basisConstraintsInfo[i].first;

        // Check for constraints of the form xi = k, where i = 1,...,n and k is a real number
        if(problemToSimplify.getConstraints()[currentBasisConstraint_i].getType() == EQUAL) {
            helper.constraintsToRemove.push_back(currentBasisConstraint_i);
            helper.fixedVariables.emplace_back(basisConstraintsInfo[currentBasisConstraint_i].second, problemToSimplify.getConstraints()[currentBasisConstraint_i].getRhs());
        }

        // checks for any constraint of the types xi <= k and xi >= k   
        for(std::size_t j = i + 1; j < basisConstraintsInfo.size(); j++) {
            int currentBasisConstraint_j = basisConstraintsInfo[j].first;
            if(problemToSimplify.getConstraints()[currentBasisConstraint_i].getLhs() == problemToSimplify.getConstraints()[currentBasisConstraint_j].getLhs() &&
               problemToSimplify.getConstraints()[currentBasisConstraint_i].getType() != problemToSimplify.getConstraints()[currentBasisConstraint_j].getType() &&
               problemToSimplify.getConstraints()[currentBasisConstraint_i].getRhs() == problemToSimplify.getConstraints()[currentBasisConstraint_j].getRhs() &&
               (problemToSimplify.getConstraints()[currentBasisConstraint_i].getType() != EQUAL && problemToSimplify.getConstraints()[currentBasisConstraint_j].getType() != EQUAL)) {
                    helper.fixedVariables.emplace_back(basisConstraintsInfo[i].second, problemToSimplify.getConstraints()[currentBasisConstraint_i].getRhs());
                    helper.constraintsToRemove.push_back(currentBasisConstraint_i);
                    helper.constraintsToRemove.push_back(currentBasisConstraint_j);
            }
        }

        // Check for constraints of the form xi <= 0
        if(problemToSimplify.getConstraints()[currentBasisConstraint_i].getType() == LESS_THAN_OR_EQUAL && problemToSimplify.getConstraints()[currentBasisConstraint_i].getRhs() == 0) {
            helper.fixedVariables.emplace_back(basisConstraintsInfo[i].second, 0);
            //helper->fixedVariables.emplace_back(basisConstraintsInfo[i].second, 0);
            helper.constraintsToRemove.push_back(currentBasisConstraint_i);
        }
    }
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
            problemToSimplify.removeFixedVariable(helper.fixedVariables[i].first, helper.fixedVariables[i].second);
        }
    }
}

}