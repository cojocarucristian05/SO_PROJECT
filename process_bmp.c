#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define EXPECTED_NUMBER_OF_ARGS 2

typedef struct header
{
    char signature[16];
    int fileSize;
    int reserved;
    int dataOffset;
} Header;

typedef struct info_header
{
    int size;
    int width;
    int height;
} InfoHeader;

typedef struct bmp_format
{
    Header header;
    InfoHeader infoHeader;
} BmpFormat;

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
    sprintf(command, "./script.sh ./%s", argv[1]);
    if(system(command) != 0)
    {
       printf("Usage %s %s\n", argv[0], argv[1]);
       exit(2);
    }
}

const char* permissionToSstring(mode_t permission) 
{
    static char buf[4];
    buf[0] = (permission & S_IRUSR) ? 'r' : '-';
    buf[1] = (permission & S_IWUSR) ? 'w' : '-';
    buf[2] = (permission & S_IXUSR) ? 'x' : '-';
    buf[3] = '\0';
    return buf;
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

void writeStatistics(char *image_file, BmpFormat bmpFormat, struct stat image_stat)
{
    int sfd = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    dprintf(sfd, "nume fisier: %s\n", image_file);
    dprintf(sfd, "inaltime: %d\n", bmpFormat.infoHeader.height);
    dprintf(sfd, "lungime: %d\n", bmpFormat.infoHeader.width);
    dprintf(sfd, "dimensiune: %d\n", bmpFormat.header.fileSize);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", image_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(image_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", image_stat.st_nlink);
    dprintf(sfd, "drepturi de acces user: %s\n", permissionToSstring(image_stat.st_mode & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", permissionToSstring(image_stat.st_mode & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n", permissionToSstring(image_stat.st_mode & S_IRWXO));

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

    writeStatistics(image_file, bmpFormat, image_stat);

    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier intrare!");
        exit(10);
    }

}

int main(int argc, char *argv[])
{
    checkNumberOfArgs(argc);
    checkTypeForArgument(argv);
    processImage(argv[1]);
    return 0;
}