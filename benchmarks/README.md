# Benchmarks

Total of 4330 benchmarks were used in the paper.
Benchmarks come from the following two sources:

1. [SV-COMP 2019](https://github.com/sosy-lab/sv-benchmarks) (SVC) : 2701 benchmarks.
2. [Arch Linux core packages](https://wiki.archlinux.org/index.php/Official_repositories#core) (OSS) : 1629 benchmarks.

Full list of benchmarks are in `full.set`.
11 benchmarks in OSS had errors while running.
These are in `err.set` and are excluded from the experiments.

## Benchmarks that take more than or equal to 4 hours in IKOS

For benchmarks taking more than or equal to 4 hours, context sensitivity is adjusted in
PIKOS to make the analysis finish in 4 hours.
These benchmarks are listed in `me4hours.set`.
The script `find_context_sensitivity.sh` searches for the optimal context
sensitivity for each benchmark in this list and outputs to `context_sensitivity.csv`.
The numbers computed by the authors are in `../results/` directory.
The optimal number may differ among the environments.
The global variable `NUMTHREADS` decides how many threads to use for the search
and should be tuned to fit the user's environment.

## Description of sets
- `full.set` : full set of benchmarks.
- `err.set` : the set of benchmarks with errors.
- `lt4hours.set` : the set of benchmarks that takes less than 4 hours in IKOS.
- `me4hours.set` : the set of benchmarks that takes more than or equal to 4 hours in IKOS.
- `tab3.set` : the set of benchmarks in Table 3.
- `tab4.set` : the set of benchmarks in Table 4.
