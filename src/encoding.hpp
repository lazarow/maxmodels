#include <sstream>
#include <algorithm>
#include <utility>
#include <cmath>

#include "types.hpp"

namespace maxmodels
{
    /**
     *
     * a <- b, not c.
     * a <- c, d.
     *
     * b, not c <=> r1
     * (~r1 lub b) i (~r1 lub ~c)               b, not c <- r1
     * (r1 lub ~b lub c)                        b, not c -> r1
     * c, d <=> r2
     *
     * ~a lub r1 lub r2
     *
     * Soft:
     * 1 -r1 0
     *
     */
    EncodedProgram encode(const LogicProgram &program, const SimplifiedProgram &simplifiedProgram)
    {
        // Auxilary variables.
        unsigned int variable = 1;
        std::stringstream out;
        Atom atom;
        Rules rulesWithVariables;
        std::map<unsigned int, std::set<unsigned int>> bodies;
        unsigned int nofRulesWithBody = 0;

        // Output variable.
        EncodedProgram encodedProgram;
        encodedProgram.atomsToVariablesMapping.insert(std::make_pair(CONSTRAINT_HEAD, 0));

        // Step 1: Creating rules with variables, which are reindexed atoms.
        for (const Rule &rule : simplifiedProgram.rules)
        {
            Rule ruleWithVariables;

            if (encodedProgram.atomsToVariablesMapping.contains(rule.head) == false)
            {
                encodedProgram.atomsToVariablesMapping.insert(std::make_pair(rule.head, variable));
                encodedProgram.variablesToAtomsMapping.insert(std::make_pair(variable, rule.head));
                variable++;
            }
            ruleWithVariables.head = encodedProgram.atomsToVariablesMapping.at(rule.head);

            for (const Literal &literal : rule.body)
            {
                atom = literal < 0 ? -literal : literal;
                if (encodedProgram.atomsToVariablesMapping.contains(atom) == false)
                {
                    encodedProgram.atomsToVariablesMapping.insert(std::make_pair(atom, variable));
                    encodedProgram.variablesToAtomsMapping.insert(std::make_pair(variable, atom));
                    variable++;
                }
                ruleWithVariables.body.insert((literal < 0 ? -1 : 1) * encodedProgram.atomsToVariablesMapping.at(atom));
            }

            rulesWithVariables.insert(ruleWithVariables);
        }

        // Step 2: Encoding rules into clauses.
        for (const Rule &rule : rulesWithVariables)
        {
            if (rule.head == 0)
            {
                out << "h";
                for (const Literal &literal : rule.body)
                {
                    out << " " << -literal;
                }
                out << " 0" << std::endl;
            }
            else if (rule.head > 1 && rule.body.size() == 0)
            {
                out << "h " << rule.head << " 0" << std::endl;
            }
            else
            {
                bodies[rule.head].insert(variable);
                out << "h " << rule.head << " -" << variable << " 0" << std::endl;
                out << "1 -" << variable << " 0" << std::endl;
                nofRulesWithBody++;
                for (const Literal &literal : rule.body)
                {
                    out << "h -" << variable << " " << literal << " 0" << std::endl;
                }
                out << "h " << variable;
                for (const Literal &literal : rule.body)
                {
                    out << " " << -literal;
                }
                out << " 0" << std::endl;
                variable++;
            }
        }
        for (auto it = bodies.begin(); it != bodies.end(); it++)
        {
            out << "h -" << (it->first);
            for (auto const &bodyVariable : it->second)
            {
                out << " " << bodyVariable;
            }
            out << " 0" << std::endl;
        }

        // Step 3: Additional clauses for weights (optimization).
        if (program.weights.size() > 0)
        {
            unsigned int augment = std::pow(10, std::ceil(std::log10(nofRulesWithBody)));
            for (auto const &[literal, weight] : program.weights)
            {
                atom = literal < 0 ? -literal : literal;
                // Add a weight if only there is an atom in clauses.
                if (encodedProgram.atomsToVariablesMapping.contains(atom))
                {
                    out << (augment * weight) << " ";
                    out << (literal < 0 ? "" : "-") << encodedProgram.atomsToVariablesMapping.at(atom);
                    out << " 0" << std::endl;
                }
            }
        }

        // Step 4: Additional clauses the founded set.
        if (program.founded.size() > 0)
        {
            for (auto const &atom : program.founded)
            {
                if (encodedProgram.atomsToVariablesMapping.contains(atom))
                {
                    out << "h " << encodedProgram.atomsToVariablesMapping.at(atom) << " 0" << std::endl;
                }
            }
        }

        // Step 5: Additional clauses the unfounded set.
        if (program.unfounded.size() > 0)
        {
            for (auto const &atom : program.unfounded)
            {
                if (encodedProgram.atomsToVariablesMapping.contains(atom))
                {
                    out << "h -" << encodedProgram.atomsToVariablesMapping.at(atom) << " 0" << std::endl;
                }
            }
        }

        encodedProgram.wcnf = out.str();

        return encodedProgram;
    }
}