#!/bin/bash

LOG="tester.log"
HOST="localhost"
PORT=6667
TIME=5

COMMANDS=$(cat << EOF
PASS ab
NICK Serjio
USER Serjio * 0: Serjio
JOIN #1
PART #1 : tchau
EOF
)

echo "Test start at $(date)" >> "$LOG"
echo "Sending commands to $HOST:$PORT" >> "$LOG"

OUTPUT=$( printf "%s\n" "$COMMANDS" | timeout $TIME nc -C $HOST $PORT)

echo "$OUTPUT" >> "$LOG"

echo "=== Saved in $LOG ==="

echo "$OUTPUT"
