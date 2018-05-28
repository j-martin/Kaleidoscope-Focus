#! /usr/bin/env python3
## kaleidoscope-focus -- Bidirectional communication plugin, host helper
## Copyright (C) 2017, 2018  Gergely Nagy, Jesus Alvarez
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

import serial
import sys
import time
import os
import readline
import atexit
import io
import signal
import traceback
import argparse

class Commander (object):
    serial = None
    _args = None
    _ser = None

    def __init__(self, args):
        self._args = args
        self.connect()

    def connect(self):
        self._ser = serial.Serial(self._args.device, baudrate=9600, bytesize=8,
                                  parity='N', stopbits=1, timeout=5, write_timeout=5)
        self.serial = io.TextIOWrapper(io.BufferedRWPair(self._ser, self._ser))

    def echo(self, text="", prompt=True):
        if self._args.quiet:
            return
        if prompt:
            print("> {}".format(text))
            return
        print("{}".format(text))

    def close(self):
        if self._ser.is_open:
            self.echo("closing connection...")
            self._ser.close()

    def write(self, text):
        if not self._ser.is_open:
            self.connect()
        self._ser.write(text.encode())
        if '\n' not in text:
            self._ser.write(b'\n')

    def history(self):
        histfile = os.path.join (os.path.expanduser ("~"), ".kaleidoscope-commander.hist")
        try:
            readline.read_history_file (histfile)
        except IOError:
            pass
        atexit.register (readline.write_history_file, histfile)

    def run (self):
        if self._args.quiet:
            cmd = input();
        else:
            cmd = input("> ");

        if cmd == "quit" or cmd == "exit":
            self.close()
            sys.exit (0)

        if cmd == "":
            return

        self.write(cmd)

        while True:
            resultLine = self.serial.readline()
            if len(resultLine) == 0:
                self.echo("no output")
                break

            if resultLine == "\r\n" or resultLine == "\n":
                resultLine = " "
            else:
                resultLine = resultLine.rstrip ()

            if resultLine == ".":
                break

            if resultLine:
                prompt = ""
                if not self._args.quiet:
                    prompt = "< "
                print("{}{}".format(prompt, resultLine))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-q", "--quiet", action="store_true",
                        help="Operate quietly, only displaying raw communication")
    parser.add_argument("-d", "--device", action="store",
                        help="Device to open (defaults to `/dev/ttyACM0`)",
                        default="/dev/ttyACM0")
    args = parser.parse_args()

    cli = Commander(args)

    def _signal_handler(signum, frame):
        cli.echo(prompt=False)
        cli.echo("bye!")
        cli.close()
        sys.exit(0)

    signal.signal(signal.SIGINT, _signal_handler)

    cli.connect()
    cli.history()

    while True:
        try:
            cli.run ()
        except EOFError:
            cli.close()
            sys.exit(0)
        except Exception as e:
            cli.echo("ERROR!")
            traceback.print_exc()
            cli.close()
            sys.exit(1)
