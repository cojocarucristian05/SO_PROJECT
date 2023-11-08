#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "writer.h"
#include "process_bmp.h"

void processImage(char *image_file)
{
    int imageFileDescriptor = open(image_file, O_RDONLY);
    if (imageFileDescriptor < 0)
    {
        perror("Eroare deschidere fisier intrare!");
        exit(EXIT_FAILURE);
    }

    BmpFormat bmpFormat;
    if (read(imageFileDescriptor, bmpFormat.header.signature, 2) != 2)
    {
        perror("Eroare citire semnatura header!");
        exit(EXIT_FAILURE);
    }

    if (read(imageFileDescriptor, &bmpFormat.header.fileSize, 4) != 4)
    {
        perror("Eroare citire fileSize header!");
        exit(EXIT_FAILURE);
    }

    if (read(imageFileDescriptor, &bmpFormat.header.reserved, 4) != 4)
    {
        perror("Eroare citire reserved header!");
        exit(EXIT_FAILURE);
    }

    if (read(imageFileDescriptor, &bmpFormat.header.dataOffset, 4) != 4)
    {
        perror("Eroare citire dataOffset header!");
        exit(EXIT_FAILURE);
    }

    if (read(imageFileDescriptor, &bmpFormat.infoHeader.size, 4) != 4)
    {
        perror("Eroare citire size of infoheader!");
        exit(EXIT_FAILURE);
    }

    if (read(imageFileDescriptor, &bmpFormat.infoHeader.width, 4) != 4)
    {
        perror("Eroare citire size of infoheader!");
        exit(EXIT_FAILURE);
    }

    if (read(imageFileDescriptor, &bmpFormat.infoHeader.height, 4) != 4)
    {
        perror("Eroare citire size of infoheader!");
        exit(EXIT_FAILURE);
    }

    struct stat image_stat;
    if (stat(image_file, &image_stat) < 0)
    {
        perror("Eroare citire informatii fisier!");
        exit(EXIT_FAILURE);
    }

    writeImageStatistics(image_file, bmpFormat, image_stat);    

    if (close(imageFileDescriptor) < 0)
    {
        perror("Eroare inchidere fisier intrare!");
        exit(EXIT_FAILURE);
    }

}