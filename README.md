# linsolve
Cli tool for solving linear programming models

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
* Fixing occasional segmentation faults
* Better code documentation

## Coming soon
* More branching straegies