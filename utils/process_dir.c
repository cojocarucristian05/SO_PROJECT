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
#include "../headers/bmp.h"

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

void writePermission(int sfd, mode_t permission)
{
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(permission & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(permission & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(permission & S_IRWXO));
}

void writePermissionLink(int sfd, mode_t permission)
{
    dprintf(sfd, "drepturi de acces user legatura: %s\n", userPermissionToString(permission & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup legatura: %s\n", groupPermissionToString(permission & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii legatura: %s\n\n", otherPermissionToString(permission & S_IRWXO));
}

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

int processDirectory(char *file_name)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat dir_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);

    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, nume_intare);
    free(nume_intare);

    if (stat(path, &dir_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(EXIT_FAILURE);
    }

    dprintf(sfd, "nume director: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", dir_stat.st_uid);
    writePermission(sfd, dir_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }

    return 5;
}

int processRegularFile(char *file_name)
{
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat file_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);

    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, nume_intare);
    free(nume_intare);

    if (stat(path, &file_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(EXIT_FAILURE);
    }

    dprintf(sfd, "nume fisier: %s\n", file_name);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(file_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", file_stat.st_nlink);
    writePermission(sfd, file_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }

    return 8;
}

#define RED_PROPORTION 0.299
#define GREEN_PROPORTION 0.587
#define BLUE_PROPORTION 0.114

// functie care modifica valoarea pixelilor conform proportilor
void convertToGrayScale(BmpFormat *bmpFormat) 
{
    int size = bmpFormat->infoHeader.width * bmpFormat->infoHeader.height;
    for (int i = 0; i < size; i++) {
        bmpFormat->colorTable[i].red = (char)(bmpFormat->colorTable[i].red * RED_PROPORTION);
        bmpFormat->colorTable[i].green = (char)(bmpFormat->colorTable[i].green * GREEN_PROPORTION);
        bmpFormat->colorTable[i].blue = (char)(bmpFormat->colorTable[i].blue * BLUE_PROPORTION);
    }
}

BmpFormat* processImage(char *file_name)
{   
    char path[PATH_MAX];
    int imageFileDescriptor = 0;
    BmpFormat* bmpFormat = malloc(sizeof(BmpFormat)); // Allocate memory for the BmpFormat structure

    if (bmpFormat == NULL) 
    {
        perror("Eroare alocare memorie!");
        exit(EXIT_FAILURE);
    }

    sprintf(path, "%s%s", DIR_PATH, file_name);
    
    // deschidere fisier
    imageFileDescriptor = open(path, O_RDWR);
    if (imageFileDescriptor < 0)
    {
        perror("Eroare deschidere fisier intrare!");
        exit(EXIT_FAILURE);
    }

    // citire header
    if (read(imageFileDescriptor, &bmpFormat->header, sizeof(Header)) != sizeof(Header))
    {
        perror("Eroare citire header!");
        exit(EXIT_FAILURE);
    }

    // citire info header
    if (read(imageFileDescriptor, &bmpFormat->infoHeader, sizeof(InfoHeader)) != sizeof(InfoHeader))
    {
        perror("Eroare citire info header!");
        exit(EXIT_FAILURE);
    }

    // alocare memorie tablou pixeli
    int colorTableSize = bmpFormat->infoHeader.width * bmpFormat->infoHeader.height;
    bmpFormat->colorTable = (ColorTable *)malloc(colorTableSize * sizeof(ColorTable));
    if (bmpFormat->colorTable == NULL) 
    {
        perror("Eroare alocare memorie!");
        exit(EXIT_FAILURE);
    }

    // citire color table
    if (read(imageFileDescriptor, bmpFormat->colorTable, sizeof(ColorTable) * colorTableSize) != sizeof(ColorTable) * colorTableSize)
    {
        perror("Eroare citire color table!");
        exit(EXIT_FAILURE);
    }

    // Close the file
    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier intrare!");
        exit(EXIT_FAILURE);
    }

    return bmpFormat; // Return the dynamically allocated BmpFormat structure
}


int processImage1(char *file_name)
{
    BmpFormat *bmp_format = processImage(file_name);
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat image_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);

    sprintf(path, "%s%s", DIR_PATH, file_name);
    sprintf(outputFilePath, "%s%s_statistica.txt", OUTPUT_DIR_PATH, nume_intare);
    free(nume_intare);

    if (stat(path, &image_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(EXIT_FAILURE);
    }

    dprintf(sfd, "nume fisier: %s\n", file_name);
    dprintf(sfd, "inaltime: %d\n", bmp_format->infoHeader.height);
    dprintf(sfd, "lungime: %d\n", bmp_format->infoHeader.width);
    dprintf(sfd, "dimensiune: %ld\n", image_stat.st_size);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", image_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(image_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", image_stat.st_nlink);
    writePermission(sfd, image_stat.st_mode);

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }
    free(bmp_format);

    return 10;
}

void processImage2(char *file_name)
{
    char path[PATH_MAX];
    int imageFileDescriptor = 0;
    BmpFormat *bmp_format = processImage(file_name);
    int colorTableSize = bmp_format->infoHeader.width * bmp_format->infoHeader.height;

    sprintf(path, "%s%s", DIR_PATH, file_name);
    imageFileDescriptor = open(path, O_RDWR);
    if (imageFileDescriptor < 0)
    {
        perror("Eroare deschidere fisier bmp pentru modificare pixeli!");
        exit(EXIT_FAILURE);
    }

    convertToGrayScale(bmp_format);

    // mutam cursorul inapoi la pozitia de inceput a tabloului
    if (lseek(imageFileDescriptor, bmp_format->header.dataOffset, SEEK_SET) < 0)
    {
        perror("Eroare setare cursor!");
        exit(EXIT_FAILURE);
    }

    // scriere date, in fisierul bmp suprascriem valoarea pixelilor originali cu cei modificati
    if (write(imageFileDescriptor, bmp_format->colorTable, sizeof(ColorTable) * colorTableSize) != sizeof(ColorTable) * colorTableSize)
    {
        perror("Eroare scriere pixeli modificati!");
        exit(EXIT_FAILURE);
    }

    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier bmp dupa prelucrare pixeli!");
        exit(EXIT_FAILURE);
    }

    free(bmp_format);
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