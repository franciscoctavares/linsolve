#pragma once

#include "lp/lp_utils.hpp"
#include "lp/lp.hpp"

namespace LP {

class LPSimplifier {
    private:
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