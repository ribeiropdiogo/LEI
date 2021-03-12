# LEI - Big Data

This project consists in a FUSE filesystem complemented with tracing functionalities. The result of the tracing system is further used in Elastic Search in order to analyze data and discover potential problems.

# 🚀 Getting Started

## 🔧 Prerequisites

In order to run the project you need to install the following:

* fuse
* libfuse-dev

This project is **only compatible with Ubuntu** and probably won't work on MacOS.

## 📦 Running

The project contains a `Makefile` wich can be used the following way:

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

## :nerd_face: How it Works

When running `make run` (which is included in `make all`), you are mounting the folder `fs_data` which in the mountpoint `fs`. The result is a filesystem located at the mountpoint which supports our tracing fuctionality.


# :muscle: Developed by:

This project was built as part of **Software Engineering Laboratories @ University of Minho** by:

* A84442 - [Diogo Ribeiro](https://github.com/ribeiropdiogo)
* A83712 - [Rui Mendes](https://github.com/ruimendes29)
* A84930 - [Rui Reis](https://github.com/Syrayse)
