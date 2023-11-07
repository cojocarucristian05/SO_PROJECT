#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include "argument_validator.h"

void checkNumberOfArgs(int argc)
{
    if (argc != EXPECTED_NUMBER_OF_ARGS)
    {
        perror("Invalid number of args!");
        exit(1);
    }
}

void checkTypeForArgument(char *argv[])
{
    char command[256];
    sprintf(command, "./scripts/check_argument_type.sh %s", argv[1]);
    if(system(command) != 0)
    {
       printf("Usage %s %s\n", argv[0], argv[1]);
       exit(2);
    }
}