#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include "script_executor.h"
#include "process_bmp.h"
#include "writer.h"

void processFile(char *file_name, struct stat file_stat)
{
    char command[300];
    sprintf(command, "%s %s", BMP_REGEX_SCRIPT_PATH, file_name);
    if(system(command) != 0)
    {
        writeRegularFileStatistics(file_name, file_stat);
    }
    else
    {
        processImage(file_name, file_stat);
    }
}