#!/bin/bash

DIR="/data/local/tmp"
EXE=RsaFractal

CMD="cd $DIR; ./$EXE $@"
echo "Android cmd: $CMD"
adb shell "$CMD"
adb shell sync
