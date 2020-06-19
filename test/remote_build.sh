#!/bin/bash

echo "Copying code..."

USER="u62116@t5600.rmi.yaht.net"

while read CODE_FILE
do
    REMOTE_FILE=$(echo "$CODE_FILE" | sed 's|../||')
    echo "$CODE_FILE -> $REMOTE_FILE"

    scp "${CODE_FILE}" "${USER}:~/${REMOTE_FILE}"
done < <(find ../Fractal -name "*.cpp" -or -name "*.h")

echo "Building"

ssh "$USER" "./build.sh; echo $?"