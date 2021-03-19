#!/usr/bin/python3

import yaml
import sockets
import sys

def main():
    try:
        print("> Starting the server ")

        print("> Loading configuration file")
        with open("config.yml", "r") as cfgfile:
            cfg = yaml.safe_load(cfgfile)

        host = cfg["server"]["host"]
        port = cfg["server"]["port"]

        sockets.start(host, port)

    except KeyboardInterrupt:
        print("> Server shutting down")
        sys.exit(0)


if __name__ == "__main__":
    main()