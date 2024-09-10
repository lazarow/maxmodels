# maxmodels

A non-native solver for [Answer Set Programming](https://en.wikipedia.org/wiki/Answer_set_programming).
It reads a logic program, i.e. a set of logical rules, and computes the corresponding an answer set.

## Description

The solver uses the translation of a logic program into MaxSAT's clauses in order to speed up
the optimization. It uses Gringo as a grounder.

## Getting Started

### Dependencies

-   Tools: `make`, `curl` and `g++`.
-   _Gringo_ (a grounder from Potassco).
-   MaxSAT solvers: [Open-WBO and WMaxCDCL](https://maxsat-evaluations.github.io/2023/descriptions.html).

### Installing

-   Run `make install` to download all dependencies.
-   Run `make build` to compile the solver.
-   Create the `.env` file with MaxSAT solvers' paths, i.e. Open-WBO and WMaxCDCL.

### Executing program

```
gringo --output=smodels --warn=none <input_model> | ./bin/maxmodels -e <path to .env file>
```

## Authors

Contributors names and contact info:

-   [Arkadiusz Nowakowski](https://ab.us.edu.pl/emp?id=46971),
-   [Wojciech Wieczorek](https://kiia.ubb.edu.pl/pracownicy/dr-habwojciechwieczorek),
-   [Łukasz Strąk](https://ab.us.edu.pl/emp?id=47011).

## License

This project is licensed under the Apache License 2.0 - see the LICENSE.md file for details
