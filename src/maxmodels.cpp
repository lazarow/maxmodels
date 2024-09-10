#include <utility>

#include "configuration.hpp"
#include "helpers.hpp"
#include "reader.hpp"
#include "simplification.hpp"
#include "encoding.hpp"
#include "solving.hpp"

using namespace std;
using namespace maxmodels;

int main(int argc, char *argv[])
{
    try
    {
        cout << "% maxmodels 1.0.0" << endl;
        cout << "%" << endl;

        cout << "% Reading configuration ..." << endl;
        Configuration configuration = getConfiguration(argc, argv);

        cout << "% Reading the input program ..." << endl;
        ostringstream ss;
        ss << cin.rdbuf();
        LogicProgram program = readFromSmodelsFormat(ss.str());
        if (configuration.printRules)
        {
            cout << "% Rules:" << endl;
            cout << (configuration.useMapping ? as_string(program.rules, program.mapping) : as_string(program.rules)) << endl;
        }
        if (configuration.printLabels)
        {
            cout << "% Labels:" << endl;
            cout << as_string(program.mapping) << endl;
        }

        cout << "% Simplifying ..." << endl;
        SimplifiedProgram simplifiedProgram = simplify(program);
        if (simplifiedProgram.isIncoherent)
        {
            cout << "INCONSISTENT" << endl;
            return 20;
        }
        if (configuration.printSimplifiedRules)
        {
            cout << "% Simplified rules:" << endl;
            cout << (configuration.useMapping ? as_string(simplifiedProgram.rules, program.mapping) : as_string(simplifiedProgram.rules)) << endl;
            cout << "% Facts:" << endl;
            cout << (configuration.useMapping ? as_string(simplifiedProgram.facts, program.mapping) : as_string(simplifiedProgram.facts)) << endl;
        }
        cout << "% Simplification ratio: " << simplifiedProgram.simplificationRatio << endl;

        cout << "% Encoding the program into MaxSAT clauses ..." << endl;
        EncodedProgram encoding = encode(program, simplifiedProgram);
        if (configuration.printClauses)
        {
            cout << "% wcnf:" << endl;
            cout << encoding.wcnf;
        }

        if (configuration.isDebug)
        {
            return 1;
        }

        cout << "% Solving ..." << endl;
        if (program.weights.size() > 0)
        {
            cout << "% There is the optimization rule. WMaxCDCL will be used ..." << endl;
        }
        else
        {
            cout << "% There is not an optimization rule. Open-WBO will be used ..." << endl;
        }
        unsigned int iteration = 0;
        unsigned int openWBOTimeLimitInSeconds = 1;
        bool isOptimization = program.weights.size() > 0;
        while (true)
        {
            iteration++;
            cout << "% Iteration " << iteration << endl;

            std::string solverCommand;
            if (isOptimization)
            {
                solverCommand = configuration.wMaxCDCLPath;
            }
            else
            {
                solverCommand = configuration.openWBOPath;
                solverCommand += " -cpu-lim=" + to_string(openWBOTimeLimitInSeconds);
            }
            SolvingOutcome solvingResult = solve(
                encoding,
                solverCommand,
                configuration.printSolverOutput);

            if (solvingResult.isTimeout)
            {
                openWBOTimeLimitInSeconds *= 2;
                cout << "% Current timeout for OpenWBO:" << openWBOTimeLimitInSeconds << " s." << endl;
                continue;
            }

            if (solvingResult.isError)
            {
                throw runtime_error(solvingResult.errorMessage);
            }

            if (solvingResult.isIncoherent)
            {
                cout << "INCONSISTENT" << endl;
                return 20;
            }

            Atoms answerSetCandidate;
            answerSetCandidate.insert(simplifiedProgram.facts.begin(), simplifiedProgram.facts.end());
            answerSetCandidate.insert(solvingResult.supportingModel.begin(), solvingResult.supportingModel.end());

            if (isAnswerSet(program.rules, answerSetCandidate))
            {
                cout << "ANSWER" << endl;
                cout << as_string(answerSetCandidate, program.mapping) << endl;
                if (isOptimization)
                {
                    Atom atom;
                    int cost = 0, totalWeight = 0;
                    for (auto const &[literal, weight] : program.weights)
                    {
                        if (literal < 0 && answerSetCandidate.contains(-literal) == false)
                        {
                            cost += weight;
                        }
                        if (literal > 0 && answerSetCandidate.contains(literal))
                        {
                            cost += weight;
                        }
                        totalWeight += weight;
                    }
                    cout << "COST " << cost << endl;
                    cout << "OPTIMUM" << endl;
                    cout << "% Optimization: " << (totalWeight - cost) << endl;
                }
                return 10;
            }
            else
            {
                if (isOptimization)
                {
                    Atom atom;
                    int cost = 0, totalWeight = 0;
                    for (auto const &[literal, weight] : program.weights)
                    {
                        if (literal < 0 && answerSetCandidate.contains(-literal) == false)
                        {
                            cost += weight;
                        }
                        if (literal > 0 && answerSetCandidate.contains(literal))
                        {
                            cost += weight;
                        }
                        totalWeight += weight;
                    }
                    cout << "% Optimization: " << (totalWeight - cost) << endl;
                }
                encoding.wcnf += solvingResult.exclusionClause;
            }

            if (iteration > configuration.solverIterationLimit)
            {
                throw runtime_error("Solver's iteration limit has been reached.");
            }
        }
    }
    catch (const runtime_error &error)
    {
        cerr << "% " << error.what() << endl;
        return 1;
    }

    return 1;
}