#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>

#include "types.hpp"
#include "helpers.hpp"

#ifdef TEST
#include "../libs/acutest.h"
#endif

namespace maxmodels
{
    LogicProgram readFromSmodelsFormat(std::string programInSmodelsFormat)
    {
        // Input.
        std::istringstream input(programInSmodelsFormat);

        // Output.
        LogicProgram program;

        // Auxilary variables.
        unsigned int i, buffer, position, type, nofHeads, nofLiterals, nofNegativeLiterals, bound;
        std::string line;
        std::vector<unsigned int> definition;
        Head head;
        Atom atom;
        Literal literal;
        Label label;
        std::queue<CardinalityRule> cardinalityRules;
        std::queue<ChoiceRule> choiceRules;

        // Const.
        const unsigned int BASIC_RULE = 1;
        const unsigned int CARDINALITY_RULE = 2;
        const unsigned int CHOICE_RULE = 3;
        const unsigned int MINIMIZE_RULE = 6;
        std::vector<int> validTypes = {BASIC_RULE, CARDINALITY_RULE, CHOICE_RULE, MINIMIZE_RULE};

        // Section 1: Rules.
        while (std::getline(input, line))
        {
            if (line == "0" || line == "0\r")
            {
                break;
            }
            definition.clear();
            std::istringstream ss(line);
            while (ss >> buffer)
            {
                definition.push_back(buffer);
            }
            position = 0;
            type = definition.at(position++);
            if (std::find(validTypes.begin(), validTypes.end(), type) == validTypes.end())
            {
                // std::cerr << line << std::endl;
                throw std::runtime_error("The model consist an unsupported rule.");
            }
            Atoms heads;
            nofHeads = type == CHOICE_RULE ? definition.at(position++) : 1;
            for (i = 0; i < nofHeads; i++)
            {
                head = definition.at(position++);
                if (head > program.largest)
                {
                    program.largest = head;
                }
                heads.insert(head);
            }
            nofLiterals = definition.at(position++);
            nofNegativeLiterals = definition.at(position++);
            if (type == CARDINALITY_RULE)
            {
                bound = definition.at(position++);
            }
            Body body;
            for (i = 0; i < nofLiterals; ++i)
            {
                atom = definition.at(position);
                if (atom > program.largest)
                {
                    program.largest = atom;
                }
                body.insert(i < nofNegativeLiterals ? -atom : atom);
                if (type == MINIMIZE_RULE)
                {
                    program.weights[i < nofNegativeLiterals ? -atom : atom] = definition.at(position + nofLiterals);
                }
                position++;
            }
            if (type == BASIC_RULE)
            {
                Rule rule;
                rule.head = head;
                rule.body = body;
                program.rules.insert(rule);
            }
            else if (type == CARDINALITY_RULE && bound == 0)
            {
                Rule rule;
                rule.head = head;
                program.rules.insert(rule);
            }
            else if (type == CARDINALITY_RULE && bound >= body.size())
            {
                Rule rule;
                rule.head = head;
                rule.body = body;
                program.rules.insert(rule);
            }
            else if (type == CARDINALITY_RULE)
            {
                CardinalityRule rule;
                rule.head = head;
                rule.body = body;
                rule.bound = bound;
                cardinalityRules.push(rule);
            }
            else if (type == CHOICE_RULE)
            {
                ChoiceRule rule;
                rule.heads = heads;
                rule.body = body;
                choiceRules.push(rule);
            }
        }

        while (choiceRules.empty() == false)
        {
            ChoiceRule choiceRule = choiceRules.front();
            choiceRules.pop();

            Rule rule;
            rule.head = ++program.largest;
            rule.body = choiceRule.body;
            program.rules.insert(rule);

            for (Head head : choiceRule.heads)
            {
                Rule rule1;
                rule1.head = ++program.largest;
                rule1.body.insert(-head);
                program.rules.insert(rule1);

                Rule rule2;
                rule2.head = head;
                rule2.body.insert(rule.head);
                rule2.body.insert(-rule1.head);
                program.rules.insert(rule2);
            }
        }

        while (cardinalityRules.empty() == false)
        {
            CardinalityRule cardinalityRule = cardinalityRules.front();
            cardinalityRules.pop();

            unsigned int columns = cardinalityRule.bound + 2;
            unsigned int rows = cardinalityRule.body.size() + 1;
            unsigned int literalIndex;
            program.largest++; // Shift to the correct position as the first offset is 0.

            Rule rule;
            rule.head = cardinalityRule.head;
            rule.body.insert(program.largest + cardinalityRule.bound); // ctr(0, l)
            program.rules.insert(rule);

            for (unsigned int k = 0; k <= cardinalityRule.bound; k++)
            {
                literalIndex = 0;
                for (Literal literal : cardinalityRule.body)
                {
                    Rule rule1;
                    rule1.head = program.largest + literalIndex * columns + k + 1;         // ctr(i, k+1)
                    rule1.body.insert(program.largest + (literalIndex + 1) * columns + k); // ctr(i+1, k)
                    rule1.body.insert(literal);                                            // a
                    program.rules.insert(rule1);

                    Rule rule2;
                    rule2.head = program.largest + literalIndex * columns + k;             // ctr(i,k)
                    rule2.body.insert(program.largest + (literalIndex + 1) * columns + k); // ctr(i+1, k)
                    program.rules.insert(rule2);

                    literalIndex++;
                }
            }

            Rule rule3;
            rule3.head = program.largest + cardinalityRule.body.size() * columns;
            program.rules.insert(rule3);

            program.largest += rows * columns - 2;
        }

        // Section 2: Atoms' descriptions mapping.
        while (std::getline(input, line))
        {
            if (line == "0" || line == "0\r")
            {
                break;
            }
            std::istringstream ss(line);
            ss >> atom;
            ss >> label;
            program.mapping.insert(make_pair(atom, label));
        }

        // Section 3: B+.
        if (std::getline(input, line) && (line == "B+" || line == "B+\r"))
        {
            while (std::getline(input, line))
            {
                if (line == "0" || line == "0\r")
                {
                    break;
                }
                throw std::runtime_error("The B+ section is prohibited.");
            }
        }

        // Section 3: B-.
        if (std::getline(input, line) && (line == "B-" || line == "B-\r"))
        {
            Atoms shouldBeExcluded;
            while (std::getline(input, line))
            {
                if (line == "0" || line == "0\r")
                {
                    break;
                }
                shouldBeExcluded.insert(stoi(line));
            }
            if (shouldBeExcluded.size() > 1 || shouldBeExcluded.contains(1) == false)
            {
                throw std::runtime_error("The B- section is prohibited.");
            }
        }

        // The number of models to be returned.
        std::getline(input, line);
        if (stoi(line) != 1)
        {
            throw std::runtime_error("Only one model can be returned.");
        }

        return program;
    }
}

#ifdef TEST
struct Test
{
    const std::string inputFilename;
    const maxmodels::Rules expectedRules;
    const maxmodels::Mapping expectedMapping;
    const maxmodels::Weights expectedWeight;
    const maxmodels::Atom expectedLargest = 0;
    const bool expectException = false;
};
#define NOF_TESTS 13
Test tests[NOF_TESTS] = {
    {"tests/samples/valid01.smodels", {{2, -3}, {3, -2}, {1, 3}}, {{2, "a"}, {3, "b"}}, {}, 3},       // Simple program.
    {"tests/samples/invalid01.smodels", {}, {}, {}, 0, true},                                         // Unsupported rule.
    {"tests/samples/invalid02.smodels", {}, {}, {}, 0, true},                                         // Unsupported rule.
    {"tests/samples/valid02.smodels", {{2}, {3}}, {}, {}, 3},                                         // Only facts.
    {"tests/samples/invalid03.smodels", {}, {}, {}, 0, true},                                         // Section B+ has at least one element.
    {"tests/samples/invalid04.smodels", {}, {}, {}, 0, true},                                         // Section B- has at least two element.
    {"tests/samples/invalid05.smodels", {}, {}, {}, 0, true},                                         // Section B- has one element nut it differs from 1.
    {"tests/samples/valid03.smodels", {{4, 2}, {5, -3}, {3, 4, -5}}, {}, {}, 5},                      // Simple choice rule from a book.
    {"tests/samples/valid04.smodels", {{5, 2}, {6, -3}, {3, 5, -6}, {7, -4}, {4, 5, -7}}, {}, {}, 7}, // More complex choice rule.
    {
        "tests/samples/valid05.smodels",
        {{4, 6}, {6, 8, 2}, {5, 8}, {9, 11, 3}, {8, 11}, {7, 9, 2}, {6, 9}, {10, 12, 3}, {9, 12}, {11}},
        {},
        {},
        12},                                                                                       // Simple cardinality rule from a book.
    {"tests/samples/valid06.smodels", {{4}}, {}, {}, 4},                                           // Cardinality rules with the bound = 0 should be facts.
    {"tests/samples/valid07.smodels", {{4, 2, 3}}, {}, {}, 4},                                     // Cardinality rules with the bound = N should be normal rules.
    {"tests/samples/valid08.smodels", {{4}, {2, 4, -5}, {5, -2}, {3, 4, -6}, {6, -3}}, {}, {}, 6}, // Choice rule with an empty body.
};
void test_reading_from_smodels(void)
{
    for (unsigned int i = 0; i < NOF_TESTS; ++i)
    {
        std::string inputProgram = maxmodels::get_file_contents(tests[i].inputFilename);
        if (tests[i].expectException)
        {
            TEST_EXCEPTION(maxmodels::readFromSmodelsFormat(inputProgram), std::runtime_error);
        }
        else
        {
            maxmodels::LogicProgram actualProgram = maxmodels::readFromSmodelsFormat(inputProgram);
            TEST_CHECK(actualProgram.rules == tests[i].expectedRules);
            TEST_MSG("The expected rules:\n%s", maxmodels::as_char(maxmodels::as_string(tests[i].expectedRules)));
            TEST_MSG("The actual rules:\n%s", maxmodels::as_char(maxmodels::as_string(actualProgram.rules)));
            TEST_CHECK(actualProgram.mapping == tests[i].expectedMapping);
            TEST_CHECK(actualProgram.weights == tests[i].expectedWeight);
            TEST_CHECK(actualProgram.largest == tests[i].expectedLargest);
            TEST_MSG("The expected largest: %i", tests[i].expectedLargest);
            TEST_MSG("The actual largest: %i", actualProgram.largest);
        }
    }
}
TEST_LIST = {
    {"reading a provided program in the Smodels format", test_reading_from_smodels},
    {NULL, NULL}};
#endif