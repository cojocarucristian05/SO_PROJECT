#ifndef PRINTER_H
#define PRINTER_H
#include "../headers/bmp.h"

void writeRegularFileStatistics(char *image_file, struct stat image_stat);
void writeLinkStatistics(struct dirent *dirent1, struct stat link_stat);
void writeDirStatistics(struct dirent *dirent1,  struct stat dir_stat);
void writeImageStatistics(char *image_file, BmpFormat bmpFormat, struct stat image_stat);

#endif