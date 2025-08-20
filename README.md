# linsolve
Cli tool for solving linear programming models

## Note
This repo was migrated from a another account of mine in [Codeberg](https://codeberg.org/libertymaxi/linsolve)(now archived).

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

## Coming soon
* More branching straegies