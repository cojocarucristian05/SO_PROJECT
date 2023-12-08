#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "writer.h"
#include "process_dir.h"
#include "process_bmp.h"
#include "path.h"
#include "../headers/bmp.h"
#include "../headers/file_info.h"
#include "process_bmp.h"
#include "process_regular_file.h"
#include "process_links.h"
#include "process_directory.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* functie procesare director intrare */
void processDIR(char *din_path, char *dout_path, char *c)
{
    int suma = 0;
    DIR *dir1 = NULL, *dir2 = NULL;
    struct dirent *dirent1 = NULL;

    dir1 = opendir(din_path);       // deschidere director
    if (dir1 == NULL)
    {
        perror("Eroare deschidere director intrare!\n");
        exit(EXIT_FAILURE);
    }
    
    FileInfo *files = NULL;     // tablou informatii fisiere din directorul de intrare
    int countEntries = 0;       // numar intrari in directorul de intrare
    // parcurgere director
    while((dirent1 = readdir(dir1)) != NULL)
    {
        // daca intrarea curenta este directorul curent sau directorul parinte -> skip
        if (strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0)
            continue;
        /* daca intrarea curenta este director, fisier sau legatura simbolica 
                ->  se creeaza o noua intrare in tablou
        */
        if (dirent1->d_type == DT_DIR || dirent1->d_type == DT_LNK || dirent1->d_type == DT_REG) 
        {
            // realocare memorie
            files = (FileInfo*)realloc(files, (countEntries + 1) * sizeof(FileInfo));
            if (files == NULL) 
            {
                perror("Eroare alocare memorie");
                exit(EXIT_FAILURE);
            }
            strcpy(files[countEntries].file_name, dirent1->d_name);             // copiere nume intrare
            if (dirent1->d_type == DT_DIR) files[countEntries++].type = dir;    // copiere tip pt dir
            if (dirent1->d_type == DT_LNK) files[countEntries++].type = slink;  // copiere tip pt slink
            if (dirent1->d_type == DT_REG)
            {
                char command[300];
                sprintf(command, "%s %s", BMP_REGEX_SCRIPT_PATH, dirent1->d_name);
                if(system(command) != 0)                        // testam daca fisierul este obisnuit sau de tipul bmp
                {
                    files[countEntries++].type = reg_file;      // copiere tip pt regular file
                }
                else
                    files[countEntries++].type = image;         // copiere tip pt bmp
            }
        }
    
    }

    // inchidere director intrare
    if (closedir(dir1) < 0)
    {
        free(files);
        perror("Eroare inchidere director intrare!");
        exit(EXIT_FAILURE);
    }

    // deschidere director iesire
    dir2 = opendir(dout_path);
    if (dir2 == NULL)
    {
        perror("Eroare deschidere director iesire!\n");
        exit(EXIT_FAILURE);
    }

    pid_t *pid = NULL, wpid;        // tablou procese copii, id auxiliar pentru testarea valorii de return
    int processCount = 0;           // numar procese copii
    
    // se creeaza procesele copii in paralel in functie de tipul intrari curente din tablou
    for(int i = 0; i < countEntries; i++)
    {
        // daca nu este de tipul fisier se creeaza un singur proces
        if (files[i].type == dir || files[i].type == slink) {
            pid = realloc(pid, (processCount + 1) * sizeof(pid_t));     // realocare memorie
            if (pid == NULL) {
                perror("Eroare realocare memorie procese\n");
                exit(1);
            }

            // creeare proces copil
            if ((pid[processCount++] = fork()) < 0) {
                perror("Eroare creare proces\n");
                exit(1);
            }

            // daca suntem in codul procesului fiu
            if (pid[processCount - 1] == 0) {
                int nr = 0;             // numarul de linii scrise in fisierul <intrare>_statistica
                if (files[i].type == dir) 
                {
                    nr = processDirectory(files[i].file_name, din_path, dout_path);
                }
                if (files[i].type == slink) 
                {
                    nr = processLinks(files[i].file_name, din_path, dout_path);
                }
                exit(nr);
            }
        }
        else if (files[i].type == reg_file)
        {
            int pipe1[2];   // pipe 1: primul fiu -> al doilea fiu
            int pipe2[2];   // pipe 2: al doilea fiu -> parinte

            // daca este de tipul fisier obisnuit se creeaza 2 procese
            int nr = 0;

            pid = realloc(pid, (processCount + 2) * sizeof(pid_t));     // realocare memorie
            if (pid == NULL) {
                perror("Eroare realocare memorie procese\n");
                exit(1);
            }

            // creeare pipe1
            if (pipe(pipe1) == -1 )
            {
                perror("Eroare creare pipe\n");
                exit(EXIT_FAILURE);
            }

            // Creeaza proces pentru statistica
            if ((pid[processCount++] = fork()) < 0) {
                perror("Eroare creare proces\n");
                exit(EXIT_FAILURE);
            }

            // daca suntem in codul unuia dintre procesele fiu
            if (pid[processCount - 1] == 0) {
                close(pipe1[0]);                                        // inchidem capat citire in procesul fiu
                dup2(pipe1[1], STDOUT_FILENO);                          // redirectionam capatul de scriere al pipe-ului 1 la iesirea standard
                nr = processRegularFile(files[i].file_name, din_path, dout_path, pipe1[1]);  // procesam fisierul (statistica + citire continut)
                close(pipe1[1]);                                        // inchidem capatul de scriere in procesul fiu
                exit(nr);                                               // intoarcem numarul de linii scrise
            }

            close(pipe1[1]);        // inchidem capatul scriere in procesul parinte

            // creeare pipe2
            if (pipe(pipe2) == -1)
            {
                perror("Eroare creare pipe\n");
                exit(EXIT_FAILURE);
            }

            // creeaza proces pentru procesare continut fisier
            if ((pid[processCount++] = fork()) < 0)
            {
                perror("Eroare crealre proces\n");
                exit(EXIT_FAILURE);
            }

            if (pid[processCount - 1] == 0)
            {
                close(pipe2[0]);                // inchidem capatul de citire al pipe-ulu 2 in procesul fiu 2
                dup2(pipe1[0], STDIN_FILENO);   // redirectionam capatul de citire al pipe-ului 1 la intrarea standard
                dup2(pipe2[1], STDOUT_FILENO);  // redirectionam capatul de scriere al pipe-ului 2 la iesirea standard
                // trimitem in executie functia care ruleaza scriptul pentru continutul fisierului curent
                processFileContent(c);
                close(pipe1[0]);            // inchidem capatul de citire al pipe-ului 1 in procesul fiu 2
                close(pipe2[1]);            // inchidem capatul de scriere al pipe-ului 2 in procesul fiu 2
                exit(0);
            }

            // inchidem capetele pe care nu le folosim
            close(pipe1[0]);
            close(pipe2[1]);
            
            dup2(pipe2[0], STDIN_FILENO);       // redirectionam capatul de citire al pipe-ului 2 la intrarea standard
            
            // in timp ce al doilea proces fiu creat pentru regular file scrie la stdout, procesul parinte citeste de la stdin
            char buffer[BUFFER_SIZE];
            ssize_t bytesRead;
            while ((bytesRead = read(pipe2[0], buffer, sizeof(buffer))) > 0) {
                suma += atoi(buffer);
            }

            // inchidem capetul de citire al pipe-ului 2 dupa ce procesul parinte a terminat de citit
            close(pipe2[0]);
        }
        else if (files[i].type == image)            // daca este un fisier bmp vom creea 2 procese
        {
            int nr = 0;
            pid = realloc(pid, (processCount + 2) * sizeof(pid_t));     // realocare memorie
            if (pid == NULL) {
                perror("Eroare realocare memorie procese\n");
                exit(1);
            }

            // creeare procese
            for (int j = 0; j < 2; j++) {
                if ((pid[processCount++] = fork()) < 0) {
                    perror("Eroare creare proces\n");
                    exit(1);
                }

                // daca suntem in codul unuia dintre procesele fiu
                if (pid[processCount - 1] == 0) {
                    // un proces va scrie numarul de linii, iar celalalt va modifica imaginea
                    if (j == 0)
                        nr = processImage1(files[i].file_name, din_path, dout_path);
                    else
                        processImage2(files[i].file_name, din_path);
                    exit(nr);
                }
            }
        }
    }

    int status;
    // pentru fiecare proces, asteptam valoarea de return
    for (int i = 0; i < processCount; i++) {
        wpid = wait(&status);
        if (WIFEXITED(status))
            printf("Procesul fiu %d s-a terminat cu codul %d\n", wpid, WEXITSTATUS(status));
        else
            printf("Procesul fiu %d s-a terminat anormal\n", wpid);
    }

    // eliberare memorie
    free(pid);
    free(files);
    
    // inchidere director iesire
    if (closedir(dir2) < 0)
    {
        perror("Eroare inchidere director iesire!");
        exit(EXIT_FAILURE);
    }

    // afisam numarul de propozitii corecte ce contin caracterul <c>
    printf("Au fost identificate in total <%d> propozitii corecte care contin caracterul <%c>\n", suma, c[0]);
}