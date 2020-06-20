#!/bin/bash

cd /home/kosyo/Documents/Fractal/test

date >> spam.log

./time.sh server 2048x2048 -1:1:-1:1 128x16 32 28 24 20 16 12 8 4  
./time.sh server 2048x2048 -1:1:-1:1 128x8 32 28 24 20 16 12