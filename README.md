# linsolve
Command Line tool for solving linear programming models

## Note
This repo was migrated from another account of mine in [Codeberg](https://codeberg.org/libertymaxi/linsolve)(now archived).

## Compilation

```
make build OPTIM_LEVEL=2
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
* Simplex method as base solver
* Branch and Bound method for Integer Programming problems

## Currently working on
* MILP(Mixed Integer Linear Programming) support, in which any variable can be a real number(continuous), an integer, or binary

## Planned Features
* Support for more model file formats, similar to LPSolve
* Multi-Objective Linear Programming
* Non-Linear Programming