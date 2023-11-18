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

char* extrageNumeIntrare(const char* numeComplet) {
    char* nume_intrare = (char*)malloc(strlen(numeComplet) + 1);
    
    if (nume_intrare == NULL) {
        fprintf(stderr, "Eroare de alocare memorie\n");
        return NULL;
    }

    char* punct = strrchr(numeComplet, '.');

    if (punct != NULL) {
        strncpy(nume_intrare, numeComplet, punct - numeComplet);
        nume_intrare[punct - numeComplet] = '\0';
    } else {
        strcpy(nume_intrare, numeComplet);
    }

    return nume_intrare;
}

void processLinks(char *file_name)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat link_stat;     
    char *aux = extrageNumeIntrare(file_name);
    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, aux);
    if (lstat(path, &link_stat) < 0)
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

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void processDirectory(char *file_name)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat dir_stat;     
    char *aux = extrageNumeIntrare(file_name);

    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, aux);
    if (stat(path, &dir_stat) < 0)
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

    dprintf(sfd, "nume director: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", dir_stat.st_uid);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void processRegularFile(char *file_name)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat file_stat;     
    char *aux = extrageNumeIntrare(file_name);

    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, aux);
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

    dprintf(sfd, "nume fisier: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", file_stat.st_uid);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void processImage1(char *file_name)
{
    printf("Image writer\n");
}

void processImage2(char *file_name)
{
    printf("Image modifier\n");
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
                if (files[i].type == dir) processDirectory(files[i].file_name);
                if (files[i].type == slink) processLinks(files[i].file_name);
                if (files[i].type == reg_file) processRegularFile(files[i].file_name);
                exit(0);
            }
        }
        else if (files[i].type == image)
        {
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
                        processImage1(files[i].file_name);
                    else
                        processImage2(files[i].file_name);
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
    
    if (closedir(dir2) < 0)
    {
        perror("Eroare inchidere director iesire!");
        exit(EXIT_FAILURE);
    }

}