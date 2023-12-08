#!/bin/bash
CHARACTER=$1
EXECUTABIL="program"
MAIN="main.c"
UTILS="./utils/utils.c ./utils/writer.c ./utils/process_bmp.c ./utils/process_directory.c ./utils/process_links.c ./utils/process_regular_file.c ./utils/process_dir.c"
ARGS="./dir_intrare/ ./dir_iesire/ $CHARACTER"

gcc -Wall -o $EXECUTABIL $UTILS $MAIN

./$EXECUTABIL $ARGS

rm $EXECUTABIL