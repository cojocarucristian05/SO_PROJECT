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
// #include "script_executor.h"
#include "writer.h"
#include "process_dir.h"
#include "process_bmp.h"

#define DIR_PATH "./dir/"
#define OUTPUT_DIR_PATH "./dir_iesire/"
#define STAT_FILE_PATH "statistica.txt"
#define BMP_REGEX_SCRIPT_PATH "./scripts/bmp_regex.sh"

typedef enum type
{
    image,
    reg_file,
    slink,
    dir
} Type;

typedef struct file_info
{
    char file_name[PATH_MAX];
    Type type;
} FileInfo;

void processDirent(struct dirent *dirent1)
{
    pid_t pid;
    char path[PATH_MAX];
    struct stat file_stat; 
    int sfd;
    char outputFilePath[PATH_MAX];       
    int numarLiniiScrise = 0;

    sprintf(path, "%s%s", DIR_PATH, dirent1->d_name);
    if (stat(path, &file_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork() < 0))
    {
        perror("Eroare creare proces!");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, dirent1->d_name);
        printf("%s\n", outputFilePath);
        
        sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);
        
        if (sfd < 0)
        {
            perror("Eroare deschidere fisier iesire!");
            exit(EXIT_FAILURE);
        }

        if (dirent1->d_type == DT_DIR)
        {
            writeDirStatistics(sfd, dirent1, file_stat, &numarLiniiScrise);
        }
        else if (dirent1->d_type == DT_LNK)
        {
            struct stat link_stat;
            if (lstat(path, &link_stat) < 0)
            {
                perror("Eroare citire informatii legatura simbolica!");
                exit(EXIT_FAILURE);
            }
            writeLinkStatistics(sfd, dirent1, link_stat, file_stat, &numarLiniiScrise);
        }
        else if (dirent1->d_type == DT_REG)
        {
            char command[300];
            sprintf(command, "%s %s", BMP_REGEX_SCRIPT_PATH, dirent1->d_name);
            if(system(command) != 0)
            {
                writeRegularFileStatistics(sfd, dirent1->d_name, file_stat, &numarLiniiScrise);
            }
            else
            {
                processImage(sfd, dirent1->d_name, file_stat, &numarLiniiScrise);
            }
        }
        numarLiniiScrise = 20;
        printf("Nr linii: %d\n", numarLiniiScrise);
        exit(numarLiniiScrise);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);

        // int sfd = open(STAT_FILE_PATH, O_APPEND | O_WRONLY | O_CREAT,  0666);
        // if (sfd < 0)
        // {
        //     perror("Eroare deschidere fisier statistica!");
        //     exit(EXIT_FAILURE);
        // }

        // printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", pid, WEXITSTATUS(status));
        // dprintf(sfd, "%d\n", WEXITSTATUS(status));
        
        // if (close(sfd) < 0)
        // {
        //     perror("Eroare inchidere fisier statistica proces parinte!");
        //     exit(EXIT_FAILURE);
        // }
        printf("pid: %d %d\n", getpid(), getppid());
    }
}

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
    
    dir2 = opendir(dout_path);
    if (dir2 == NULL)
    {
        perror("Eroare deschidere director iesire!\n");
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

    pid_t *pid = NULL, wpid;
    int processCount = 0;

    for(int i = 0; i < countEntries; i++)
    {
        // printf("name: %s type: %d\n", files[i].file_name, files[i].type);
        if (files[i].type == dir || files[i].type == slink || files[i].type == reg_file) {
            pid = realloc(pid, (processCount + 1) * sizeof(pid_t));
            if (pid == NULL) {
                perror("Eroare realocare memorie\n");
                exit(1);
            }

            if ((pid[processCount++] = fork()) < 0) {
                perror("Eroare creare proces\n");
                exit(1);
            }

            if (pid[processCount - 1] == 0) {
                if (files[i].type == dir) printf("DIR\n");
                if (files[i].type == slink) printf("SLINK\n");
                if (files[i].type == reg_file) printf("RFILE\n");
                exit(0);
            }
        }
        else if (files[i].type == image)
        {
            pid = realloc(pid, (processCount + 2) * sizeof(pid_t));
            if (pid == NULL) {
                perror("Eroare realocare memorie\n");
                exit(1);
            }

            for (int j = 0; j < 2; j++) {
                if ((pid[processCount++] = fork()) < 0) {
                    perror("Eroare creare proces\n");
                    exit(1);
                }

                if (pid[processCount - 1] == 0) {
                    if (j == 0)
                        printf("Image writer\n");
                    else
                        printf("Image modifier\n");
                    exit(0);
                }
            }
        }
    }

    printf("Procese pornite:\n");
    for (int i = 0; i < processCount; i++)
        printf("%d ", pid[i]);
    printf("\n");
    int status;
    for (int i = 0; i < processCount; i++) {
        wpid = wait(&status);
        if (WIFEXITED(status))
            printf("\nProcesul fiu %d s-a terminat cu codul %d\n", wpid, WEXITSTATUS(status));
        else
            printf("\nProcesul fiu %d s-a terminat anormal\n", wpid);
    }

    free(pid);
    free(files);

    if (closedir(dir1) < 0)
    {
        perror("Eroare inchidere director intrare!");
        exit(EXIT_FAILURE);
    }

    if (closedir(dir2) < 0)
    {
        perror("Eroare inchidere director iesire!");
        exit(EXIT_FAILURE);
    }

}