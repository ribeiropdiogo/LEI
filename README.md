# Real-Time Tracer

This project consists in a real-time tracer built using a FUSE filesystem. All the gathered data is stored on Elasticsearch ans we use Kibana to view the collected data. Our system allows a user to run applications on it and analyze data in real-time such as types os system calls, theira average duration and proportion, most accessed paths, and more. This is complemented by the usage of Metricbeat to better understand the usage of system resources.

# 🚀 Getting Started

## 🔧 Prerequisites

In order to run the project you need to install the following:

* fuse
* libfuse-dev
* python
* elasticsearch
* kibana
* gcc

This project was **only tested with Ubuntu** and probably won't work on MacOS.

## 📦 Running

### Elasticsearch & Kibana

To start elasticsearch and kibana, you need the following commands:

```bash
sudo systemctl start elasticsearch.service
sudo systemctl start kibana.service
```

To stop them, just repeat the commands with `stop` instead of `start`:

```bash
sudo systemctl stop elasticsearch.service
sudo systemctl stop kibana.service
```

### Python Server

In order to run the python server which connects the filesystem to our elasticsearch server, you just need to run the following in the `server` folder:

```bash
python3 server.py
```

The `server` folder also contains a `config.yml`file with the server's configuration parameters.

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

In order to run one of the test programs, you can use the following commands:

```bash
make compile
make rocks (or) make psql
```

## :nerd_face: How it Works

When running `make run` (which is included in `make all`), you are mounting the folder `fs_data` at the mountpoint `fs`. The result is a filesystem located at the mountpoint which supports our tracing fuctionality.

The system call used in this filesystem are captured and sent via sockets to the `server` we built in python. This server processes this data by inserting it on elasticsearch. After beeing indexed on elasticsearch, you can see all the data on Kibana using our dashboards. 


# :muscle: Developed by:

This project was built as part of **Software Engineering Laboratories @ University of Minho** by:

* A84442 - [Diogo Ribeiro](https://github.com/ribeiropdiogo)
* A83712 - [Rui Mendes](https://github.com/ruimendes29)
