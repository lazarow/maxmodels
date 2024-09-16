# maxmodels

A non-native solver for [Answer Set Programming](https://en.wikipedia.org/wiki/Answer_set_programming).
It reads a logic program, i.e. a set of logical rules, and computes the corresponding an answer set.

## Description

The solver uses the translation of a logic program into MaxSAT's clauses to speed up
the optimization. It admits only normal (basic) rules, integrity constraints, and weights, and therefore
additional tools must be used to handle more complex logic programs:
a grounder that supports the intermediate file format of _lparse/smodels_
and a normalizer to replace extended rules.

As a grounder, we recommend [_gringo_](https://potassco.org/clingo/) and as a normalizer [_lp2normal_](https://research.ics.aalto.fi/software/asp/lp2normal/).

### Note

_maxmodels_ is designed with optimization in mind, therefore for decision and query problems,
we recommend other systems like Clingo and DLV.

## Getting Started

### Requirements

-   The Linux operating system.
-   Tools: `make`, `curl` and `g++`.
-   _gringo_: a grounder from Potassco (or other compatible).
-   _lp2normal_: a normalizer (or other compatible).
-   The MaxSAT solver: [WMaxCDCL](https://maxsat-evaluations.github.io/2023/descriptions.html).

### Installing

-   Run `make install` to download all dependencies.
-   Run `make build` to compile the solver.
-   Create the `.env` file.

### Executing program

```
gringo --output=smodels <input_model> | lp2normal-2.27 | ./bin/maxmodels -e <path to .env file>
```

The output of the solver is (mostly) compatible with [the ASP competition standard](https://www.mat.unical.it/aspcomp2013/files/aspoutput.txt).

## Authors

Contributors names and contact info:

-   [Arkadiusz Nowakowski](https://ab.us.edu.pl/emp?id=46971),
-   [Wojciech Wieczorek](https://kiia.ubb.edu.pl/pracownicy/dr-habwojciechwieczorek),
-   [Łukasz Strąk](https://ab.us.edu.pl/emp?id=47011).

## License

This project is licensed under the Apache License 2.0 - see the LICENSE.md file for details
