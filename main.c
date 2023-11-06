#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "./utils/utils.h"
#include "bmp.h"

#define EXPECTED_NUMBER_OF_ARGS 2

void checkNumberOfArgs(int argc)
{
    if (argc != EXPECTED_NUMBER_OF_ARGS)
    {
        perror("Invalid number of args!");
        exit(1);
    }
}

void checkTypeForArgument(char *argv[])
{
    char command[256];
    sprintf(command, "./scripts/check_argument_type.sh %s", argv[1]);
    if(system(command) != 0)
    {
       printf("Usage %s %s\n", argv[0], argv[1]);
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
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(image_stat.st_mode & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(image_stat.st_mode & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(image_stat.st_mode & S_IRWXO));

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }

}

void processImage(char *image_file)
{
    int imageFileDescriptor = open(image_file, O_RDONLY);
    if (imageFileDescriptor < 0)
    {
        perror("Eroare deschidere fisier intrare!");
        exit(1);
    }

    BmpFormat bmpFormat;
    if (read(imageFileDescriptor, bmpFormat.header.signature, 2) != 2)
    {
        perror("Eroare citire semnatura header!");
        exit(2);
    }

    if (read(imageFileDescriptor, &bmpFormat.header.fileSize, 4) != 4)
    {
        perror("Eroare citire fileSize header!");
        exit(3);
    }

    if (read(imageFileDescriptor, &bmpFormat.header.reserved, 4) != 4)
    {
        perror("Eroare citire reserved header!");
        exit(4);
    }

    if (read(imageFileDescriptor, &bmpFormat.header.dataOffset, 4) != 4)
    {
        perror("Eroare citire dataOffset header!");
        exit(5);
    }

    if (read(imageFileDescriptor, &bmpFormat.infoHeader.size, 4) != 4)
    {
        perror("Eroare citire size of infoheader!");
        exit(6);
    }

    if (read(imageFileDescriptor, &bmpFormat.infoHeader.width, 4) != 4)
    {
        perror("Eroare citire size of infoheader!");
        exit(7);
    }

    if (read(imageFileDescriptor, &bmpFormat.infoHeader.height, 4) != 4)
    {
        perror("Eroare citire size of infoheader!");
        exit(8);
    }

    struct stat image_stat;
    if (stat(image_file, &image_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(9);
    }

    writeImageStatistics(image_file, bmpFormat, image_stat);    

    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier intrare!");
        exit(10);
    }

}

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
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(dir_stat.st_mode & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(dir_stat.st_mode & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(dir_stat.st_mode & S_IRWXO));

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
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(link_stat.st_mode & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(link_stat.st_mode & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(link_stat.st_mode & S_IRWXO));

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void writeRegularFileStatistics(char *image_file, struct stat image_stat)
{
    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume fisier: %s\n", image_file);
    dprintf(sfd, "dimensiune: %ld\n", image_stat.st_size);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", image_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(image_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", image_stat.st_nlink);
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(image_stat.st_mode & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(image_stat.st_mode & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(image_stat.st_mode & S_IRWXO));

    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(2);
    }
}

void processDIR(char *dir_path)
{
    DIR *dir = NULL;
    struct dirent *dirent1 = NULL;
    char path[PATH_MAX];
    struct stat file_stat;

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
        sprintf(path, "./dir/%s", dirent1->d_name);
        if (dirent1->d_type == DT_DIR)
        {
            if (lstat(path, &file_stat) < 0)
            {
                perror("Eroare citire informatii fisier!");
                exit(EXIT_FAILURE);
            }
            writeDirStatistics(dirent1, file_stat);
        }
        else if (dirent1->d_type == DT_LNK)
        {
            if (stat(path, &file_stat) < 0)
            {
                perror("Eroare citire informatii fisier!");
                exit(EXIT_FAILURE);
            }
            writeLinkStatistics(dirent1, file_stat);
        }
        else if (dirent1->d_type == DT_REG)
        {
            char command[300];
            if (stat(path, &file_stat) < 0)
            {
                perror("Eroare citire informatii fisier!");
                exit(EXIT_FAILURE);
            }
            sprintf(command, "./scripts/bmp_regex.sh %s", dirent1->d_name);
            if(system(command) != 0)
            {
                writeRegularFileStatistics(dirent1->d_name, file_stat);
            }
            else
            {
                processImage(path);
            }
        }
    }

    if (closedir(dir) < 0)
    {
        perror("Eroare inchidere director!");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char *argv[])
{
    checkNumberOfArgs(argc);
    checkTypeForArgument(argv);
    processDIR(argv[1]);
    return 0;
}