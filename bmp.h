#ifndef BMP_H
#define BMP_H

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

#endif