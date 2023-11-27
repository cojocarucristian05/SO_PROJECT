#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include "process_bmp.h"
#include "writer.h"
#include "path.h"
#include "utils.h"

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

    return bmpFormat;
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