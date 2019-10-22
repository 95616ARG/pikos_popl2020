# PIKOS Artifact for POPL 2020

This is the software artifact that accompanies the paper "Deterministic Parallel Fixpoint Computation" by Sung Kook Kim, Arnaud J. Venet, and Aditya V. Thakur.

It mainly consists of our parallel abstract interpreter PIKOS, 4330 benchmarks used in the experiments, and scripts to reproduce the results in the paper.

PIKOS is based on a sequential abstract interpreter, [IKOS](https://github.com/NASA-SW-VnV/ikos).
Given a program as input, both IKOS and PIKOS compute invariants on each program point and run checks on them.
In fact, PIKOS is implemented as an analysis option of IKOS, allowing it to compute the invariants in parallel using multiple CPU cores.
Therefore, **PIKOS requires multi-core machines to reproduce the results.**
It requires at least **16 cores** to reproduce all of the results.
IKOS is chosen as the baseline.

After building and installing PIKOS, one can run PIKOS on each benchmark to get an analysis report,
compare the invariants computed by IKOS and PIKOS, or measure the speedup between IKOS and PIKOS.
Also, one can reproduce the data and generate tables and figures in the paper.

As the paper runs the experiments on 4330 benchmarks, cloud computing environment is recommended to reproduce the results in a timely manner.
Detailed AWS configurations and scripts used by the authors are provided in `aws/`.

## Installation

Benchexec, a framework used in this artifact for reliable benchmarking, is currently not supported under the Docker container.
Therefore, the installation has to be done locally without using Docker.

The reference environment uses **`Ubuntu 16.04`**.

An AMI is provided for AWS, so the installation may be skipped for it.
See `aws/README.md` if you prefer using AWS.

### Install Dependencies

The script `install_dependencies.sh` will install all required dependencies. It requires root access.

```
$ sudo ./install_dependencies.sh
```

### Build and Install PIKOS

The script `install.sh` will buid and install PIKOS in `./build/` directory. It does not require root access.

```
$ ./install.sh
```

### Extract Benchmarks

The script `extract_benchmarks.sh` will extract benchmarks in `./benchmarks/` directory. It does not require root access.

```
$ ./extract_benchmarks.sh
```

## General PIKOS Usage

The script `run_pikos.sh` runs the analysis on the given program.
It computes the invariants and runs checks on them.
This script is just to illustrate that PIKOS is fully functioning abstract interpreter,
and it is not used or required in reproducing the results.

```
$ ./run_pikos.sh ./benchmarks/test.c
```

If you see the following output as the result, installation was successful, and you can now
reproduce the results.
PIKOS reports two occurrences of buffer overflow at line 8 and 9.

```
[*] Compiling ./benchmarks/test.c
[*] Running ikos preprocessor
[*] Running ikos analyzer
[*] Translating LLVM bitcode to AR
[*] Running liveness analysis
[*] Running widening hint analysis
[*] Running interprocedural value analysis
[*] (Concurrently) Analyzing entry point 'main'
[*] Checking properties for entry point 'main'

# Time stats:
clang        : 0.056 sec
ikos-analyzer: 0.019 sec
ikos-pp      : 0.011 sec

# Summary:
Total number of checks                : 7
Total number of unreachable checks    : 0
Total number of safe checks           : 5
Total number of definite unsafe checks: 2
Total number of warnings              : 0

The program is definitely UNSAFE

# Results
benchmarks/test.c: In function 'main':
benchmarks/test.c:8:10: error: buffer overflow, trying to access index 10 of global variable 'a' of 10 elements
    a[i] = i;
         ^
benchmarks/test.c: In function 'main':
benchmarks/test.c:9:18: error: buffer overflow, trying to access index 10 of global variable 'a' of 10 elements
    printf("%i", a[i]);
                 ^
```

PIKOS inherits command line options of IKOS. Below highlights the relevant options used in this artifact.
Description of the numerical abstract domains are taken from the documentation of [IKOS](https://github.com/NASA-SW-VnV/ikos).

### Number of allowed threads

Using the `-nt` parameter restricts the number of allowed threads in PIKOS.
The following command will run PIKOS with maximum 4 threads:

```
$ ./run_pikos.sh -nt=4 ./benchmarks/test.c
```

`-nt=4` is the default.
`-nt=0` will let TBB library decide the number of threads.
Authors arbitrary chose 99 as the maximum number of threads.
To change this, modify line 991 of `./pikos/analyzer/src/ikos_analyzer.cpp` and install again.

### Context sensitivity

Using the `-cs` parameter restricts the context sensitivity in PIKOS.
This number corresponds to the allowed depth of function call inlining.
The following command will run PIKOS with maximum 10 depth of function calls:

```
$ ./run_pikos.sh -cs=10 ./benchmarks/test.c
```

`-cs=0` is the default, and it disables this restriction.
This restriction is used only for the benchmarks that take longer than 4 hours to analyze.
See `benchmarks/README.md` for more details.

### Numerical abstract domains

The list of available (thread-safe) numerical abstract domains are:

* `-d=interval`: The interval domain, see [CC77](https://www.di.ens.fr/~cousot/COUSOTpapers/publications.www/CousotCousot-POPL-77-ACM-p238--252-1977.pdf).
* `-d=congruence`: The congruence domain, see [Gra89](http://www.tandfonline.com/doi/abs/10.1080/00207168908803778).
* `-d=interval-congruence`: The reduced product of interval and congruence.
* `-d=dbm`: The Difference-Bound Matrices domain, see [PADO01](https://www-apr.lip6.fr/~mine/publi/article-mine-padoII.pdf).
* `-d=gauge`: The gauge domain, see [CAV12](https://ti.arc.nasa.gov/publications/4767/download/).
* `-d=gauge-interval-congruence`: The reduced product of gauge, interval and congruence.

By default, PIKOS uses the **interval** domain, and experiments were conducted with it.
If you want to try other domains, use the `-d` parameter:

```
$ ./run_pikos.sh -nt=4 -d=dbm ./benchmarks/test.c
```

### Disabling checks

Passing the options, `--no-checks --no-fixpoint-cache`, disables the checks -- such as buffer overflow analysis, division by zero analysis, null pointer analysis, etc -- after computing the invariants:

```
$ ./run_pikos.sh -nt=4 -d=dbm --no-checks --no-fixpoint-cache ./benchmarks/test.c
```

**These are the default options when timing the results**, as we are only concerned about the invariant computation time.

### Invariants comparison between IKOS and PIKOS

For `-nt` above 99, comparison between IKOS and PIKOS will be performed instead of the regular analysis.
For example, passing `-nt=104` will compute and compare the invariants of IKOS and PIKOS with maximum 4 threads.
If the invariants differ, "UNMATCH!!" will be printed and the process will return 42.
All runs using the above abstract domains should have the same invariants by design.

```
$ ./run_pikos.sh -nt=104 ./benchmarks/test.c
```

## Reproducing Paper Results

Reproducing the results are divided into steps: (1) reproducing the data and (2) generating tables/figures from the data.
The first step may take a lot of time if you choose to reproduce all of the data.
This step may be done using AWS to finish in a timely manner. See `aws/README.md` for more information.
The second step shouldn't take much time and can be done locally.
It still requires Benchexec to be installed locally though.
See `install_dependencies.sh`.

Again, PIKOS's speedup comes from utilizing multiple CPU cores. 
**PIKOS requires multi-core machines to reproduce the results.**
It requires at least **16 cores** to reproduce all of the results.
For the scripts with name of `reproduce*.sh`,
we will specify the least amount of cores required at the end of the names.

Also, the use of **TCMalloc**, which is a memory allocator installed in `install_dependencies.sh`,
is a must in reproducing the results.
Please make sure that it is installed properly. There should be a library, `/usr/local/lib/libtcmalloc.so`.

### Measuring speedup of a single benchmark

An optional script, `measure_speedup.sh`, is provided to measure the speedup of a single benchmark.
It takes the same command line options as `run_pikos.sh`.
It simply outputs the speedup, defined as the running time of IKOS / running time of PIKOS.

```
$ ./measure_speedup.sh -nt=4 ./benchmarks/OSS/audit-2.84/ausearch.bc
>>> Running time of IKOS  = 31.33911 seconds.
>>> Running time of PIKOS = 10.12769 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS) = 3.09x.
```

### Reproducing data


#### Using a local machine
#### Using AWS
#### Partial
#### Complete

### Generating tables and figures

The csv file produced above is used to generate tables and figures in the paper.

#### Fig. 6-(a)

The script `generate_fig6-a.py` generates the scatter plot in Fig. 6-(a).
It also outputs the means described in the evaluation section.
It requires the columns `walltime (s)`, `walltime (s)-4`, and `speedup-4` in
the csv file.
It outputs `fig6-a.png`.

```
$ ./generate_fig6-a.py ./results/base-nt4.csv
```

#### Fig. 6-(b)

The script `generate_fig6-b.py` generates the histograms in Fig. 6-(b).
It requires the columns `walltime (s)`, `walltime (s)-4`, and `speedup-4` in
the csv file.
It outputs 4 subfigures, `fig6-b-[0~3].png`.

```
$ ./generate_fig6-b.py ./results/base-nt4.csv
```

#### Table 3

The script `generate_tab3.py` generates the table 3.
It requires the columns `benchmark`, `category`, `walltime (s)`, `walltime (s)-4`,
and `speedup-4` in the csv file.
It outputs `tab3-speedup.csv` and `tab3-ikos.csv`, which are used to fill in the table 3.

```
$ ./generate_tab3.py ./results/base-nt4.csv
```

#### Table 4
