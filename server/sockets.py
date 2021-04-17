#!/usr/bin/python3

import socket
from tracerapi import ESTracerAPI
from datetime import datetime

def parse_nano_date(str_date):
	dt = datetime.fromtimestamp(int(str_date) // 1000000000)
	return dt.strftime('%Y-%m-%dT%H:%M:%S.') + str(int(int(str_date) % 1000000000)).zfill(9) + '+01:00'

def parse(tracer,line):
    params = line.split(" ")
    esIdx = 'calls'
    
    params[2] = parse_nano_date(params[2])
    #print(params[2])

    if params[0] == "mknod":
        tracer.add_doc(esIdx,    {"type":"mknod", "pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "creation_mode": params[6]})
    elif params[0] == "access":
        tracer.add_doc(esIdx,    {"type":"access","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "access_mode": params[6]})
    elif params[0] == "open":
        tracer.add_doc(esIdx,     {"type":"open","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "flags": params[6]})
    elif params[0] == "read":
        tracer.add_doc(esIdx,     {"type":"read","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "bytes_read": params[6]})
    elif params[0] == "pread":
        tracer.add_doc(esIdx,    {"type":"pread","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "bytes_read": params[6], "offset": params[7]})
    elif params[0] == "write":
        tracer.add_doc(esIdx,    {"type":"write","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "bytes_write": params[6]})
    elif params[0] == "pwrite":
        tracer.add_doc(esIdx,   {"type":"pwrite","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "bytes_write": params[6], "offset": params[7]})
    elif params[0] == "truncate":
        tracer.add_doc(esIdx, {"type":"truncate","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "path": params[5], "new_size": params[6]})
    elif params[0] == "lseek":
        tracer.add_doc(esIdx,    {"type":"lseek","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "file_descriptor": params[5], "offset": params[6]})
    elif params[0] == "fsync":
        tracer.add_doc(esIdx,    {"type":"fsync","pid": params[1], "timestamp_inicial": params[2], "duration": params[3], "error": params[4], "file_descriptor": params[5]})


def start(host, port):
    tracer = ESTracerAPI()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        data = ''
        print("> Server ready")
        conn, addr = s.accept()
        with conn:
            print('> Connected by', addr)
            while True:
                b = conn.recv(256)
                data += b.decode('utf-8')

                if not data:
                    break
                if not data.endswith("\n"):
                    continue

                lines = data.split("\n")
                lines = filter(None, lines)

                for line in lines:
                    #print(line)    
                    parse(tracer, line)

                data = ''

                
