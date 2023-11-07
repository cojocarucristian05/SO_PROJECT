#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include "writer.h"

const char* getLastModified(struct timespec st_mtim);
const char* userPermissionToString(mode_t permission);
const char* groupPermissionToString(mode_t permission);
const char* otherPermissionToString(mode_t permission);
void writePermission(int sfd, mode_t permission);

void writeDirStatistics(struct dirent *dirent1,  struct stat dir_stat)
{
    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume director: %s\n", dirent1->d_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", dir_stat.st_uid);
    writePermission(sfd, dir_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void writeLinkStatistics(struct dirent *dirent1, struct stat link_stat)
{
    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume legatura: %s\n", dirent1->d_name);
    dprintf(sfd, "dimensiune: %ld\n", link_stat.st_size);
    writePermission(sfd, link_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void writeRegularFileStatistics(char *regular_file, struct stat regular_file_stat)
{
    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume fisier: %s\n", regular_file);
    dprintf(sfd, "dimensiune: %ld\n", regular_file_stat.st_size);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", regular_file_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(regular_file_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", regular_file_stat.st_nlink);
    writePermission(sfd, regular_file_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void writeImageStatistics(char *image_file, BmpFormat bmpFormat, struct stat image_stat)
{
    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume fisier: %s\n", image_file);
    dprintf(sfd, "inaltime: %d\n", bmpFormat.infoHeader.height);
    dprintf(sfd, "lungime: %d\n", bmpFormat.infoHeader.width);
    dprintf(sfd, "dimensiune: %ld\n", image_stat.st_size);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", image_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(image_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", image_stat.st_nlink);
    writePermission(sfd, image_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }

}


const char* getLastModified(struct timespec st_mtim) 
{
    static char buf[200];
    struct tm t;
    tzset();
    if (localtime_r(&(st_mtim.tv_sec), &t) == NULL)
    {
        perror("Eroare!");
        exit(2);
    }
    sprintf(buf, "%d.%d.%d", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
    return buf;
}

const char* userPermissionToString(mode_t permission) 
{
    static char buf[4];
    buf[0] = (permission & S_IRUSR) ? 'r' : '-';
    buf[1] = (permission & S_IWUSR) ? 'w' : '-';
    buf[2] = (permission & S_IXUSR) ? 'x' : '-';
    buf[3] = '\0';
    return buf;
}

const char* groupPermissionToString(mode_t permission) 
{
    static char buf[4];
    buf[0] = (permission & S_IRGRP) ? 'r' : '-';
    buf[1] = (permission & S_IWGRP) ? 'w' : '-';
    buf[2] = (permission & S_IXGRP) ? 'x' : '-';
    buf[3] = '\0';
    return buf;
}

const char* otherPermissionToString(mode_t permission) 
{
    static char buf[4];
    buf[0] = (permission & S_IROTH) ? 'r' : '-';
    buf[1] = (permission & S_IWOTH) ? 'w' : '-';
    buf[2] = (permission & S_IXOTH) ? 'x' : '-';
    buf[3] = '\0';
    return buf;
}

void writePermission(int sfd, mode_t permission)
{
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(permission & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(permission & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(permission & S_IRWXO));
}