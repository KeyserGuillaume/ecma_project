# [MPRO] Ecma Project
A school project in which we solve a robust stochastic problem with several methods

## Build

- **Debug mode**, inside `Debug` directory
```sh
cmake -DCPLEX_ROOT_DIR="/opt/ibm/ILOG/CPLEX_Studio129/" -DCMAKE_BUILD_TYPE=Debug ..
```

- **Release mode**, inside `Debug` directory
```sh
cmake -DCPLEX_ROOT_DIR="/opt/ibm/ILOG/CPLEX_Studio129/" -DCMAKE_BUILD_TYPE=Release ..
```

## Usage

```sh
./ecma <method> ../data/10_ulysses_9.ts
```

where method is:
* `DUAL`
* `CUTTING_PLANES`
* `HEURISTIC`
* `BRANCH_AND_CUT`
