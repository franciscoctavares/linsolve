#include "catch2/catch_test_macros.hpp"
#include "lp/constraint.hpp"
#include <vector>

static std::vector<double> v1 = {1, 2, 3, 4};

TEST_CASE("Constraint::Constraint()", "[Constraint][Constructor]") {

    SECTION("Constraint::Constraint() throws an exception if newConstraintType is not <=, >= or =") {
        REQUIRE_THROWS(Constraint(v1, "<>", 3));
        REQUIRE_THROWS(Constraint(v1, "123", 3));
        REQUIRE_THROWS(Constraint(v1, "abc", 3));

        REQUIRE_NOTHROW(Constraint(v1, "<=", 3));
        REQUIRE_NOTHROW(Constraint(v1, ">=", 3));
        REQUIRE_NOTHROW(Constraint(v1, "=", 3));
    }

    SECTION("Constraint::Constraint() produces the correct result") {
        Constraint a(v1, "<=", 3);
        REQUIRE(a.getLhs() == v1);
        REQUIRE(a.getRhs() == 3);
        REQUIRE(a.getType() == LESS_THAN_OR_EQUAL);
    }

}

TEST_CASE("Constraint::removeFixedVariable()", "[Constraint][removeFixedVariable]") {
    Constraint a(v1, "<=", 3);

    SECTION("Constraint::removeFixedVariable() throws an exception if the varIndex argument is out of bounds") {
        REQUIRE_THROWS(a.removeFixedVariable(-1, 3));
        REQUIRE_THROWS(a.removeFixedVariable(4, 3));
        Constraint b = a;
        REQUIRE_NOTHROW(b.removeFixedVariable(0, 1));
        b = a;
        REQUIRE_NOTHROW(b.removeFixedVariable(1, 1));
        b = a;
        REQUIRE_NOTHROW(b.removeFixedVariable(2, 1));
        b = a;
        REQUIRE_NOTHROW(b.removeFixedVariable(3, 1));
    }

    SECTION("Constraint::removeFixedVariable() produces the correct result") {
        a.removeFixedVariable(0, 3);
        REQUIRE(a.getLhs().size() == 3);
        REQUIRE(a.getLhs()[0] == 2);
        REQUIRE(a.getLhs()[1] == 3);
        REQUIRE(a.getLhs()[2] == 4);
        REQUIRE(a.getRhs() == 0);
    }

}