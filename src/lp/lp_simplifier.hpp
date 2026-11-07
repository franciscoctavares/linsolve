#pragma once

#include "lp/lp_utils.hpp"
#include "lp/lp.hpp"

namespace LP {

class LPSimplifier {
    private:
        static void getBasisConstraintsInfo(LpProblem& problemToSimplify, SimplifierHelper& helper);

        /**
         * @brief Searches for constraints of the form `x_i = k`
         * 
         */
        static void searchForXIEqualsKConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper);

        /**
         * @brief Searches for pairs of constraints in the form `x_i <= k` and `x_i >= k`
         * 
         */
        static void searchForXIInequalityConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper);

        /**
         * @brief Searches for constraints of the form `x_i <= 0`
         * 
         */
        static void searchForXIZeroVariables(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void searchForFixedVariables(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void searchForRepeatedConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void searchForReducedFeasibleRegion(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void removeRedundantInformationFromHelper(SimplifierHelper& helper);

        static void newVarsToOldVars(LpProblem& problemToSimplify, SimplifierHelper& helper);
    public:

        static SimplifierHelper computeSimplifierHelper(LpProblem& originalProblem);

        static void simplifyProblem(LpProblem& problemToSimplify, SimplifierHelper& helper);
};

}