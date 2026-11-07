#include "catch2/catch_test_macros.hpp"
#include "lp/constraint.hpp"
#include "lp/lp.hpp"
#include <vector>
#include "lp/lp_simplifier.hpp"



TEST_CASE("LPSimplifier::searchForFixedVariables()", "[LPSimplifier][searchForFixedVariables]") {
    SECTION("LPSimplifier::searchForFixedVariables() finds constraints of the type x_i = k") {
        SECTION("No constraints of the type x_i = k") {
            Constraint c1({1, 0, 0}, "<=", 3);
            Constraint c2({1, 1, 0}, "<=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 3);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 0, 0}, "<=", 3));
        }

        SECTION("One or more constraints of the type x_i = k") {
            Constraint c1({1, 0, 0}, "=", 3);
            Constraint c2({1, 1, 0}, "<=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 0}, "<=", 1));   
        }
    }

    SECTION("LPSimplifier::searchForFixedVariables() searches for pairs of constraints of the type x_i <= k and x_i >= k") {
        SECTION("No pairs of constraints of the type x_i <= k and x_i >= k") {
            Constraint c1({1, 0, 0}, "<=", 3);
            Constraint c2({1, 1, 0}, "<=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 3);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 0, 0}, "<=", 3));
        }

        SECTION("One or more pairs of constraints of the type x_i <= k and x_i >= k") {
            Constraint c1({1, 0, 0}, "<=", 3);
            Constraint c2({1, 0, 0}, ">=", 3);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 1);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 1}, "<=", 6));
        }
    }

    SECTION("LPSimplifier::searchForFixedVariables() searches for constrains of the type x_i <= 0") {
        SECTION("No constraints of the type x_i <= 0") {
            Constraint c1({1, 1, 0}, "<=", 4);
            Constraint c2({1, 2, 0}, "<=", 8);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 3);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 1, 0}, "<=", 4));
        }

        SECTION("One or more constraints of the type x_i <= 0") {
            Constraint c1({1, 0, 0}, "<=", 0);
            Constraint c2({1, 0, 0}, ">=", 3);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({0, 0}, ">=", 3));
        }
    }
}

TEST_CASE("LPSimplifier::searchForRepeatedConstraints()", "[LPSimplifier][searchForRepeatedConstraints]") {
    SECTION("LPSimplifier::searchForRepeatedConstraints() produces the correct output") {
        SECTION("There are no repeated constraints") {
            Constraint c1({1, 2, 0}, "<=", 7);
            Constraint c2({1, 1, 0}, "<=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 3);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 2, 0}, "<=", 7));
        }

        SECTION("There are repeated constraints") {
            Constraint c1({1, 1, 0}, "<=", 4);
            Constraint c2({1, 1, 0}, "<=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 1, 0}, "<=", 4));
        }
    }
}

TEST_CASE("LPSimplifier::removeRedundantInformationFromHelper()", "[LPSimplifier][removeRedundantInformationFromHelper])") {
    SECTION("removeRedundantInformationFromHelper produces the correct output") {
        SECTION("No repeated information") {
            Constraint c1({1, 1, 0}, "<=", 4);
            Constraint c2({1, 2, 0}, ">=", 6);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 3);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 1, 0}, "<=", 4));
        }

        SECTION("Repeated constraints to remove") {
            Constraint c1({1, 1, 0}, "<=", 4);
            Constraint c2({1, 1, 0}, "<=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);

            REQUIRE(helper.constraintsToRemove == std::vector<uint>{1});

            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1, 1}, 1, 3));
            REQUIRE(model.getConstraints().size() == 2);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 1, 0}, "<=", 4));
        }

        SECTION("Repeated fixed variables") {
            Constraint c1({1, 0, 0}, "<=", 4);
            Constraint c2({1, 0, 0}, ">=", 4);
            Constraint c3({0, 1, 1}, "<=", 6);
            Constraint c4({1, 0, 0}, "=", 4);

            LP::LpProblem model(LP::ProblemType::MAX, {1, 1, 1}, {c1, c2, c3});
            LP::SimplifierHelper helper = LP::LPSimplifier::computeSimplifierHelper(model);
            LP::LPSimplifier::simplifyProblem(model, helper);

            REQUIRE(model.getObjectiveFunction() == Matrix({1, 1}, 1, 2));
            REQUIRE(model.getConstraints().size() == 1);
            REQUIRE(model.getConstraints()[0] == Constraint({1, 1}, "<=", 6));
        }
    }
}