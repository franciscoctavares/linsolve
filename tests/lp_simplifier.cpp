#include "catch2/catch_test_macros.hpp"
#include "lp/constraint.hpp"
#include "lp/lp.hpp"
#include <vector>
#include "lp/lp_simplifier.hpp"



TEST_CASE("LPSimplifier::searchForFixedVariables()", ) {

    SECTION("LPSimplifier::searchForFixedVariables() finds constraints of the type x_i = k") {
        Constraint c1({1, 0, 0}, "=", 3);
        Constraint c2({1, 1, 0}, "<=", 4);
        Constraint c3({0, 1, 1}, "<=", 6);

        LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});

        SECTION("LPSimplifier::searchForFixedVariables() finds one constraint of the type x_i = k as the first constraint") {
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);

            REQUIRE(helper.fixedVariables.size() == 1);
            REQUIRE(helper.fixedVariables[0] == std::make_pair<uint, double>(0, 3));

            REQUIRE(helper.constraintsToRemove.size() == 1);
            REQUIRE(helper.constraintsToRemove[0] == 0);

            REQUIRE(helper.pairsOfVars.size() == 2);
            REQUIRE(helper.pairsOfVars[0] == std::make_pair<uint, uint>(0, 1));
            REQUIRE(helper.pairsOfVars[1] == std::make_pair<uint, uint>(1, 2));

            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 0}, "<=", 1));

        }

        SECTION("LPSimplifier::searchForFixedVariables() finds one constraint of the type x_i = k as neither the first or last constraint") {
            model.setConstraints({
                Constraint({1, 1, 0}, "<=", 4),
                Constraint({1, 0, 0}, "=", 3),
                Constraint({0, 1, 1}, "<=", 6)
            });

            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);

            REQUIRE(helper.fixedVariables.size() == 1);
            REQUIRE(helper.fixedVariables[0] == std::make_pair<uint, double>(0, 3));
            REQUIRE(helper.constraintsToRemove.size() == 1);
            REQUIRE(helper.constraintsToRemove[0] == 1);
            REQUIRE(helper.pairsOfVars.size() == 2);
            REQUIRE(helper.pairsOfVars[0] == std::make_pair<uint, uint>(0, 1));
            REQUIRE(helper.pairsOfVars[1] == std::make_pair<uint, uint>(1, 2));

            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 0}, "<=", 1));
        }

        SECTION("LPSimplifier::searchForFixedVariables() finds one constraint of the type x_i = k as the last constraint") {
            model.setConstraints({
                Constraint({1, 1, 0}, "<=", 4),
                Constraint({0, 1, 1}, "<=", 6),
                Constraint({1, 0, 0}, "=", 3)
            });

            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);

            REQUIRE(helper.fixedVariables.size() == 1);
            REQUIRE(helper.fixedVariables[0] == std::make_pair<uint, double>(0, 3));
            REQUIRE(helper.constraintsToRemove.size() == 1);
            REQUIRE(helper.constraintsToRemove[0] == 2);
            REQUIRE(helper.pairsOfVars.size() == 2);
            REQUIRE(helper.pairsOfVars[0] == std::make_pair<uint, uint>(0, 1));
            REQUIRE(helper.pairsOfVars[1] == std::make_pair<uint, uint>(1, 2));

            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 0}, "<=", 1));
        }

        SECTION("LPSimplifier::searchForFixedVariables() finds more than one constraint of the type x_i = k") {
            LP::LpProblem model2(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});

            model2.setConstraints({
                Constraint({1, 1, 0}, "<=", 4),
                Constraint({0, 1, 1}, "<=", 6),
                Constraint({1, 0, 0}, "=", 3),
                Constraint({0, 1, 0}, "=", 4)
            });

            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model2);

            helper.displayHelper();

            REQUIRE(helper.fixedVariables.size() == 2);
            REQUIRE(helper.fixedVariables[0] == std::make_pair<uint, double>(0, 3));
            REQUIRE(helper.fixedVariables[1] == std::make_pair<uint, double>(0, 4));
            REQUIRE(helper.constraintsToRemove.size() == 2);
            REQUIRE(helper.constraintsToRemove[0] == 2);
            REQUIRE(helper.constraintsToRemove[1] == 3);
            REQUIRE(helper.pairsOfVars.size() == 2);
            REQUIRE(helper.pairsOfVars[0] == std::make_pair<uint, uint>(2, 0));
            REQUIRE(helper.pairsOfVars[1] == std::make_pair<uint, uint>(1, 2));

        }
    }

}