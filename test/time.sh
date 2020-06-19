#!/bin/bash

USAGE_MSG="Usage: ./time.sh [pc|dev] [size] [complex size] [granularity] <thread counts...>"

if [[ $# -lt 5 ]]
then
    echo "$USAGE_MSG"
    exit 1
fi

CMD=./run_pc.sh

TYPE=$1
echo "Type = $TYPE"
if [[ $TYPE == "dev" ]]
then
    #./device_upload.sh
    CMD=./run_device.sh
fi

SIZE="$2"
C_SIZE="$3"
GRANULARITY="$4"
ATTEMPTS=5

# write header
OUT_FILE="stat_${TYPE}_${GRANULARITY}.csv"
echo "threads,granularity,time" > "$OUT_FILE"

LOG_FILE="${TYPE}_${GRANULARITY}.log"
date > "$LOG_FILE"

shift 4

for T_COUNT in "$@"
do
    G=$GRANULARITY
    if [[ $T_COUNT -eq 1 ]]
    then
        G=1
    fi

    LINE="$T_COUNT,$G"
    ITER=0
    while [[ $ITER -lt $ATTEMPTS ]]
    do
        echo "Attempt $ITER / $ATTEMPTS"
        echo $CMD -s $SIZE -r $C_SIZE -t $T_COUNT -g $G -v 1
        echo $CMD -s $SIZE -r $C_SIZE -t $T_COUNT -g $G -v 1 >> "$LOG_FILE"
        OUTPUT=$($CMD -s $SIZE -r $C_SIZE -t $T_COUNT -g $G -v 1)
        echo "$OUTPUT" >> "$LOG_FILE"

        TIME=$(echo "$OUTPUT" | grep -oE "^Generating took.*" | grep -oE "[0-9]+")
        echo "Time: $TIME"

        LINE="$LINE,$TIME" 

        echo "" >> "$LOG_FILE"

        ITER=$((ITER + 1))
    done

    echo "$LINE" >> "$OUT_FILE"
done
