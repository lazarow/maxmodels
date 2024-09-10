#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "../libs/System2.h"

#include "types.hpp"

namespace maxmodels
{
    struct SolvingOutcome
    {
        bool isIncoherent = false;
        bool isTimeout = false;

        bool isError = false;
        std::string errorMessage;

        // An exclusion clause is used if the supporting model is not an answer set.
        std::string exclusionClause;

        Atoms supportingModel;
    };

    SolvingOutcome solve(const EncodedProgram &encoding, const std::string &solverCommand, bool printOutput = false)
    {
        SolvingOutcome result;

        // Creating the solving command.
        System2CommandInfo commandInfo;
        memset(&commandInfo, 0, sizeof(System2CommandInfo));
        commandInfo.RedirectInput = true;
        commandInfo.RedirectOutput = true;
        System2Run(solverCommand.c_str(), &commandInfo);

        // Passing the input clauses to the solver.
        System2WriteToInput(&commandInfo, encoding.wcnf.c_str(), encoding.wcnf.size());
        // It is my mod for the library to close the pipe.
        System2CloseInput(&commandInfo);

        std::string output;
        char outputBuffer[1024];
        uint32_t bytesRead = 0;
        int resultFlag = SYSTEM2_RESULT_READ_NOT_FINISHED;
        while (resultFlag == SYSTEM2_RESULT_READ_NOT_FINISHED)
        {
            resultFlag = System2ReadFromOutput(&commandInfo, outputBuffer, 1023, &bytesRead);
            outputBuffer[bytesRead] = 0;
            std::string outputPartial(outputBuffer, bytesRead);
            output += outputPartial;
        }

        if (printOutput)
        {
            std::cout << "# Solver's output:" << std::endl;
            std::cout << output;
        }

        bool isTimeout = output.find("s Segmentation fault") != std::string::npos;
        isTimeout = isTimeout || output.find("segmentation fault") != std::string::npos;
        isTimeout = isTimeout || output.find("Segmentation Fault") != std::string::npos;
        isTimeout = isTimeout || output.find("TIMEOUT") != std::string::npos;
        if (isTimeout)
        {
            result.isTimeout = true;
            return result;
        }

        if (output.find("s UNSATISFIABLE") != std::string::npos)
        {
            result.isIncoherent = true;
            return result;
        }

        size_t modelPosition = output.find("\nv ");

        if (modelPosition == std::string::npos)
        {
            throw std::runtime_error("MaxSAT solver's result has not been found.");
        }

        unsigned int variable = 1;
        std::stringstream out;
        out << "h";
        for (size_t i = modelPosition + 3; i < output.length() && (output[i] != '\n' || output[i] != '\r'); i++)
        {

            if (output[i] == '1')
            {
                // Creating a supporting model based on a solver's result.
                if (encoding.variablesToAtomsMapping.contains(variable))
                {
                    result.supportingModel.insert(encoding.variablesToAtomsMapping.at(variable));
                }
                out << " -" << variable++;
            }
            else if (output[i] == '0')
            {
                out << " " << variable++;
            }
        }

        out << " 0" << std::endl;
        result.exclusionClause = out.str();

        return result;
    }

    bool isAnswerSet(const Rules &rules, const Atoms &answerSetCandidate)
    {
        Rules positiveRules; // Reduct.
        bool toDelete;

        for (auto const rule : rules)
        {
            if (rule.isConstraint() == false)
            {
                Rule positiveRule;
                positiveRule.head = rule.head;
                toDelete = false;
                /**
                 * For each rule in a program:
                 * (a) remove the rule, if it contains a negative literal -B and B is in a model;
                 * (b) remove all remaining negative literals.
                 */
                for (const Literal literal : rule.body)
                {
                    if (literal > 0)
                    {
                        positiveRule.body.insert(literal);
                    }
                    else if (answerSetCandidate.contains(-literal))
                    {
                        toDelete = true;
                        break;
                    }
                }
                if (toDelete == false)
                {
                    positiveRules.insert(positiveRule);
                }
            }
            else
            {
                /**
                 * For each an (integrity) constraints, check if it is satisfied by a model.
                 * If any constraint is satisfied, then a model is not an answer set.
                 */
                bool isSatisfy = true;
                for (const Literal literal : rule.body)
                {
                    if (literal > 0 && answerSetCandidate.contains(literal) == false)
                    {
                        isSatisfy = false;
                        break;
                    }
                    else if (literal < 0 && answerSetCandidate.contains(-literal))
                    {
                        isSatisfy = false;
                        break;
                    }
                }
                if (isSatisfy)
                {
                    return false;
                }
            }
        }

        /**
         * Calculating a fixpoint.
         */
        bool isFixpoint = false;
        Atoms smallestFixpoint;

        while (isFixpoint == false)
        {
            Rules simplifiedPositiveRules;
            isFixpoint = true;
            for (auto const rule : positiveRules)
            {
                if (rule.isFact())
                {
                    smallestFixpoint.insert(rule.head);
                    isFixpoint = false;
                }
                else if (smallestFixpoint.contains(rule.head))
                {
                    isFixpoint = false;
                }
                else
                {
                    Rule simplifiedRule;
                    simplifiedRule.head = rule.head;
                    for (const Literal literal : rule.body)
                    {
                        if (smallestFixpoint.contains(literal) == false)
                        {
                            simplifiedRule.body.insert(literal);
                        }
                        else
                        {
                            isFixpoint = false;
                        }
                    }
                    simplifiedPositiveRules.insert(simplifiedRule);
                }
            }
            positiveRules = simplifiedPositiveRules;
        }

        return answerSetCandidate == smallestFixpoint;
    }
}