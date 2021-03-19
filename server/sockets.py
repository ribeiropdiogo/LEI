#!/usr/bin/python3

import socket


def start(host, port):
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
                print(data.decode('utf-8'))
