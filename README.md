# maxmodels

A non-native solver for Answer Set Programming.

## Description

The solver uses the translation of a logic program into MaxSAT's clauses in order to speed up
the optimization compared with the state-of-the-art solvers like Clingo or DLV. It uses Gringo as a grounder.

## Getting Started

### Dependencies

-   Tools: `make`, `curl` and `g++`.
-   _Gringo_ (a grounder from Potassco).
-   MaxSAT solver compatible with the standard WCNF format (a new format after 2022, see [this](https://maxsat-evaluations.github.io/2022/rules.html#input) for more details).

### Installing

-   Run `make install` to download all dependencies.
-   Run `make build` to compile the solver.
-   Change a MaxSAT solver path in the configuration file `.env`.

### Executing program

```
gringo --output=smodels --warn=none <input_model> | ./bin/maxmodels
```

## Authors

Contributors names and contact info:

-   [Arkadiusz Nowakowski](https://ab.us.edu.pl/emp?id=46971),
-   [Wojciech Wieczorek](https://kiia.ubb.edu.pl/pracownicy/dr-habwojciechwieczorek),
-   [Łukasz Strąk](https://ab.us.edu.pl/emp?id=47011).

## License

This project is licensed under the Apache License 2.0 - see the LICENSE.md file for details
