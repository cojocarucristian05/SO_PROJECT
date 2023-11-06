#!/bin/bash

if [ $# -ne 1 ]; then
    exit 1
fi

REGEX="\.bmp$"

if [[ ! $1 =~ $REGEX ]]; then
    exit 2
fi