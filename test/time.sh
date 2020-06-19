#!/bin/bash

USAGE_MSG="Usage: ./time.sh [pc|dev|server] [size] [complex size] [chunk size] <thread counts...>"

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
elif [[ $TYPE == "server" ]]
then
    CMD=./run_server.sh
fi

SIZE="$2"
C_SIZE="$3"
CHUNK_SIZE="$4"
ATTEMPTS=7

NOW=$(date +%H%M%S)
RUN_ID="${TYPE}_${CHUNK_SIZE}_${NOW}"

# write header
OUT_FILE="stat_${RUN_ID}.csv"
echo "threads,chunksize,time" > "$OUT_FILE"

LOG_FILE="${RUN_ID}.log"
date > "$LOG_FILE"

shift 4

for T_COUNT in "$@"
do
    G=$CHUNK_SIZE
    if [[ $T_COUNT -eq 1 ]]
    then
        G=1
    fi

    LINE="$T_COUNT,$G"
    ITER=0
    while [[ $ITER -lt $ATTEMPTS ]]
    do
        echo "Attempt $ITER / $ATTEMPTS"
        echo $CMD -s $SIZE -r $C_SIZE -t $T_COUNT -c $G -v 1
        echo $CMD -s $SIZE -r $C_SIZE -t $T_COUNT -c $G -v 1 >> "$LOG_FILE"
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
