#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "./utils/process_dir.h"

#define EXPECTED_NUMBER_OF_ARGS 4   /* numar de argumente asteptat */

/* functie care verifica numarul de argumente in linia de comanda */
void checkNumberOfArgs(int argc, char *argv[])
{
    if (argc != EXPECTED_NUMBER_OF_ARGS)
    {
        printf("Usage %s %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
}

/* functie care verifica tipul argumentelor din linia de comanda */
void checkTypeForArgument(char *argv[])
{
    struct stat st;
    int n = 0;
    for (int i = 1; i < EXPECTED_NUMBER_OF_ARGS - 1; i++)
    {
        n = stat(argv[i], &st);
        if (n < 0)
        {
            printf("Usage %s %s\n", argv[0], argv[i]);
            exit(EXIT_FAILURE);
        }
        if (!S_ISDIR(st.st_mode))
        {
            printf("Usage %s %s\n", argv[0], argv[i]);
            exit(EXIT_FAILURE);
        }
    }
}

/* functia main */
int main(int argc, char *argv[])
{
    checkNumberOfArgs(argc, argv);  // verificare numar argumente 
    checkTypeForArgument(argv);     // verificare tipuri argumente
    processDIR(argv[1], argv[2], argv[3]);   // trimitere in executie a procesarii directorului de intrare
    return 0;
}