# eqv_pred

# eqv

The folder `/eqv` contains the algorithm used to compute the equivalence classes.

On debian run:
```
sudo apt instal libz3=dev make g++
cd eqv
make -j2
```

## Usage

```./eqv <path to cnf file>```

The executable will create a new file `path to cnf file.eqv` which will be the new formula
with the propagated equalities. The executable will also output to stdout
which variables are in which equivalence classes.

# wrapper

The folder `/wrapper` contains the code used to monitor both time and memory usage
of a process.

On debian run:
```
cd wrapper
make
```
