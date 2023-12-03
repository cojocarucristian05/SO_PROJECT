#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include "process_regular_file.h"
#include "path.h"
#include "utils.h"
#include "writer.h"

#define BUFFER_SIZE 1024

/* functie procesare intrare curenta de tipul regular file scrie statistica, apoi scrie la iesirea standard continutul fisierului*/
int processRegularFile(char *file_name)
{
    // printf("nume fisier: %s\n", file_name);
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat file_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);      // extragere nume pentru <intrare>_statistica

    sprintf(path, "%s%s", DIR_PATH, file_name);     // formare path intrare
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, nume_intare);       // formare path iesire
    free(nume_intare);      // eliberare memorie nume

    // citire info fisier folosind functia stat
    if (stat(path, &file_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    // deschidere fisier iesire
    sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(EXIT_FAILURE);
    }

    // scriere date
    dprintf(sfd, "nume fisier: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(file_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", file_stat.st_nlink);
    writePermission(sfd, file_stat.st_mode);

    // inchidere fisier iesire
    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }


    // printf("%s\n", file_name);
    int fd = open(path, O_RDONLY);
    if (fd < 0) 
    {
        perror("Eroare deschidere fisier citire continut!");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = 0;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    if (close(fd)) 
    {
        perror("Eroare inchidere fisier dupa citire continut\n");
        exit(EXIT_FAILURE);
    }

    return 8;   // returnam numarul de linii scrise in fisierul de iesire
}

/* functie de procesare continut -> trimite in executie scriptul care verifica numarul de propozitii corecte pentru continutul fisierului curent*/
void processFileContent(char *c)
{
    // executa script
    execlp("bash", "bash", "./scripts/numara_propozitii_corecte.sh", c, (char *)NULL);

    // termina al doilea fiu in caz de eroare
    perror("Eroare exec");
    exit(EXIT_FAILURE);
}