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

/* functie procesare intrare curenta de tipul regular file */
int processRegularFile(char *file_name, int pipe1[2])
{
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

    // inchide capatul de citire al pipe-ului
    if (close(pipe1[0]) < 0)
    {
        perror("Eroare inchidere capat scriere pipe!");
        exit(EXIT_FAILURE);
    }

    // deschidere fisier pentru citire continut
    // int fd = open(file_name, O_RDONLY);
    // if (fd == -1) {
    //     perror("Eroare deschidere fișier pentru citire\n");
    //     exit(EXIT_FAILURE);
    // }

    char buffer[1024];
    ssize_t bytesRead = 0;

    // // citim continutul fisierul si trimitem prin pipe la procesul fiu 2
    // while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
    //     write(pipe1[1], buffer, bytesRead);
    // }

    // Scrie ceva în pipe 1
    const char *message1 = "Mesaj de la primul fiu";
    write(pipe1[1], message1, strlen(message1) + 1);

    // // inchide fișierul
    // if (close(fd) < 0)
    // {
    //     perror("Eroare inchidere fisier dupa citire!");
    //     exit(EXIT_FAILURE);
    // }

    // inchide capatul de scriere al pipe-ului
    if (close(pipe1[1]) < 0)
    {
        perror("Eroare inchidere capat scriere pipe!");
        exit(EXIT_FAILURE);
    }

    return 8;   // returnam numarul de linii scrise in fisierul de iesire
}

void processFileContent(int pipe1[2], int pipe2[2])
{
    close(pipe1[1]);

    // Citește din pipe 1
    char buffer[100];
    read(pipe1[0], buffer, sizeof(buffer));

    printf("Al doilea fiu a citit: %s\n", buffer);

    // Închide capătul de citire al pipe-ului 1
    close(pipe1[0]);

    // Închide capătul nepotrivit al pipe-ului 2
    close(pipe2[0]);

    // Scrie ceva în pipe 2
    const char *message2 = "Mesaj de la al doilea fiu";
    write(pipe2[1], message2, strlen(message2) + 1);

    // Închide capătul de scriere al pipe-ului 2
    close(pipe2[1]);
    // Citește conținutul prin pipe
    // char buffer[1024];
    // ssize_t bytesRead;
    
    // while ((bytesRead = read(pipe_read, buffer, sizeof(buffer))) > 0)
    // {
    //     // Procesează conținutul
    //     // ...

    //     // Trimite numărul de "propoziții corecte" prin pipe
    //     int numPropozitiiCorecte = 42; // Modificați în funcție de implementarea dvs.
    //     write(pipe_write, &numPropozitiiCorecte, sizeof(numPropozitiiCorecte));
    // }

    // // inchide capatul de citire al pipe-ului
    // if (close(pipe_read) < 0)
    // {
    //     perror("Eroare inchidere capat scriere pipe!");
    //     exit(EXIT_FAILURE);
    // }

    // // inchide capatul de scriere al pipe-ului
    // if (close(pipe_write) < 0)
    // {
    //     perror("Eroare inchidere capat scriere pipe!");
    //     exit(EXIT_FAILURE);
    // }
}