#!/bin/bash

LOG="tester.log"
HOST="localhost"
PORT=6667
TIME=10

COMMANDS=$(cat << EOF
PASS 12345
NICK Miku
USER Miku * 0: Hatsune Miku
MODE #Generic +k helloWorld
MODE #Generic +k lala
MODE #Generic +k
MODE #Generic
MODE #Generic -k
MODE Generic
EOF
)

echo "Test start at $(date)" >> "$LOG"
echo "Sending commands to $HOST:$PORT" >> "$LOG"

OUTPUT=$( printf "%s\n" "$COMMANDS" | timeout $TIME nc -C $HOST $PORT)

echo "$OUTPUT" >> "$LOG"

echo "=== Saved in $LOG ==="

echo "$OUTPUT"
