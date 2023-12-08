#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "path.h"
#include "process_directory.h"
#include "writer.h"
#include "utils.h"

/* functie procesare intrare curenta de tipul dir */
int processDirectory(char *file_name, char *din_path, char *dout_path)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat dir_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);  // extragere nume pentru <intrare>_statistica

    sprintf(path, "%s%s", din_path, file_name);     // formare path intrare
    sprintf(outputFilePath, "%s%s_statistica.txt", dout_path, nume_intare);   // formare path iesire
    free(nume_intare);      // eliberare memorie nume

    // citire info dir folosind functia stat
    if (stat(path, &dir_stat) < 0)
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

    // scriere date in fisierul iesire
    dprintf(sfd, "nume director: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", dir_stat.st_uid);
    writePermission(sfd, dir_stat.st_mode);

    // inchidere fisier
    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }

    return 5;   // returnam numarul de linii scrise in fisierul de iesire
}