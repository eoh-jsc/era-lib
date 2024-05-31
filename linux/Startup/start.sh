#!/bin/bash

token="YourAuthToken"

board_type=""
token_file="./token.txt"

if [ -f ${token_file} ]; then
    token=$(cat "${token_file}")
fi

case "$1" in
raspberry|tinker)
    board_type="$1"
    ;;
*)
    exec bash
    ;;
esac

cd ../
command_run="sudo ./era --token="${token}
command_build="make clean all target="${board_type}
if [ ! -f "./era" ]; then
    exec ${command_build}
fi

exec ${command_run}
