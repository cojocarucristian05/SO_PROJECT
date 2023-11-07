#!/bin/bash

EXECUTABIL="program"
MAIN="main.c"
UTILS="./utils/argument_validator.c ./utils/writer.c"
ARGS="dir/"

gcc -Wall -o $EXECUTABIL $UTILS $MAIN

./$EXECUTABIL $ARGS