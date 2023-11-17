#!/bin/bash

EXECUTABIL="program"
MAIN="main.c"
UTILS="./utils/script_executor.c ./utils/process_bmp.c ./utils/writer.c ./utils/process_dir.c"
ARGS="dir/ dir_iesire/"

gcc -Wall -o $EXECUTABIL $UTILS $MAIN

./$EXECUTABIL $ARGS

rm $EXECUTABIL