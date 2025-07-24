#!/bin/bash

LOG="tester.log"
HOST="localhost"
PORT=6667
TIME=3

COMMANDS=$(cat << EOF
PASS 12345
NICK Miku
USER Miku * 0: Hatsune Miku
Hello world
MODE Miku +io
MODE Miku
MODE #Generic
EOF
)

echo "Test start at $(date)" >> "$LOG"
echo "Sending commands to $HOST:$PORT" >> "$LOG"

OUTPUT=$( printf "%s\n" "$COMMANDS" | timeout $TIME nc $HOST $PORT)

echo "$OUTPUT" >> "$LOG"

echo "=== Saved in $LOG ==="

echo "$OUTPUT"
