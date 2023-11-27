#ifndef BMP_H
#define BMP_H

#pragma pack(2) // aliniem structura in memorie

typedef struct header 
{
    char signature[2];
    int fileSize;
    int reserved;
    int dataOffset;
} Header;

typedef struct info_header 
{
    int size;
    int width;
    int height;
    short planes;
    short bitCount;
    int compression;
    int imageSize;
    int xPixelsPerM;
    int yPixelsPerM;
    int colorsUsed;
    int colorsImportant;
} InfoHeader;

typedef struct color_table 
{
    char red;
    char green;
    char blue;
} ColorTable;

typedef struct bmp_format 
{
    Header header;
    InfoHeader infoHeader;
    ColorTable *colorTable;
} BmpFormat;

#endif