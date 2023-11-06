#!/bin/bash

if [ $# -ne 1 ]; then 
    exit 1
fi

if [ ! -d $1 ]; then
    exit 2
fi
