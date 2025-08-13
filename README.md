# linsolve
Cli tool for solving linear programming models

## Features
* Simplex method for LP problems
* Branch and Bound method for pure IP problems
    * Exploration strategies:
        * EXPLORE_ALL_NODES
        * BEST_OBJECTIVE_FUNCTION_VALUE
        * RANDOM
    * Fathoming of nodes with continuous solutions worse than current incumbent solution
    * Performance metrics(explored nodes and execution time)

## Currently working on
* 2 new Exploration strategies: WIDTH and DEPTH

## Coming soon

