#!/bin/bash

EXECUTABIL="program"
MAIN="main.c"
UTILS="./utils/utils.c ./utils/printer.c"
ARGS="dir/"

gcc -Wall -o $EXECUTABIL $UTILS $MAIN

./$EXECUTABIL $ARGS