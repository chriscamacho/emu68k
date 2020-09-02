#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "You need Super COW!"
  exit
fi

socat pty,link=/dev/ttyEmu68Server pty,link=/dev/ttyEmu68Client &

PID=$!
sleep 1
chgrp users /dev/ttyEmu68*
chmod 664 /dev/ttyEmu68*
echo
echo "please connect your terminal to /dev/ttyEmu68Client"
echo
read -n 1 -s -r -p "Press any key to uplug virtual serial cable ($PID)"
echo

kill $PID

echo "disconnected"
echo
