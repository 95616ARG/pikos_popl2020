# PIKOS Artifact for POPL 2020

This is the software artifact that accompanies the paper "Deterministic Parallel Fixpoint Computation" by Sung Kook Kim, Arnaud J. Venet, and Aditya V. Thakur.


**"Deterministic Parallel Fixpoint Computation"**
Preprint: [http://arxiv.org/abs/1909.05951](http://arxiv.org/abs/1909.05951).
```
@article{DBLP:conf/popl/KimVT20,                                                     
  author    = {Sung Kook Kim and                                            
               Arnaud J. Venet and                                                   
               Aditya V. Thakur},                                                    
  title     = {Deterministic Parallel Fixpoint Computation},                         
  journal   = {{PACMPL}},                                                            
  volume    = {4},                                                                   
  number    = {{POPL}},                                                              
  pages     = {14:1--14:33},                                                         
  year      = {2020},                                                                
  note      = {To appear},                                                           
  url       = {https://doi.org/10.1145/3371082},                                     
  doi       = {10.1145/3371082},                                                     
}
```

```
@article{DBLP:journals/corr/abs-1909-05951,
  author    = {Sung Kook Kim and
               Arnaud J. Venet and
               Aditya V. Thakur},
  title     = {Deterministic Parallel Fixpoint Computation},
  journal   = {CoRR},
  volume    = {abs/1909.05951},
  year      = {2019},
  url       = {http://arxiv.org/abs/1909.05951},
  archivePrefix = {arXiv},
  eprint    = {1909.05951},
}
```

It mainly consists of our parallel abstract interpreter PIKOS, 4330 benchmarks used in the experiments, and scripts to reproduce the results in the paper.

PIKOS is based on a sequential abstract interpreter, [IKOS](https://github.com/NASA-SW-VnV/ikos).
The files changed in IKOS to implement PIKOS are summarized in [`CHANGES.md`](CHANGES.md).
Given a program as input, both IKOS and PIKOS compute invariants on each program point and run checks on them.
In fact, PIKOS is implemented as an analysis option of IKOS, allowing it to compute the invariants in parallel using multiple CPU cores.
Therefore, **PIKOS requires multi-core machines to reproduce the results.**
It requires at least **16 cores** to reproduce all of the results.
IKOS is chosen as the baseline.

After building and installing PIKOS, one can run PIKOS on each benchmark to get an analysis report,
compare the invariants computed by IKOS and PIKOS, or measure the speedup between IKOS and PIKOS.
Also, one can reproduce the data and generate tables and figures in the paper.

As the paper runs the experiments on 4330 benchmarks, cloud computing environment is recommended to reproduce the results in a timely manner.
Detailed AWS configurations and scripts used by the authors are provided in [`aws/`](aws).

## Installation

The reference environment uses **`Ubuntu 16.04`**.

### Docker

Skip the installation section if you are using the docker.
One needs `docker` installed.
The image is availabe in the DockerHub repository `skkeem/pikos:dev`.
The following command will download the image and run it interactivley:
```
$ docker run --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw -w /pikos_popl2020 -it --privileged skkeem/pikos:dev
```
sha256:3d99811735e0e3577e7eea90785323d1975ac6250939ba4f70e6695ebedf5520

One can also build the image using the Dockerfile in this repo.
```
$ docker build -t skkeem/pikos:dev .
```

### Install Dependencies

The script `install_dependencies.sh` will install all required dependencies. It requires root access.

```
$ sudo ./install_dependencies.sh
$ sudo usermod -aG benchexec $USER
$ ./install_python_dependencies.sh
```

Dependencies:
- LLVM and Clang 8.0.x
- A C++ compiler that supports C++17 (gcc >= 8.3.0)
- CMake >= 3.4.3
- GMP >= 4.3.1
- Python 3 >= 3.5.3
- SQLite >= 3.6.20
- Boost >= 1.58, <= 1.69
- BenchExec >= 1.18
- TBB >= 2
- TCMalloc >= 4.5.3
- texlive texlive-latex-extra dvipng
- bc

Python3.6 Dependencies:
- pandas
- matplotlib
- scipy

### Build and Install PIKOS

The script `install.sh` will buid and install PIKOS in `./build/` directory. It does not require root access.

```
$ ./install.sh
```

### Extract Benchmarks

The script `extract_benchmarks.sh` will extract benchmarks in [`./benchmarks/`](benchmarks) directory. It does not require root access.
The sha256sum of the downloaded file is d4f355097133e1b32135d9fd530b33b02ea11536fd930c6fc3ee9266f6b1b1c1.

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
$ ./run_pikos.sh -nt=4 ./benchmarks/OSS/audit-2.8.4/ausearch.bc
```

`-nt=4` is the default.
`-nt=0` will let TBB library decide the number of threads.
Authors arbitrary chose 99 as the maximum number of threads.
To change this, modify line 991 of [`./pikos/analyzer/src/ikos_analyzer.cpp`](pikos/analyzer/src/ikos_analyzer.cpp) and install again.

### Context sensitivity

Using the `-cs` parameter restricts the context sensitivity in PIKOS.
This number corresponds to the allowed depth of dynamic inlining in the interprocedural analysis.
The following command will run PIKOS with maximum 5 depth of function calls:

```
$ ./run_pikos.sh -cs=5 ./benchmarks/OSS/audit-2.8.4/ausearch.bc
```

`-cs=0` is the default, and it disables this restriction.
This restriction is used only for the benchmarks that take longer than 4 hours to analyze.
See [`benchmarks/README.md`](benchmarks/README.md) for more details.

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
$ ./run_pikos.sh -nt=4 -d=dbm ./benchmarks/OSS/audit-2.8.4/ausearch.bc
```

### Disabling checks

Passing the options, `--no-checks --no-fixpoint-cache`, disables the checks -- such as buffer overflow analysis, division by zero analysis, null pointer analysis, etc -- after computing the invariants:

```
$ ./run_pikos.sh -nt=4 -d=dbm --no-checks --no-fixpoint-cache ./benchmarks/OSS/audit-2.8.4/ausearch.bc
```

**These are passed when timing the results**, as we are only concerned about the invariant computation time.

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
This step may be done using AWS to finish in a timely manner. See [`aws/README.md`](aws/README.md) for more information.
The second step shouldn't take much time and can be done locally.
It still requires the dependencies to be installed locally.
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
$ ./measure_speedup.sh -nt=4 ./benchmarks/OSS/audit-2.8.4/ausearch.bc
>>> Running time of IKOS  = 31.33911 seconds.
>>> Running time of PIKOS = 10.12769 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS) = 3.09x.
```

### Reproducing data

The following scripts generate data in a csv file.
It has the following columns:
- `benchmark`: Name of the benchmark.
- `category`: The source of the benchmark. SVC or OSS. 
- `cs`: The context sensitivity used.
- `walltime (s)`: Analysis time in IKOS
- `walltime (s)-k`: Analysis time in PIKOS<k>, where k is the number of threads allowed.
- `speedup-k`: Speedup of PIKOS<k>

Ignore warnings about No propertyfile, variable replacement, and file name appearing twice
when running the scripts.

#### Reproduce all

It runs IKOS, PIKOS<2>, PIKOS<4>, PIKOS<6>, and PIKOS<8> on all of the benchmarks.
It outputs `all.csv`.
This takes a lot of time (roughly 48 days, if using only a single machine).

```
$ ./reproduce_all-8.sh
```

#### Reproduce rq1

It runs IKOS and PIKOS<4> on all of the benchmarks.
It outputs `rq1.csv`.
This can be used to answer RQ1.
This takes a lot of time too (roughly 25 days, if using only a single machine).

```
$ ./reproduce_rq1-4.sh
```

#### Reproduce tab3

It runs IKOS and PIKOS<4> on benchmarks in table 3.
It outputs `tab2.csv`.
This can be used to generate table 2.
This takes roughly 36 hours, if using only a single machine.

```
$ ./reproduce_tab2-4.sh
```

Alternatively, one can choose to run only the upper part of table 2.
It outputs `tab2a.csv`.
This takes roughly 8 hours, if using only a single machine.

```
$ ./reproduce_tab2a-4.sh
```

The following command measures the speedup for the benchmark with highest
speedup in PIKOS<4>.
The result for this benchmark can be found in the first entry of the table 2.

```
$ ./measure_speedup.sh -nt=4 ./benchmarks/OSS/audit-2.8.4/aureport.bc
>>> Running time of IKOS  = 684.19316 seconds.
>>> Running time of PIKOS = 188.25443 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS) = 3.63x.
```

#### Reproduce tab3

It runs IKOS, PIKOS<4>, PIKOS<8>, PIKOS<12>, and PIKOS<16> on benchmarks in table 3.
It outputs `tab3.csv`.
This can be used to generate table 3.
This takes roughly 12 hours, if using only a single machine.

```
$ ./reproduce_tab3-16.sh
```

The following command measures the speedups for the benchmark with highest
scalability, `./benchmarks/OSS/audit-2.8.4/aureport.bc/`.
The result for this benchmark can be found in the first entry of the table 3.

```
$ ./measure_tab3-aureport.sh
>>> Running time of IKOS  = 684.19316 seconds.
>>> Running time of PIKOS<4> = 188.25443 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS<4>) = 3.63x.
>>> Running time of PIKOS<8> = 104.18474 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS<8>) = 6.57x.
>>> Running time of PIKOS<12> = 75.86368 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS<12>) = 9.02x.
>>> Running time of PIKOS<16> = 62.36445 seconds.
>>> Speedup (running time of IKOS / running time of PIKOS<16>) = 10.97x.
```

### Generating tables and figures

The csv file produced above can be used to generate tables and figures in the paper.
We will demonstrate with the data obtained by the authors in `./results-paper/all.csv`,
which can be reproduced by using the script described in [Reproduce all](#Reproduce-all).

#### Fig. 5

The script `generate_fig5.py` generates the scatter plot in Fig. 5.
It also outputs the means described in the evaluation section.
It requires the columns `walltime (s)`, `walltime (s)-4`, and `speedup-4` in
the csv file.
It outputs `fig5.png`.

```
$ ./generate_fig5.py ./results-paper/all.csv
```

#### Fig. 6

The script `generate_fig6.py` generates the histograms in Fig. 6.
It requires the columns `walltime (s)`, `walltime (s)-4`, and `speedup-4` in
the csv file.
It outputs 4 subfigures, `fig6-[0~3].png`.

```
$ ./generate_fig6.py ./results-paper/all.csv
```

#### Fig. 7

The script `generate_fig9.py` generates box plot and violin plot in Fig. 7.
It requires the columns `walltime (s)`, `speedup-2`, `speedup-4`, `speedup-6`, and `speedup-8` in the csv file.
It outputs `fig7-a.png` and `fig7-b.png`.

```
$ ./generate_fig7.py ./results-paper/all.csv
```
#### Fig. 8

The script `generate_fig8.py` generates scalability coefficent plot in Fig. 8.
It requires the columns `walltime (s)`, `speedup-2`, `speedup-4`, `speedup-6`, and `speedup-8` in the csv file.
It outputs `fig8-a.png` and `fig8-b.png`.

```
$ ./generate_fig8.py ./results-paper/all.csv
```

#### Table 2

The script `generate_tab3.py` generates the entries for the table 2.
It requires the columns `benchmark`, `category`, `walltime (s)`, `walltime (s)-4`,
and `speedup-4` in the csv file.
It outputs `tab2-speedup.csv` and `tab2-ikos.csv`, which are used to fill table 2.

```
$ ./generate_tab2.py ./results-paper/all.csv
```

#### Table 3
The script `generate_tab4.py` chooses the benchmarks for table 3 based on the
scalability coefficient.
It requires the columns `benchmark`, `category`, `cs`, `walltime (s)`, `walltime (s)-4`, `speedup-2`, `speedup-4`, `speedup-6`, and `speedup-8` in the csv file.
It outputs `tab3-candidates.csv`.
One has to run PIKOS<12> and PIKOS<16> on these benchmarks additionally to complete the table 3.

```
$ ./generate_tab3.py ./results-paper/all.csv
```

## People
- [Aditya V. Thakur](https://thakur.cs.ucdavis.edu/) can be reached at
  [avthakur@ucdavis.edu](mailto:avthakur@ucdavis.edu).
- Arnaud J. Venet can be reached at
  [ajv@fb.com](mailto:ajv@fb.com).
- [Sung Kook Kim](https://skkeem.github.io/) can be reached at
  [sklkim@ucdavis.edu](mailto:sklkim@ucdavis.edu).
