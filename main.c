#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./utils/script_executor.h"
#include "./utils/process_dir.h"

#define EXPECTED_NUMBER_OF_ARGS 2

void checkNumberOfArgs(int argc, char *argv[])
{
    if (argc != EXPECTED_NUMBER_OF_ARGS)
    {
        printf("Usage %s %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
}

void checkTypeForArgument(char *argv[])
{
    struct stat st;
    int n = 0;
    n = stat(argv[1], &st);
    if (n < 0)
    {
        printf("Usage %s %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
    if (!S_ISDIR(st.st_mode))
    {
        printf("Usage %s %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    checkNumberOfArgs(argc, argv);
    checkTypeForArgument(argv);
    processDIR(argv[1]);
    return 0;
}