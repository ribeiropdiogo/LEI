#!/usr/bin/python3

import socket
from tracerapi import ESTracerAPI

def parse(tracer,line):
    params = line.split(" ")

    if params[0] == "mknod":
        tracer.add_doc("mknod",    {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "creation_mode": params[6]})
    elif params[0] == "access":
        tracer.add_doc("acess",    {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "access_mode": params[6]})
    elif params[0] == "open":
        tracer.add_doc("open",     {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "flags": params[6]})
    elif params[0] == "read":
        tracer.add_doc("read",     {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "bytes_read": params[6]})
    elif params[0] == "pread":
        tracer.add_doc("pread",    {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "bytes_read": params[6], "offset": params[7]})
    elif params[0] == "write":
        tracer.add_doc("write",    {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "bytes_write": params[6]})
    elif params[0] == "pwrite":
        tracer.add_doc("pwrite",   {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "bytes_write": params[6], "offset": params[7]})
    elif params[0] == "truncate":
        tracer.add_doc("truncate", {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "path": params[5], "new_size": params[6]})
    elif params[0] == "lseek":
        tracer.add_doc("lseek",    {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "file_descriptor": params[5], "offset": params[6]})
    elif params[0] == "fsync":
        tracer.add_doc("fsync",    {"pid": params[1], "timestamp_inicial": params[2], "timestamp_final": params[3], "error": params[4], "file_descriptor": params[5]})


def start(host, port):
    tracer = ESTracerAPI()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        print("> Server ready")
        conn, addr = s.accept()
        with conn:
            print('> Connected by', addr)
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                parse(tracer, data.decode('utf-8'))
