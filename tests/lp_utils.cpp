#include "catch2/catch_test_macros.hpp"
#include "lp/lp_utils.hpp"

TEST_CASE("SimplifierHelper::isHelperEmpty()", "[SimplifierHelper][isHelperEmpty]") {
    LP::SimplifierHelper helper;
    SECTION("SimplifierHelper::isHelperEmpty() produces the correct output") {
        // 0 0 0
        REQUIRE(helper.isHelperEmpty());

        // NOT(0) 0 0
        helper.fixedVariables.push_back(std::make_pair(0, 2));
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.fixedVariables.clear();

        // 0 NOT(0) 0
        helper.constraintsToRemove.push_back(0);
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.constraintsToRemove.clear();

        // 0 0 NOT(0)
        helper.pairsOfVars.push_back(std::make_pair(0, 0));
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.pairsOfVars.clear();

        // NOT(0) NOT(0) 0
        helper.fixedVariables.push_back(std::make_pair(0, 2));
        helper.constraintsToRemove.push_back(0);
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.fixedVariables.clear();
        helper.constraintsToRemove.clear();

        // 0 NOT(0) NOT(0)
        helper.constraintsToRemove.push_back(0);
        helper.pairsOfVars.push_back(std::make_pair(0, 0));
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.constraintsToRemove.clear();
        helper.pairsOfVars.clear();

        // NOT(0) 0 NOT(0)
        helper.fixedVariables.push_back(std::make_pair(0, 2));
        helper.pairsOfVars.push_back(std::make_pair(0, 0));
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.fixedVariables.clear();
        helper.pairsOfVars.clear();

        // NOT(0) NOT(0) NOT(0)
        helper.fixedVariables.push_back(std::make_pair(0, 2));
        helper.constraintsToRemove.push_back(0);
        helper.pairsOfVars.push_back(std::make_pair(0, 0));
        REQUIRE_FALSE(helper.isHelperEmpty());
        helper.fixedVariables.clear();
        helper.constraintsToRemove.clear();
        helper.pairsOfVars.clear();
    }
}

TEST_CASE("SimplifierHelper::isNumberAnInteger()", "[SimplifierHelper][isNumberAnInteger]") {
    SECTION("SimplifierHelper::isNumberAnInteger() produces the correct output") {
        REQUIRE(LP::isNumberAnInteger(4, 1e-10));
        REQUIRE_FALSE(LP::isNumberAnInteger(4.1, 1e-10));
    }
}