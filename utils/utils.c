#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "utils.h"

char* extrageNumeIntrare(const char* numeComplet)
{
    char* nume_intrare = (char*)malloc(strlen(numeComplet) + 1);
    
    if (nume_intrare == NULL)
    {
        perror("Eroare alocare memorie!");
        exit(EXIT_FAILURE);
    }

    char* punct = strrchr(numeComplet, '.');

    if (punct != NULL) 
    {
        strncpy(nume_intrare, numeComplet, punct - numeComplet);
        nume_intrare[punct - numeComplet] = '\0';
    } 
    else 
    {
        strcpy(nume_intrare, numeComplet);
    }

    return nume_intrare;
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