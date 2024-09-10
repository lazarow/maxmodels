#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstring>

#include "types.hpp"

#ifndef HELPERS_HPP
#define HELPERS_HPP
namespace maxmodels
{
    std::string as_string(const Rule &rule)
    {
        std::stringstream out;
        out << rule.head << " <- ";
        for (auto it = rule.body.begin(); it != rule.body.end(); ++it)
        {
            out << *it;
            if (std::next(it) != rule.body.end())
            {
                out << ", ";
            }
        }
        out << ".";
        return out.str();
    }

    std::string as_string(const Rule &rule, const Mapping &mapping)
    {
        std::stringstream out;
        if (rule.head == 1)
        {
            out << ":- ";
        }
        else if (mapping.contains(rule.head))
        {
            out << mapping.at(rule.head) << (rule.body.size() ? " :- " : "");
        }
        else
        {
            return "";
        }
        for (auto it = rule.body.begin(); it != rule.body.end(); ++it)
        {
            Literal literal = *it;
            if (literal < 0)
            {
                out << (mapping.contains(-literal) ? "not " + mapping.at(-literal) : "Unknown");
            }
            else
            {
                out << (mapping.contains(literal) ? mapping.at(literal) : "Unknown");
            }
            if (std::next(it) != rule.body.end())
            {
                out << ", ";
            }
        }
        out << ".";
        return out.str();
    }

    std::string as_string(const Rules &rules)
    {
        std::stringstream out;
        if (rules.size() == 0)
        {
            out << "No rules.";
        }
        for (auto it = rules.begin(); it != rules.end(); ++it)
        {
            out << as_string(*it);
            if (std::next(it) != rules.end())
            {
                out << std::endl;
            }
        }
        return out.str();
    }

    std::string as_string(const Rules &rules, const Mapping &mapping)
    {
        std::stringstream out;
        if (rules.size() == 0)
        {
            out << "No rules.";
        }
        for (auto it = rules.begin(); it != rules.end(); ++it)
        {
            std::string stringifiedRule = as_string(*it, mapping);
            if (stringifiedRule.empty())
            {
                continue;
            }
            out << stringifiedRule;
            if (std::next(it) != rules.end())
            {
                out << std::endl;
            }
        }
        return out.str();
    }

    std::string as_string(const Atoms &atoms)
    {
        std::stringstream out;
        out << "{";
        for (auto it = atoms.begin(); it != atoms.end(); ++it)
        {
            out << *it;
            if (std::next(it) != atoms.end())
            {
                out << ", ";
            }
        }
        out << "}";
        return out.str();
    }

    std::string as_string(const Atoms &atoms, const Mapping &mapping)
    {
        std::stringstream out;
        for (auto it = atoms.begin(); it != atoms.end(); ++it)
        {
            unsigned int atom = *it;
            if (mapping.contains(atom) == false)
            {
                continue;
            }
            out << mapping.at(atom);
            if (std::next(it) != atoms.end())
            {
                out << " ";
            }
        }
        return out.str();
    }

    std::string as_string(const Mapping &mapping)
    {
        std::stringstream out;
        out << "{";
        for (auto it = mapping.begin(); it != mapping.end(); ++it)
        {
            out << (*it).first << ": " << (*it).second;
            if (std::next(it) != mapping.end())
            {
                out << ", ";
            }
        }
        out << "}";
        return out.str();
    }

    std::string as_string(const Weights &weights)
    {
        std::stringstream out;
        out << "{";
        for (auto it = weights.begin(); it != weights.end(); ++it)
        {
            out << (*it).first << ": " << (*it).second;
            if (std::next(it) != weights.end())
            {
                out << ", ";
            }
        }
        out << "}";
        return out.str();
    }

    std::string as_string(const ChoiceRule &rule)
    {
        std::stringstream out;
        out << as_string(rule.heads) << " -> ";
        for (auto it = rule.body.begin(); it != rule.body.end(); ++it)
        {
            out << *it;
            if (std::next(it) != rule.body.end())
            {
                out << ", ";
            }
        }
        out << ".";
        return out.str();
    }

    char *as_char(const std::string &inputString)
    {
        char *charPtr = new char[inputString.length() + 1];
        strcpy(charPtr, inputString.c_str());
        return charPtr;
    }

    std::string get_file_contents(std::string filename)
    {
        std::ifstream file(filename);
        if (file.is_open() == false)
        {
            std::cerr << "The following file cannot be opened: " << filename << std::endl;
            throw std::runtime_error("The execution is stopped due to errors.");
        }
        std::stringstream ss;
        ss << file.rdbuf();
        std::string contents = ss.str();
        file.close();
        return contents;
    }
}
#endif
