#!/bin/sh
cd jansson-2.14
./configure
make
make check
make install
