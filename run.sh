#!/bin/bash

EXECUTABIL="program"
MAIN="main.c"
UTILS="./utils/utils.c"
ARGS="dir/"

gcc -Wall -o $EXECUTABIL $UTILS $MAIN

./$EXECUTABIL $ARGS