# Benchmarks

The Benchmark files (DIMACS) are available at https://github.com/diverse-project/samplingfm

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

# Replication

To replicate our experiments, multiple csv files have to be generated.
The Benchmarks archive has to be extracted.

The easiest part is to run the samplers on the formulas. To achieve this,
one can build and use the singularity containers with the given singularity scripts.
The wrapper will be in the container (and needs to be compiled before).
The wrapper will output the csv line to stderr in the format "cmd, state, memory, time".
The cmd column needs to be modified so that it contains the file path.
The file paths are used as keys in the python scripts and need to be the same
in all the csv files.

Then one needs to compute the eqv.csv file (the file containing the number
of equivalence classes for every formula) using eqv. `mis.csv` contains the sizes
of the mis and can be computed with `https://github.com/meelgroup/mis`.

The tree width can be computed by using the `cnf_to_graph` executable
and a program called flow_cutter_pace17 (`https://github.com/kit-algo/flow-cutter-pace17`).
Both need to be in the path so that the script `tw.sh` can use them.

The deficiency can be computed using the `deff.sh` script which uses lp_solve
to compute the true defficiency. It also uses `cnf_lp_deff` which is contained
in the `cnf_to_graph` project.

The script `nb_clauses.sh` creates a simple csv file with the basic statistics of
the formulas and the script `minisat_test.sh` is a helper script which
needs to be modified to run either z3 or minisat.

Once all of the csv files have been generated with the right format,
then the python scripts can be used. The script `correlation.py` shows
the correlation matrices. The script `gen_rf_100.py` generates the models
which are then converted to tables using the `get_scores.py` script.
The `models.py` script generates models which are converted to
tables using the `cmp_models.py` script.
The script `fi.py` generates the tables containing the feature importances
in a random forest.
If some csv files were regenerated, then the python scripts might need
to be modified as the required csv files are hardcoded in the scripts.
While these scripts can be cumbersome to use (because of the restrictive file formats),
this restriction also allows us to run them without parameters.
