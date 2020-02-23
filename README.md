# shm_sync

This repository contains companion code for the following paper:

Shinhyung Yang, Seongho Jeong, Byunguk Min, Yeonsoo Kim, Bernd Burgstaller, and Johann Blieberger.<br/>
*Design-space evaluation for non-blocking synchronization in Ada:
lock elision of protected objects, concurrent objects, and low-level atomics*.
Technical Report TR-0004, ELC Lab, Department of Computer Science, Yonsei University, 2020. \[[pdf](https://elc.yonsei.ac.kr/publications/TR_Ada_nb_sync.pdf)\]

If you use this code, please cite the paper using the below BibTeX reference.

```
@techreport{shmsync:TR,
  title = {Design-space evaluation for non-blocking synchronization in {Ada}:
           lock elision of protected objects, concurrent objects, and
           low-level atomics},
  author = {Shinhyung Yang and Seongho Jeong and Byunguk Min and Yeonsoo Kim
            and Bernd Burgstaller and Johann Blieberger},
  institution = {Yonsei University, Department of Computer Science},
  number = {TR 0004},
  month = {February},
  year = {2020},
}
```

# Introduction

shm_sync is a synchronization benchmark suite for shared-memory multicore architectures.
In contrast to prior work, it provides relaxations to determine the performance overhead
incurred by a sequentially consistent high-level language memory model.



###### **News**
* [x] Feb 10, 2020: GitHub repository created 


## Prerequisites
Please refer to the top-level CMakeLists.txt file for the configuration options of shm_sync.
The list of prerequisites assumes a full build of both the Ada and C++ benchmarks. The
shm_sync benchmarks require a Linux system with the following software packages installed
(stated versions are known to work and represent the minimum for GCC and CMake).

* [GNAT community edition](https://www.adacore.com/download) 2018 
* [GCC](http://ftp.gnu.org/gnu/gcc/) 7.1.0 
* [PAPI](https://icl.utk.edu/papi/index.html) 5.5.0 
* [CMake](https://cmake.org/) 3.10.1 
* [Likwid](https://github.com/RRZE-HPC/likwid/) 4.3.4
* [Boost](https://www.boost.org/) 1.69.0
* [Python](https://www.python.org/download/releases/3.0/) 3.0

## Building the benchmark suite

**shm_sync** is a CMake-based project. To build the project, move to project
root and follow the instructions below. The name of the build directory
decides between debug and release builds.

To build the benchmarks in debug mode:
```sh
$ mkdir build_dbg
$ cd build_dbg
$ cmake ..
$ make
```

To build the benchmarks in release mode:
```sh
$ mkdir build_rel
$ cd build_rel
$ cmake ..
$ make
```

To use Likwid for running benchmarks:
```sh
$ cmake -DLIKWID_ROOT=/opt/likwid/4.3.4 ..
```

The benchmark suite supports 16-byte (double-word) atomic load, atomic store, and
compare-and-swap.
Benchmarks using 16-byte atomic operations tend to be considerbly slower than their 8-byte
counterparts. In our experiments, this was e.g., noticeable with the AdaLockFree benchmark.

To build with 16-byte atomic operations enabled:
```sh
$ cmake -DBUILD_DWATOMIC=ON ..
```

## Running benchmarks

In `scripts/run`:
```sh
$ python3 ./run.py TATASLock
```

```sh
# Run with detailed benchmark settings (settings.py)
$ python3 ./run.py TATASLock --threads 28 --executions 100 --iterations 10M
```

To run benchmark with scaling run from (1..t) threads:
```sh
$ python3 ./run.py TATASLock --threads 28 --executions 100 --iterations 10M -r
```

To run benchmark with scaling run from (1..t) threads and to view overview only:
```sh
$ python3 ./run.py TATASLock --threads 28 --executions 100 --iterations 10M -r -o
```

To run lock benchmark with busy loop lasting for 30 nsec inside critical section (-u) and busy loop lasting for 50 nsec outside critical section (-y):
```sh
$ python3 ./run.py TATASLock --threads 28 --executions 100 --iterations 10M -r -o -u 30 -y 50
```
## Obtaning benchmark data

Benchmark profiling data files are stored under the data folder in the following
subdirectory scheme:
```sh
[PROJECT_ROOT]/data/[datetime]/[benchmark_name]/[binary_name]
```
For example:
```sh
shm_sync_latest/data/April30_2019_10:15:22/TASLock
```

To get statistics and R-plots measuring ExecutionTime and Throughput, go to
`scripts/run` and run Drawing.py:
```sh
$ python3 ./Drawing.py ../../data/[datetime]/[benchmark_name] -t [number_of_threads] -i [number_of_iterations]
```
For example, to plot given sample_ArrayLock.txt:
```sh
$ python3 ./Drawing.py sample_ArrayLock.txt -t 28 -i 10000000
```

To plot given sample_ArrayLock.txt and sample_CLHQueueLock.txt separately with single operation:
```sh
$ python3 ./Drawing.py sample_ArrayLock.txt sample_CLHQueueLock.txt -t 28 14 -i 10000000 10000
```
Result Plot

<img src="/image/stats_sample_ArrayLock_ExecutionTime.png" width="450px" height="300px" title="arraylock_plot" alt="arraylock"></img><br/>

<img src="/image/stats_sample_CLHQueueLock_ExecutionTime.png" width="450px" height="300px" title="CLHQueuelock_plot" alt="clhqueuelock"></img><br/>

To plot given sample_ArrayLock.txt and sample_TATASLock.txt into a single figure, use "-c" flag.
But their [number_of_threads] for -t and [number_of_iterations] for -i should be same.
```sh
$ python3 ./Drawing.py sample_ArrayLock.txt sample_TATASLock.txt -t 28 14 -i 10000000 -c
```
Result plot

<img src="/image/stats_combined_ExecutionTime.png" width="450px" height="300px" title="combined_plot" alt="combined"></img><br/>

## Troubleshooting

If cmake cannot locate the Boost library, add the library's root path to the cmake
command line:
```sh
# version and path of Boost may vary
$ cmake -DBOOST_ROOT=/opt/boost/1.69.0 ..
```

## Contact

Please email Bernd Burgstaller at bburg@yonsei.ac.kr and include [shm_sync] in the
subject line.

## Authors

[Shinhyung Yang](https://elc.yonsei.ac.kr/people_stud.htm)<br/>
[Seongho Jeong](https://elc.yonsei.ac.kr/people_stud.htm)<br/>
[Byunguk Min](https://elc.yonsei.ac.kr/people_stud.htm)<br/>
[Yeonsoo Kim](https://elc.yonsei.ac.kr/people_stud.htm)<br/>
[Bernd Burgstaller](https://elc.yonsei.ac.kr/people.htm)<br/>
[Johann Blieberger](https://informatics.tuwien.ac.at/people/johann-blieberger)

## License

The shm_sync benchmarks are open source software distributed under the
license agreement found in LICENSE.txt.

## Acknowledgements

This work was supported by the Austrian Science Fund (FWF) project
I 1035N23, and by the Next-Generation Information Computing
Development Program through the National Research Foundation of
Korea (NRF), funded by the Ministry of Science, ICT & Future Planning
under grant NRF-2015-M3C4A-7065522.
