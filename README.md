# eqv_pred

# eqv

The folder `/eqv` contains the algorithm used to compute the equivalence classes.

On debian run:
```
sudo apt instal libz3=dev make g++
cd eqv
make -j2
```

To build a singularity container use:
```make singularity```

## Usage

```./eqv <path to cnf file>```

The executable will create a new file `path to cnf file.eqv` which will be the new formula
with the propagated equalities. The executable will also output to stdout
which variables are in which equivalence classes.

# wrapper

The folder `/wrapper` contains the code used to monitor both time and memory usage
of a process. As the wrapper reads files in the `/proc` folder of the system
it will only work on UNIX-like systems and it has only been tested on GNU/Linux.
Maybe it could still run on other systems inside the singularity containers
that are provided but this has not been tested.

On debian run:
```
cd wrapper
make
```

## usage

```wrap <max memory in MB> <max time in seconds> <command with full path to executable>```

The wrapper will output to stderr a lne containing the command, the state,
the maximum amount of virtual memory used in KB and the time used in seconds.
If the time or memory exeed the given parameters, then the program will be killed.

# singularity

The folder `/singularity` contains the scripts to create the singularity files.
These files can be used to run the samplers and model counters with our wrapper.
The files already contain the part mentioning the wrapper (limiting the sampler
to 64GB of RAM and 5 hours of computation), they can thus be used like the
samplers without the wrapper.

# Benchmarks

The file `Benchmarks.tar.gz` contains the dimacs files on which we conducted our experiments.
The archive also contains the MIS of the formulas that were computed with a timeout
of 5 hours and the equivalent formula which got simplified by propagating the equivalence
classes.
