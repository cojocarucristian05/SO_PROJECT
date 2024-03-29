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
        int grayValue = (char)(bmpFormat->rasterData[i].red * RED_PROPORTION) +
                        (char)(bmpFormat->rasterData[i].green * GREEN_PROPORTION) +
                        (char)(bmpFormat->rasterData[i].blue * BLUE_PROPORTION);
        bmpFormat->rasterData[i].red = grayValue;
        bmpFormat->rasterData[i].green = grayValue;
        bmpFormat->rasterData[i].blue = grayValue;
    }
}

// functie care parcurge fisierul imagine si retine intr-o structura conform header-ului informatii despre imagine
BmpFormat* processImage(char *file_name, char *din_path)
{   
    char path[PATH_MAX];
    int imageFileDescriptor = 0;
    BmpFormat* bmpFormat = malloc(sizeof(BmpFormat)); // alocare memorie

    if (bmpFormat == NULL) 
    {
        perror("Eroare alocare memorie!");
        exit(EXIT_FAILURE);
    }

    sprintf(path, "%s%s", din_path, file_name);
    
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

    if (bmpFormat->infoHeader.bitCount == 24) 
    {
        // alocare memorie tablou pixeli
        int rasterDataSize = bmpFormat->infoHeader.width * bmpFormat->infoHeader.height;
        bmpFormat->rasterData = (RasterData*)malloc(rasterDataSize * sizeof(RasterData));
        if (bmpFormat->rasterData == NULL) 
        {
            perror("Eroare alocare memorie!");
            exit(EXIT_FAILURE);
        }

        // citire color table
        if (read(imageFileDescriptor, bmpFormat->rasterData, sizeof(RasterData) * rasterDataSize) != sizeof(RasterData) * rasterDataSize)
        {
            perror("Eroare citire color table!");
            exit(EXIT_FAILURE);
        }
    }

    // inchidere fisier
    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier intrare!");
        exit(EXIT_FAILURE);
    }

    return bmpFormat;
}

// functie pentru scrierea informatiilor in fisierul <intrare>_statistica
int processImage1(char *file_name, char *din_path, char *dout_path)
{
    BmpFormat *bmp_format = processImage(file_name, din_path);    // extragem informatiile despre imagine
    int sfd;
    char path[PATH_MAX];
    char outputFilePath[PATH_MAX];
    struct stat image_stat;     
    char *nume_intare = extrageNumeIntrare(file_name);  // extragem numele intrarii curente (eliminam path-ul)

    sprintf(outputFilePath, "%s%s_statistica.txt", dout_path, nume_intare);   // formare path fisier iesire
    free(nume_intare);  // eliberare memorie nume

    sprintf(path, "%s%s", din_path, file_name);         // formare path pentru a deschide fisierul
    // citire info fisier folosinf functia stat
    if (stat(path, &image_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    // deschidere fisire iesire
    sfd = open(outputFilePath, O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(EXIT_FAILURE);
    }

    // scriere date in fisierul de iesire
    dprintf(sfd, "nume fisier: %s\n", file_name);
    dprintf(sfd, "inaltime: %d\n", bmp_format->infoHeader.height);
    dprintf(sfd, "lungime: %d\n", bmp_format->infoHeader.width);
    dprintf(sfd, "dimensiune: %ld\n", image_stat.st_size);
    dprintf(sfd, "identificatorul utilizatorului: %d\n", image_stat.st_uid);
    dprintf(sfd, "timpul ultimei modificari: %s\n", getLastModified(image_stat.st_mtim));
    dprintf(sfd, "contorul de legaturi: %ld\n", image_stat.st_nlink);
    writePermission(sfd, image_stat.st_mode);

    // inchidere fisier iesire
    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }
    free(bmp_format);   // eliberare memorie

    return 10;      // returnam numarul de linii scrise in fisierul de iesire
}

// functie care transforma imaginea in tonuri de gri
void processImage2(char *file_name, char *din_path)
{
    char path[PATH_MAX];
    int imageFileDescriptor = 0;
    BmpFormat *bmp_format = processImage(file_name, din_path);
    int rasterDataSize = bmp_format->infoHeader.width * bmp_format->infoHeader.height;

    sprintf(path, "%s%s", din_path, file_name);     // formare path fisier imagine
    imageFileDescriptor = open(path, O_RDWR);       // deschidere fisier
    if (imageFileDescriptor < 0)
    {
        perror("Eroare deschidere fisier bmp pentru modificare pixeli!");
        exit(EXIT_FAILURE);
    }

    if (bmp_format->infoHeader.bitCount == 24)
    {
        convertToGrayScale(bmp_format);     // transformare pixeli

        // mutam cursorul inapoi la pozitia de inceput a tabloului
        if (lseek(imageFileDescriptor, bmp_format->header.dataOffset, SEEK_SET) < 0)
        {
            perror("Eroare setare cursor!");
            exit(EXIT_FAILURE);
        }

        // scriere date, in fisierul bmp suprascriem valoarea pixelilor originali cu cei modificati
        if (write(imageFileDescriptor, bmp_format->rasterData, sizeof(RasterData) * rasterDataSize) != sizeof(RasterData) * rasterDataSize)
        {
            perror("Eroare scriere pixeli modificati!");
            exit(EXIT_FAILURE);
        }
    }

    // inchidere fisier imagine
    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier bmp dupa prelucrare pixeli!");
        exit(EXIT_FAILURE);
    }

    free(bmp_format);   // eliberare memorie
}