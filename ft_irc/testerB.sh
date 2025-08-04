#!/bin/bash

LOG="tester.log"
HOST="localhost"
PORT=6667
TIME=3

COMMANDS=$(cat << EOF
PASS 12345
NICK CARLOS
USER carlos * 0: carletos
JOIN #1
PRIVMSG #1 :Hello, world!
PART #1
EOF
)

echo "Test start at $(date)" >> "$LOG"
echo "Sending commands to $HOST:$PORT" >> "$LOG"

OUTPUT=$( printf "%s\n" "$COMMANDS" | timeout $TIME nc -C $HOST $PORT)

echo "$OUTPUT" >> "$LOG"

echo "=== Saved in $LOG ==="

echo "$OUTPUT"
