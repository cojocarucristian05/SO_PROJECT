#ifndef PROCESS_REGULAR_FILE_H
#define PROCESS_REGULAR_FILE_H

int processRegularFile(char *file_name, int pipe1[1]);
void processFileContent(int pipe1[2], int pipe2[2]);

#endif