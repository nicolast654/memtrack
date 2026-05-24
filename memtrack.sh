#!/bin/bash

MEMTRACK="/usr/local/lib/memtrack/memtrack.so"

if [[ ! -f ${MEMTRACK} ]]; then
    echo "Can't find memtrack.so Library!"
    echo "Run make install to install it"
else
    LD_PRELOAD="${MEMTRACK}" "$@"
fi


