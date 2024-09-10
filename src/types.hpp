#include <set>
#include <map>
#include <string>
#include <initializer_list>

#ifndef TYPES_HPP
#define TYPES_HPP
namespace maxmodels
{
    typedef unsigned int Atom;
    typedef std::set<Atom> Atoms;
    typedef Atom Head;
    typedef std::set<Head> Heads;

    const Atom CONSTRAINT_HEAD = 1;

    typedef int Literal;
    typedef std::set<Literal> Body;

    struct Rule
    {
        Head head;
        Body body;
        bool operator==(const Rule &rhs) const
        {
            return head == rhs.head && body == rhs.body;
        }
        bool operator<(const Rule &rhs) const
        {
            return head == rhs.head
                       ? body < rhs.body
                       : head < rhs.head;
        }
        Rule()
        {
        }
        Rule(std::initializer_list<int> const &literals)
        {
            for (auto it = literals.begin(); it != literals.end(); ++it)
            {
                if (it == literals.begin())
                {
                    head = *it;
                    continue;
                }
                body.insert(*it);
            }
        }
        bool isConstraint() const
        {
            return head == CONSTRAINT_HEAD;
        }
        bool isFact() const
        {
            return head != CONSTRAINT_HEAD && body.empty();
        }
    };
    typedef std::set<Rule> Rules;

    struct CardinalityRule
    {
        Head head;
        Body body;
        unsigned int bound;
    };

    struct ChoiceRule
    {
        Heads heads;
        Body body;
    };

    typedef std::string Label;
    typedef std::map<Atom, Label> Mapping;

    typedef std::map<Literal, unsigned int> Weights;

    typedef std::string WCNF;
    typedef std::map<Atom, unsigned int> AtomsToVariablesMapping;
    typedef std::map<unsigned int, Atom> VariablesToAtomsMapping;

    struct LogicProgram
    {
        Rules rules;
        Mapping mapping;
        Weights weights;
        Atom largest = 0;
    };

    struct SimplifiedProgram
    {
        Atoms facts;
        Rules rules;
        double simplificationRatio = 0;
        bool isIncoherent = false;
    };

    struct EncodedProgram
    {
        WCNF wcnf;
        AtomsToVariablesMapping atomsToVariablesMapping;
        VariablesToAtomsMapping variablesToAtomsMapping;
    };
}
#endif
