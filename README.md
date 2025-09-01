# linsolve
Cli tool for solving linear programming models

## Note
This repo was migrated from another account of mine in [Codeberg](https://codeberg.org/libertymaxi/linsolve)(now archived).

## Compilation

```
make build
```

### Dependencies

This project only depends on [Tabulate](https://github.com/p-ranav/tabulate?tab=readme-ov-file). Its single header file version is already included in the repository.

This project was developed using g++ version 13. To ensure proper compilation, execute the following commands:

```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
```

```
sudo apt update
```

```
sudo apt install g++-13
```

```
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 20
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
* Cli basics(including the use of [Tabulate](https://github.com/p-ranav/tabulate?tab=readme-ov-file) for formatting output into tables, when appropriate)
* Optimizing memory usage(reducing copying) to increase performance

## Planned Features
* More branching/exploration strategies
* Advanced fathoming to reduce memory usage in large IP models
* PBP(Pure Binary programming) support
* MILP(Mixed Integer Linear Programming) support, in which any variable can be a real number(continuous), an integer, or binary
* Better syntax support(easier model comprehension)
* Support for more model file formats, similar to LPSolve
* Better feature documentation(in this README file or in a wiki)