#!/bin/bash

cd /home/kosyo/Documents/Fractal/test

date >> spam.log

./time.sh server 2048x2048 -1:1:-1:1 32 8 16 20 24 28 32  
./time.sh server 2048x2048 -1:1:-1:1 16 8 16 20 24 28 32  
./time.sh server 4096x4096 -1:1:-1:1 32 16 20 24 28 32  
./time.sh server 4096x4096 -1:1:-1:1 16 16 20 24 28 32  