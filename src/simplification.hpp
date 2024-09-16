#include <algorithm>

#include "types.hpp"

#ifdef TEST
#include "../libs/acutest.h"
#endif

namespace maxmodels
{
    SimplifiedProgram simplify(LogicProgram program)
    {
        // Output.
        SimplifiedProgram simplifiedProgram;

        // Auxiliary variables.
        Atoms heads;
        Rules rules;
        Rules copyOfRules;
        bool noMoreChangesInRulesAndFacts;
        bool simplifiedProgramIsCompleted;

        // Step 1: Get all facts from the input program.
        for (auto rule : program.rules)
        {
            if (rule.isFact())
            {
                simplifiedProgram.facts.insert(rule.head);
            }
        }
        for (Atom atom : program.founded)
        {
            simplifiedProgram.facts.insert(atom);
        }

        // Step 2: Initialize the simplified program with the input program's rules.
        simplifiedProgram.rules.clear();
        simplifiedProgram.rules.insert(program.rules.begin(), program.rules.end());

    // Start the simplification. The below label will be used for looping the procedure.
    start:

        // Step 3: Get all rules and constraints excluding facts.
        // Note that from now, the simplified program is used!
        rules.clear();
        for (auto rule : simplifiedProgram.rules)
        {
            if (rule.isFact() == false)
            {
                rules.insert(rule);
            }
        }

        // Steps from 4a to 4f.
        noMoreChangesInRulesAndFacts = false;
        while (noMoreChangesInRulesAndFacts == false)
        {
            noMoreChangesInRulesAndFacts = true;
            copyOfRules.clear();
            for (const auto &rule : rules)
            {
                // Step 4a: If a body is empty, then it means that the rule is a fact.
                if (rule.isFact())
                {
                    simplifiedProgram.facts.insert(rule.head);
                    noMoreChangesInRulesAndFacts = false;
                    continue;
                }
                // Step 4b: If a body contains a negative literal, which is already a fact.
                else if (
                    rule.isConstraint() == false && std::any_of(rule.body.cbegin(), rule.body.cend(), [&simplifiedProgram](const int &literal)
                                                                { return simplifiedProgram.facts.contains(-literal); }))
                {
                    noMoreChangesInRulesAndFacts = false;
                    continue;
                }
                // Step 4c: If a head is already a fact.
                else if (
                    rule.isConstraint() == false && simplifiedProgram.facts.contains(rule.head))
                {
                    noMoreChangesInRulesAndFacts = false;
                    continue;
                }
                // Step 4e: If a body contains a head.
                else if (
                    rule.isConstraint() == false && std::any_of(rule.body.cbegin(), rule.body.cend(), [&rule](const int &literal)
                                                                { return literal == rule.head; }))
                {
                    noMoreChangesInRulesAndFacts = false;
                    continue;
                }
                // Step 4f: If a rule is a constraint, and it has an empty body.
                else if (rule.isConstraint() && rule.body.size() == 0)
                {
                    simplifiedProgram.isIncoherent = true;
                    return simplifiedProgram;
                }
                // Step 4d All facts in rules are removed.
                else
                {
                    Rule copyOfRule;
                    copyOfRule.head = rule.head;
                    for (const auto &literal : rule.body)
                    {
                        if (simplifiedProgram.facts.contains(literal) == false)
                        {
                            copyOfRule.body.insert(literal);
                        }
                        else
                        {
                            noMoreChangesInRulesAndFacts = false;
                        }
                    }
                    copyOfRules.insert(copyOfRule);
                }
            }
            rules.clear();
            rules.insert(copyOfRules.begin(), copyOfRules.end());
        }

        // Step 6. Get all head from the rules.
        heads.clear();
        for (const auto &rule : rules)
        {
            if (rule.isConstraint() == false)
            {
                heads.insert(rule.head);
            }
        }

        // Step 7.
        simplifiedProgram.rules.clear();

        // Step 8.
        simplifiedProgramIsCompleted = true;

        // Steps 9a and 9b.
        for (const auto &rule : rules)
        {
            const unsigned int initialRuleSize = rule.body.size();

            // Step 9a.
            if (rule.isConstraint() == false)
            {
                if (std::all_of(
                        rule.body.cbegin(),
                        rule.body.cend(),
                        [&heads](const int &literal)
                        { return literal < 0 || heads.contains(literal); }))
                {
                    Rule copyOfRule;
                    copyOfRule.head = rule.head;
                    for (const auto &literal : rule.body)
                    {
                        if (literal < 0 && heads.contains(-literal) == false)
                        {
                            continue;
                        }
                        copyOfRule.body.insert(literal);
                    }
                    if (copyOfRule.body.size() == 0)
                    {
                        simplifiedProgram.facts.insert(copyOfRule.head);
                        simplifiedProgramIsCompleted = false;
                    }
                    else
                    {
                        simplifiedProgram.rules.insert(copyOfRule);
                        if (initialRuleSize > copyOfRule.body.size())
                        {
                            simplifiedProgramIsCompleted = false;
                        }
                    }
                }
            }

            // Step 9b.
            else
            {
                if (all_of(
                        rule.body.cbegin(),
                        rule.body.cend(),
                        [&heads](const int &literal)
                        { return literal < 0 || heads.contains(literal); }))
                {
                    Rule copyOfRule;
                    copyOfRule.head = rule.head;
                    for (const auto &literal : rule.body)
                    {
                        if (literal < 0 && heads.contains(-literal) == false)
                        {
                            continue;
                        }
                        copyOfRule.body.insert(literal);
                    }
                    if (copyOfRule.body.size() == 0)
                    {
                        simplifiedProgram.isIncoherent = true;
                        return simplifiedProgram;
                    }
                    else
                    {
                        simplifiedProgram.rules.insert(copyOfRule);
                        if (initialRuleSize > rule.body.size())
                        {
                            simplifiedProgramIsCompleted = false;
                        }
                    }
                }
            }
        }

        if (simplifiedProgramIsCompleted == false)
        {
            goto start;
        }

        simplifiedProgram.simplificationRatio = simplifiedProgram.rules.size() / (double)program.rules.size();

        return simplifiedProgram;
    }
}

#ifdef TEST
struct Test
{
    const char *name;
    const maxmodels::Rules rules;
    const maxmodels::Atoms expectedFacts;
    const maxmodels::Rules expectedSimplifiedRules;
    const bool expectIncoherent;
};
#define NOF_TESTS 8
Test tests[NOF_TESTS] = {
    {"an empty program", {}, {}, {}, false},
    {"a single fact", {{2}}, {2}, {}, false},
    {"a simple program", {{2}, {3, 2}, {4, 3}}, {2, 3, 4}, {}, false},
    {"a program with a negation", {{2, -3}, {3, -2}}, {}, {{2, -3}, {3, -2}}, false},
    {"a program with a negation and a single fact", {{2, -3}, {3, -2}, {3}}, {3}, {}, false},
    {"a program with a constraint", {{2}, {3, 2}, {1, 3}}, {}, {}, true},
    {"a exemplary program", {{2, -3}, {3, -2}, {4, -5}, {5, -4}, {6, -7}, {7, -6}, {1, 2, 5}, {1, 3, 4}, {1, 7, 4}, {6}}, {6}, {{2, -3}, {3, -2}, {4, -5}, {5, -4}, {1, 2, 5}, {1, 3, 4}}, false}};
void test_simplify(void)
{
    for (unsigned int i = 0; i < NOF_TESTS; ++i)
    {
        TEST_CASE(tests[i].name);
        maxmodels::LogicProgram program;
        program.rules = tests[i].rules;
        maxmodels::SimplifiedProgram simplifiedProgram = maxmodels::simplify(program);
        if (tests[i].expectIncoherent)
        {
            TEST_CHECK(simplifiedProgram.isIncoherent == true);
        }
        else
        {
            TEST_CHECK(simplifiedProgram.facts == tests[i].expectedFacts);
            TEST_CHECK(simplifiedProgram.rules == tests[i].expectedSimplifiedRules);
        }
    }
}
TEST_LIST = {
    {"simplification of a program and verification its concluded facts", test_simplify},
    {NULL, NULL}};
#endif