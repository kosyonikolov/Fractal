#!/bin/bash

./time.sh dev 1024x1024 -1:1:-1:1 4 8 6 4 2
sleep 5m
./time.sh dev 1024x1024 -1:1:-1:1 8 8 6 4 2
sleep 5m
./time.sh dev 1024x1024 -1:1:-1:1 16 8 6 4 2
sleep 5m
./time.sh dev 1024x1024 -1:1:-1:1 32 8 6 4 2