#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include "utils.h"

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

const char* permissionToString(mode_t permission) 
{
    static char buf[4];
    buf[0] = (permission & S_IRUSR) ? 'r' : '-';
    buf[1] = (permission & S_IWUSR) ? 'w' : '-';
    buf[2] = (permission & S_IXUSR) ? 'x' : '-';
    buf[3] = '\0';
    return buf;
}