#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "script_executor.h"
#include "writer.h"
#include "process_dir.h"

void processDirent(struct dirent *dirent1)
{
    char path[PATH_MAX];
    struct stat file_stat;        
    sprintf(path, "./dir/%s", dirent1->d_name);
    if (stat(path, &file_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }
    
    if (dirent1->d_type == DT_DIR) writeDirStatistics(dirent1, file_stat);
    if (dirent1->d_type == DT_LNK) 
    {
        struct stat link_stat;
        if (lstat(path, &link_stat) < 0)
        {
            perror("Eroare citire informatii fisier!");
            exit(EXIT_FAILURE);
        }
        writeLinkStatistics(dirent1, link_stat, file_stat);
    }
    if (dirent1->d_type == DT_REG) processFile(dirent1->d_name, file_stat);
}

void processDIR(char *dir_path)
{
    DIR *dir = NULL;
    struct dirent *dirent1 = NULL;

    dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("Eroare deschidere director!\n");
        exit(EXIT_FAILURE);
    }

    while((dirent1 = readdir(dir)) != NULL)
    {
        if (strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0)
            continue;
        processDirent(dirent1);
    }

    if (closedir(dir) < 0)
    {
        perror("Eroare inchidere director!");
        exit(EXIT_FAILURE);
    }

}