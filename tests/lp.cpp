#include "catch2/catch_test_macros.hpp"
#include "lp/lp.hpp"
#include "lp/lp_utils.hpp"

TEST_CASE("LpProblem::LpProblem(default constructor)", "[LpProblem][Constructor]") {
    SECTION("Default constructor produces the correct result") {
        LP::LpProblem model;

        REQUIRE(model.getObjectiveFunction().getElements().size() == 0);
        REQUIRE(model.getConstraints().size() == 0);
        REQUIRE(model.getOptimalSolution().getElements().size() == 0);
    }
}

TEST_CASE("LpProblem::getObjectiveFunction()", "[LpProblem][getObjectiveFunction]") {
    SECTION("getObjectiveFunction produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2, 3}, {Constraint({1, 2, 3}, "<=", 5)});

        REQUIRE(model.getObjectiveFunction() == Matrix({1, 2, 3}, 1, 3));
    }
}

TEST_CASE("LpProblem::getOptimalSolution()", "[LpProblem][getOptimalSolution]") {
    SECTION("getOptimalSolution produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2, 3}, {Constraint({1, 2, 3}, "<=", 5)});
        REQUIRE(model.getOptimalSolution() == Matrix({0, 0, 0}, 1, 3));

        model.setOptimalSolution(Matrix({1, 2, 3}, 1, 3));
        REQUIRE(model.getOptimalSolution() == Matrix({1, 2, 3}, 1, 3));
    }
}

TEST_CASE("LpProblem::getConstraints()", "[LpProblem][getConstraints]") {
    SECTION("getConstraints produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2, 3}, {Constraint({1, 2, 3}, "<=", 5)});

        REQUIRE(model.getConstraints().size() == 1);
        REQUIRE(model.getConstraints()[0] == Constraint({1, 2, 3}, "<=", 5));
    }
}

TEST_CASE("LpProblem::getType()", "[LpProblem][getType]") {
    SECTION("getType produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2, 3}, {Constraint({1, 2, 3}, "<=", 5)});
        REQUIRE(model.getType() == LP::MAX);

        LP::LpProblem model2(LP::MIN, {1, 2, 3}, {Constraint({1, 2, 3}, "<=", 5)});
        REQUIRE(model2.getType() == LP::MIN);
    }
}

TEST_CASE("LpProblem::getSolutionType()", "[LpProblem][getSolutionType]") {
    SECTION("getSolutionType produces the correct result") {
        LP::LpProblem model;
        
        model.setSolutionType(LP::SolutionType::CONTINUOUS_SOLUTION);
        REQUIRE(model.getSolutionType() == LP::SolutionType::CONTINUOUS_SOLUTION);

        model.setSolutionType(LP::SolutionType::WHOLE_SOLUTION);
        REQUIRE(model.getSolutionType() == LP::SolutionType::WHOLE_SOLUTION);

        model.setSolutionType(LP::SolutionType::INFEASIBLE);
        REQUIRE(model.getSolutionType() == LP::SolutionType::INFEASIBLE);

        model.setSolutionType(LP::SolutionType::UNBOUNDED);
        REQUIRE(model.getSolutionType() == LP::SolutionType::UNBOUNDED);
    }
}

TEST_CASE("LpProblem::LpProblem(constructor)", "[LpProblem][Constructor]") {
    SECTION("The constructor produces the expected output") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});

        REQUIRE(model.getType() == LP::MAX);
        REQUIRE(model.getObjectiveFunction() == Matrix({1, 2}, 1, 2));
        REQUIRE(model.getConstraints().size() == 1);
        REQUIRE(model.getConstraints()[0] == Constraint({3, 4}, "<=", 5));
        REQUIRE(model.getOptimalSolution() == Matrix({0, 0}, 1, 2));
    }
}

TEST_CASE("LpProblem::LpProblem(copy constructor)", "[LpProblem][Constructor]") {
    SECTION("The copy constructor produces the expected output") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});

        LP::LpProblem model2(model);
        REQUIRE(model2.getType() == LP::MAX);
        REQUIRE(model2.getObjectiveFunction() == Matrix({1, 2}, 1, 2));
        REQUIRE(model2.getConstraints().size() == 1);
        REQUIRE(model2.getConstraints()[0] == Constraint({3, 4}, "<=", 5));
        REQUIRE(model2.getOptimalSolution() == Matrix({0, 0}, 1, 2));
    }
}

TEST_CASE("LpProblem::LpProblem(copy assignment constructor)", "[LpProblem][Constructor]") {

    SECTION("The copy assignment constructor produces the expected output") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});

        LP::LpProblem model2;
        model2 = model;
        REQUIRE(model2.getType() == LP::MAX);
        REQUIRE(model2.getObjectiveFunction() == Matrix({1, 2}, 1, 2));
        REQUIRE(model2.getConstraints().size() == 1);
        REQUIRE(model2.getConstraints()[0] == Constraint({3, 4}, "<=", 5));
        REQUIRE(model2.getOptimalSolution() == Matrix({0, 0}, 1, 2));

        model = model;
        REQUIRE(model.getType() == LP::MAX);
        REQUIRE(model.getObjectiveFunction() == Matrix({1, 2}, 1, 2));
        REQUIRE(model.getConstraints().size() == 1);
        REQUIRE(model.getConstraints()[0] == Constraint({3, 4}, "<=", 5));
        REQUIRE(model.getOptimalSolution() == Matrix({0, 0}, 1, 2));
    }
}

TEST_CASE("LpProblem::addConstrtaint()", "[LpProblem][addConstraint]") {
    SECTION("addConstraint produces the correct output") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        model.addConstraint(Constraint({3, 4}, "<=", 5));

        REQUIRE(model.getConstraints().size() == 2);
        REQUIRE(model.getConstraints()[0] == Constraint({3, 4}, "<=", 5));
        REQUIRE(model.getConstraints()[1] == Constraint({3, 4}, "<=", 5));
    }
}

TEST_CASE("LpProblem::removeConstraint()", "[LpProblem][removeConstraint]") {
    SECTION("removeConstraint throws an exception if constraintIndex is out of bounds") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        REQUIRE_THROWS(model.removeConstraint(-1));
        REQUIRE_THROWS(model.removeConstraint(1));
        REQUIRE_NOTHROW(model.removeConstraint(0));
    }

    SECTION("removeConstraint produces the correct output") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        model.addConstraint(Constraint({1, 2}, "<=", 5));
        REQUIRE_NOTHROW(model.removeConstraint(0));
        REQUIRE(model.getConstraints().size() == 1);
        REQUIRE(model.getConstraints()[0] == Constraint({1, 2}, "<=", 5));
    }
}

TEST_CASE("LpProblem::isConstraintSatisfied()", "[LpProblem][isConstraintSatisfied]") {
    SECTION("isConstraintSatisfied throws an exception if constraintIndex is out of bounds") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        REQUIRE_THROWS(model.isConstraintSatisfied(Matrix({1, 2}, 1, 2), -1));
        REQUIRE_THROWS(model.isConstraintSatisfied(Matrix({1, 2}, 1, 2), 1));
        REQUIRE_NOTHROW(model.isConstraintSatisfied(Matrix({1, 2}, 1, 2), 0));
    }

    SECTION("isConstraintSatisfied produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({1, 1}, "<=", 5), Constraint({1, 1}, "=", 5), Constraint({1, 1}, ">=", 5)});

        REQUIRE(model.isConstraintSatisfied(Matrix({1, 3}, 1, 2), 0));
        REQUIRE_FALSE(model.isConstraintSatisfied(Matrix({1, 5}, 1, 2), 0));

        REQUIRE(model.isConstraintSatisfied(Matrix({1, 4}, 1, 2), 1));
        REQUIRE_FALSE(model.isConstraintSatisfied(Matrix({1, 5}, 1, 2), 1));

        REQUIRE(model.isConstraintSatisfied(Matrix({1, 5}, 1, 2), 2));
        REQUIRE_FALSE(model.isConstraintSatisfied(Matrix({1, 2}, 1, 2), 2));
    }
}

TEST_CASE("LpProblem::isSolutionAdmissible()", "[LpProblem][isSolutionAdmissible]") {
    SECTION("isSolutionAdmissible produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});

        REQUIRE(model.isSolutionAdmissible(Matrix({1, 0.5}, 1, 2)));
        REQUIRE_FALSE(model.isSolutionAdmissible(Matrix({1, 1}, 1, 2)));
        REQUIRE_FALSE(model.isSolutionAdmissible(Matrix({-1, 1}, 1, 2)));
    }
}

TEST_CASE("LpProblem::removeFixedVariable()", "[LpProblem][removeFixedVariable]") {
    SECTION("removeFixedVariable throws an exception if varIndex is out of bounds") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        REQUIRE_THROWS(model.removeFixedVariable(-1, 4));
        REQUIRE_THROWS(model.removeFixedVariable(2, 4));
        REQUIRE_NOTHROW(model.removeFixedVariable(0, 4));
    }

    SECTION("removeFixedVariable produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        model.removeFixedVariable(0, 1);
        REQUIRE(model.getObjectiveFunction() == Matrix({2}, 1, 1));
        REQUIRE(model.getConstraints().size() == 1);
        REQUIRE(model.getConstraints()[0] == Constraint({4}, "<=", 2));
    }
}

TEST_CASE("LpProblem::solveProblem()", "[LpProblem][solveProblem]") {
    SECTION("solveProblem produces the correct result") {
        LP::LpProblem model(LP::MAX, {1, 2}, {Constraint({3, 4}, "<=", 5)});
        model.solveProblem(LP::SIMPLEX);
        REQUIRE(model.getSolutionType() == LP::CONTINUOUS_SOLUTION);
        REQUIRE(model.getOptimalSolution() == Matrix({0, 1.25}, 1, 2));
    }
}