// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include "writer.h"
// #include "process_bmp.h"

// #define RED_PROPORTION 0.299
// #define GREEN_PROPORTION 0.587
// #define BLUE_PROPORTION 0.114

// // functie care modifica valoarea pixelilor conform proportilor
// void convertToGrayScale(BmpFormat *bmpFormat) 
// {
//     int size = bmpFormat->infoHeader.width * bmpFormat->infoHeader.height;
//     for (int i = 0; i < size; i++) {
//         bmpFormat->colorTable[i].red = (char)(bmpFormat->colorTable[i].red * RED_PROPORTION);
//         bmpFormat->colorTable[i].green = (char)(bmpFormat->colorTable[i].green * GREEN_PROPORTION);
//         bmpFormat->colorTable[i].blue = (char)(bmpFormat->colorTable[i].blue * BLUE_PROPORTION);
//     }
// }

// // functie care proceseaza imaginea (citeste date, proceseaza pixeli, scrie date)
// void processImage(int sfd, char *file_name, struct stat image_stat, int *nrLinii)
// {   
//     // // char path[PATH_MAX];
//     // int imageFileDescriptor = 0;
//     // BmpFormat bmpFormat;
//     // int colorTableSize = 0;

//     // sprintf(path, "./dir/%s", file_name);
    
//     // // deschidere fisier
//     // imageFileDescriptor = open(path, O_RDWR);
//     // if (imageFileDescriptor < 0)
//     // {
//     //     perror("Eroare deschidere fisier intrare!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // citire header
//     // if (read(imageFileDescriptor, &bmpFormat.header, sizeof(Header)) != sizeof(Header))
//     // {
//     //     perror("Eroare citire header!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // citire info header
//     // if (read(imageFileDescriptor, &bmpFormat.infoHeader, sizeof(InfoHeader)) != sizeof(InfoHeader))
//     // {
//     //     perror("Eroare citire info header!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // alocare memorie tablou pixeli
//     // colorTableSize = bmpFormat.infoHeader.width * bmpFormat.infoHeader.height;
//     // bmpFormat.colorTable = (ColorTable *)malloc(colorTableSize * sizeof(ColorTable));
//     // if (bmpFormat.colorTable == NULL) 
//     // {
//     //     perror("Eroare alocare memorie!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // citire color table
//     // if (read(imageFileDescriptor, bmpFormat.colorTable, sizeof(ColorTable) * colorTableSize) != sizeof(ColorTable) * colorTableSize)
//     // {
//     //     perror("Eroare citire color table!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // modificare pixeli
//     // convertToGrayScale(&bmpFormat);

//     // // mutam cursorul inapoi la pozitia de inceput a tabloului
//     // if (lseek(imageFileDescriptor, bmpFormat.header.dataOffset, SEEK_SET) < 0)
//     // {
//     //     perror("Eroare setare cursor!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // scriere date, in fisierul bmp suprascriem valoarea pixelilor originali cu cei modificati
//     // if (write(imageFileDescriptor, bmpFormat.colorTable, sizeof(ColorTable) * colorTableSize) != sizeof(ColorTable) * colorTableSize)
//     // {
//     //     perror("Eroare scriere pixeli modificati!");
//     //     exit(EXIT_FAILURE);
//     // }

//     // // scriere statistici imagine
//     // // writeImageStatistics(sfd, file_name, bmpFormat, image_stat, nrLinii);    

//     // // inchidere fisier
//     // if (close(imageFileDescriptor) < 0)
//     // {
//     //     perror("Eroare inchidere fisier intrare!");
//     //     exit(EXIT_FAILURE);
//     // }

// }