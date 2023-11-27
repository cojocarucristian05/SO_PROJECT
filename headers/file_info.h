#ifndef FILE_INFO_H
#define FILE_INFO_H

// tipuri de fisiere posibile
typedef enum type
{
    image,
    reg_file,
    slink,
    dir
} Type;

// informatii despre fisier
typedef struct file_info
{
    char file_name[PATH_MAX];       // nume
    Type type;                      // tip
} FileInfo;

#endif