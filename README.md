# BJB

This in an C++ implementation of [involutive algorithm](https://www.researchgate.net/publication/2117106_Involutive_Algorithms_for_Computing_Groebner_Bases) based on [Janet-like monomial division](https://www.researchgate.net/publication/221588792_Janet-Like_Monomial_Division). The input of `bjb` util is the initial boolean basis in [GNV format](https://github.com/mzinin/groebner_benchmarks), the output is the corresponding boolean [Gröbner basis](https://en.wikipedia.org/wiki/Gröbner_basis).


## Getting Started

### Prerequisites

To compile this project one would need:
* C++ compiler with C++14 support (tested ones are GCC and MSVC)
* CMake >= 3.5.0

### Compile library

On Linux:
```
cd ./build
cmake ..
make
```
On Windows do the following:
```
cd ./build
cmake ..
```
and open `bjb.sln` with Visual Studio.


## Usage

The `bjb` utility should be run in the following way:
```
./bjb [options] <file_name.gnv>
```

The supported option are:

| Short key | Long key             | Description                         |
|-----------|----------------------|-------------------------------------|
| -h        | --help               | print help message and exit         |
| -v        | --version            | print version and exit              |
| -s        | --collect-statistics | collect and print out statistics    |
| -a        | --print-answer       | [rint out constructed Gröbner Basis |

`bjb` will read the content on the GNV file and build a boolean Gröbner basis for the initial basis. If and only if the GNV file contains the correct answer, the output message `The answer is CORRECT` of `The answer is WRONG` has meaning.

### Get a boolean Gröbner basis

To get the whole answer, i.e. a boolean Gröbner basis, do not forget to add `-a` key:

```
$ ./bjb -a life4.gnv
[0] = x2*x3*x4 + x0*x3 + x1*x3 + x0*x4 + x1*x4 + x2*x4
[1] = x1*x3*x4 + x0*x3 + x2*x3 + x0*x4 + x1*x4 + x2*x4
[2] = x0*x3*x4 + x1*x3 + x2*x3 + x0*x4 + x1*x4 + x2*x4
[3] = x1*x2*x4 + x0*x1 + x0*x2 + x0*x4 + x1*x4 + x2*x4 + x4
[4] = x0*x2*x4 + x0*x1 + x1*x2 + x0*x4 + x1*x4 + x2*x4 + x4
[5] = x0*x1*x4 + x0*x2 + x1*x2 + x0*x4 + x1*x4 + x2*x4 + x4
[6] = x1*x2*x3 + x0*x1 + x0*x2 + x0*x3 + x1*x4 + x2*x4 + x3*x4
[7] = x0*x2*x3 + x0*x1 + x1*x2 + x1*x3 + x0*x4 + x2*x4 + x3*x4
[8] = x0*x1*x3 + x0*x2 + x1*x2 + x2*x3 + x0*x4 + x1*x4 + x3*x4
[9] = x0*x1*x2 + x0*x1 + x0*x2 + x1*x2 + x0*x4 + x1*x4 + x2*x4 + x4

        Memory used: 8.5 Mb
 AutoReduction time: 0.00 sec
       Running time: 0.01 sec

The answer is CORRECT
```

### Run a benchmark

That is the default behavior of the utility. Just run it with GNV file and no other options. It will print the time spent and the memory consumed for constructing boolean Gröbner basis:

```
$ ./bjb life9.gnv
        Memory used: 9.5 Mb
 AutoReduction time: 0.00 sec
       Running time: 4.73 sec

The answer is CORRECT
```

### Get a full statistics:

To get all collected statistics add `-s` key:

```
$ ./bjb -s life9.gnv
Non-Multiple Prolongations considered : 5934
Non-Zero Reductions made              : 2427 (40.90%)
Zero Reductions made                  : 3507 (59.10%)

Average Non-Multiple Prolongation length : 30.35
Average Non-Zero Reduction length        : 20.31

Memory used        : 9.5 Mb
AutoReduction time : 0.00 sec
Running time       : 5.11 sec

The answer is CORRECT
```
