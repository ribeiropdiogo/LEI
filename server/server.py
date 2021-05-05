#!/usr/bin/python3

import yaml
import sockets
import sys
import asyncio
from concurrent.futures import ThreadPoolExecutor

def main():
    try:
        print("> Starting the server ")
        executor = ThreadPoolExecutor(max_workers=16)

        print("> Loading configuration file")
        with open("config.yml", "r") as cfgfile:
            cfg = yaml.safe_load(cfgfile)

        host = cfg["server"]["host"]
        port = cfg["server"]["port"]
        bytes_read = cfg["server"]["socket_bytes"]

        sockets.start(host, port, bytes_read,executor)

    except KeyboardInterrupt:
        print("> Server shutting down")
        sys.exit(0)


if __name__ == "__main__":
    main()