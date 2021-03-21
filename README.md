# LEI - Big Data

This project consists in a FUSE filesystem complemented with tracing functionalities. The result of the tracing system is further used in Elastic Search in order to analyze data and discover potential problems.

# 🚀 Getting Started

## 🔧 Prerequisites

## 📦 Running

### Fuse Filesystem

The `fuse` folder contains a `Makefile` wich can be used the following way:

If you want to compile and mount(run) the flesystem:

```bash
make all
```

or

```bash
make compile
make run
```

If you want to unmount the filesystem you just need to run:

```bash
make end
```

### Test Program

In the `test`folder there is a small sequential test program. In order to compile and run you just need to have the library files in the `/usr/lib` folder and run the following commands:

```bash
g++ -std=c++17 test.cpp -o test -lfsrouter
./test
```

## :nerd_face: How it Works

When running `make run` (which is included in `make all`), you are mounting the folder `fs_data` which in the mountpoint `fs`. The result is a filesystem located at the mountpoint which supports our library. The system call used in this filesystem are the ones defined in the library. The same logic is applied to the teste program.


# :muscle: Developed by:

* A84442 - [Diogo Ribeiro](https://github.com/ribeiropdiogo)
