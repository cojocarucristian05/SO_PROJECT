#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "process_links.h"
#include "path.h"
#include "writer.h"
#include "utils.h"

int processLinks(char *file_name)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat link_stat, file_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);

    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, nume_intare);
    free(nume_intare);

    if (lstat(path, &link_stat) < 0)
    {
        perror("Eroare citire informatii legatura!");
        exit(EXIT_FAILURE);
    }

    if (stat(path, &file_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume legatura: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", link_stat.st_uid);
    dprintf(sfd, "dimensiune legatura: %ld\n", link_stat.st_size);
    dprintf(sfd, "dimensiune fisier: %ld\n", file_stat.st_size);
    writePermissionLink(sfd, link_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }

    return 7;
}