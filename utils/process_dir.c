#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "writer.h"
#include "process_dir.h"
#include "process_bmp.h"
#include "path.h"
#include "../headers/bmp.h"
#include "../headers/file_info.h"
#include "process_bmp.h"
#include "process_regular_file.h"
#include "process_links.h"
#include "process_directory.h"

void processDIR(char *din_path, char *dout_path)
{
    DIR *dir1 = NULL, *dir2 = NULL;
    struct dirent *dirent1 = NULL;

    dir1 = opendir(din_path);
    if (dir1 == NULL)
    {
        perror("Eroare deschidere director intrare!\n");
        exit(EXIT_FAILURE);
    }
    
    FileInfo *files = NULL;
    int countEntries = 0;
    while((dirent1 = readdir(dir1)) != NULL)
    {
        if (strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0)
            continue;
        if (dirent1->d_type == DT_DIR || dirent1->d_type == DT_LNK || dirent1->d_type == DT_REG) 
        {
            files = (FileInfo*)realloc(files, (countEntries + 1) * sizeof(FileInfo));
            if (files == NULL) 
            {
                perror("Eroare alocare memorie");
                exit(EXIT_FAILURE);
            }
            strcpy(files[countEntries].file_name, dirent1->d_name);
            if (dirent1->d_type == DT_DIR) files[countEntries++].type = dir;
            if (dirent1->d_type == DT_LNK) files[countEntries++].type = slink;
            if (dirent1->d_type == DT_REG)
            {
                char command[300];
                sprintf(command, "%s %s", BMP_REGEX_SCRIPT_PATH, dirent1->d_name);
                if(system(command) != 0)
                    files[countEntries++].type = reg_file;
                else
                    files[countEntries++].type = image;
            }
        }
    
    }

    if (closedir(dir1) < 0)
    {
        free(files);
        perror("Eroare inchidere director intrare!");
        exit(EXIT_FAILURE);
    }

    dir2 = opendir(dout_path);
    if (dir2 == NULL)
    {
        perror("Eroare deschidere director iesire!\n");
        exit(EXIT_FAILURE);
    }

    pid_t *pid = NULL, wpid;
    int processCount = 0;

    for(int i = 0; i < countEntries; i++)
    {
        if (files[i].type == dir || files[i].type == slink || files[i].type == reg_file) {
            pid = realloc(pid, (processCount + 1) * sizeof(pid_t));
            if (pid == NULL) {
                perror("Eroare realocare memorie procese\n");
                exit(1);
            }

            if ((pid[processCount++] = fork()) < 0) {
                perror("Eroare creare proces\n");
                exit(1);
            }

            if (pid[processCount - 1] == 0) {
                int nr = 0;
                if (files[i].type == dir) 
                {
                    nr = processDirectory(files[i].file_name);
                }
                if (files[i].type == slink) 
                {
                    nr = processLinks(files[i].file_name);
                }
                if (files[i].type == reg_file) {
                    nr = processRegularFile(files[i].file_name);
                
                }
                exit(nr);
            }
        }
        else if (files[i].type == image)
        {
            int nr = 0;
            pid = realloc(pid, (processCount + 2) * sizeof(pid_t));
            if (pid == NULL) {
                perror("Eroare realocare memorie procese\n");
                exit(1);
            }

            for (int j = 0; j < 2; j++) {
                if ((pid[processCount++] = fork()) < 0) {
                    perror("Eroare creare proces\n");
                    exit(1);
                }

                if (pid[processCount - 1] == 0) {
                    if (j == 0)
                        nr = processImage1(files[i].file_name);
                    else
                        processImage2(files[i].file_name);
                    exit(nr);
                }
            }
        }
    }

    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    int status;
    for (int i = 0; i < processCount; i++) {
        wpid = wait(&status);
        if (WIFEXITED(status))
            printf("Procesul fiu %d s-a terminat cu codul %d\n", wpid, WEXITSTATUS(status));
        else
            printf("Procesul fiu %d s-a terminat anormal\n", wpid);

        if (WEXITSTATUS(status) > 0)
        {
            dprintf(sfd, "%d\n", WEXITSTATUS(status));
        }
    }

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }

    free(pid);
    free(files);
    
    if (closedir(dir2) < 0)
    {
        perror("Eroare inchidere director iesire!");
        exit(EXIT_FAILURE);
    }

}