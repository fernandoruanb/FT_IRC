#!/bin/bash

LOG="tester.log"
HOST="localhost"
PORT=6667
TIME=10

COMMANDS=$(cat << EOF
PASS 12345
NICK Miku
USER Miku * 0: Hatsune Miku
JOIN #channel1,#channel2,#channel3
MODE #channel2 +k helloWorld
JOIN #channel2 helloWorld
EOF
)

echo "Test start at $(date)" >> "$LOG"
echo "Sending commands to $HOST:$PORT" >> "$LOG"

OUTPUT=$( printf "%s\n" "$COMMANDS" | timeout $TIME nc -C $HOST $PORT)

echo "$OUTPUT" >> "$LOG"

echo "=== Saved in $LOG ==="

echo "$OUTPUT"
