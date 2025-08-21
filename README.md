# linsolve
Cli tool for solving linear programming models

## Note
This repo was migrated from a another account of mine in [Codeberg](https://codeberg.org/libertymaxi/linsolve)(now archived).

## Compilation



```
make build
```

## Installation

To install linsolve, simply execute the command below.

```
make install
```

By default, this command will copy the binary to a directory which most systems include in their PATH, making it accessible from anywhere.

## Usage

## Features
* Simplex method for LP problems
* Branch and Bound method for pure IP problems
    * Exploration strategies:
        * EXPLORE_ALL_NODES
        * BEST_OBJECTIVE_FUNCTION_VALUE
        * RANDOM
        * WIDTH
        * DEPTH
    * Branching strategies
        * FIRST_INDEX
        * RANDOM_VAR
        * BEST_COEFFICIENT
    * Fathoming of nodes with continuous solutions worse than current incumbent solution
    * Performance metrics(explored nodes and execution time)

## Currently working on
* Fixing documentation
* Changing methods visibility(only members that will stay public are those that are accessed outside the class)
* Adding missing error handling and making all methods more error resistant in general

## Planned Features
* Benchmarking code to compare performance of different combinations of exploration and branching strategies
* More branching/exploration strategies
* Cli basics(including the use of [Tabulate](https://github.com/p-ranav/tabulate?tab=readme-ov-file) for formatting output into tables, when appropriate)
* Advanced fathoming to reduce memory usage in large IP models
* PBP(Pure Binary programming) support
* MILP(Mixed Integer Linear Programming) support, in which any variable can be a real number(continuous), an integer, or binary
* Better syntax support(easier model comprehension)
* Support for more model file formats, similar to LPSolve
* Better feature documentation(in this README file or in a wiki)