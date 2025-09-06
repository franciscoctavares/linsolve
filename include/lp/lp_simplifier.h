#ifndef LP_SIMPLIFIER_H
#define LP_SIMPLIFIER_H

#include "lp/lp_utils.h"
#include "lp/lp.h"

namespace LP {

class LPSimplifier {
    private:
        static void searchForFixedVariables(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void searchForRepeatedConstraints(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void searchForReducedFeasibleRegion(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void removeRedundantInformationFromHelper(LpProblem& problemToSimplify, SimplifierHelper& helper);

        static void newVarsToOldVars(LpProblem& problemToSimplify, SimplifierHelper& helper);
    public:

        static SimplifierHelper computeSimplifierHelper(LpProblem& originalProblem);

        static void simplifyProblem(LpProblem& problemToSimplify, SimplifierHelper& helper);
};

}

#endif