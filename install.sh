#!/bin/sh
cd jansson-2.13
./configure
make
make check
make install
