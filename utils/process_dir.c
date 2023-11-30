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

/* functie procesare director intrare */
void processDIR(char *din_path, char *dout_path)
{
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
                    files[countEntries++].type = reg_file;      // copiere tip pt regular file
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

    int pipe1[2];   // pipe 1: primul fiu -> al doilea fiu
    int pipe2[2];   // pipe 2: al doilea fiu -> parinte
    
    // creeaza pipe-uri
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        perror("Eroare creare pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t *pid = NULL, wpid;        // tablou procese copii, id auxiliar pentru testarea valorii de return
    int processCount = 0;           // numar procese copii

    // se creeaza procesele copii in paralel in functie de tipul intrari curente din tablou
    for(int i = 0; i < countEntries; i++)
    {
        // daca nu este de tipul bmp se creeaza un singur proces
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
                    nr = processDirectory(files[i].file_name);
                }
                if (files[i].type == slink) 
                {
                    nr = processLinks(files[i].file_name);
                }
                // if (files[i].type == reg_file) {
                //     nr = processRegularFile(files[i].file_name);
                
                // }
                exit(nr);
            }
        }
        else if (files[i].type == reg_file)
        {
            int nr = 0;
            pid = realloc(pid, (processCount + 2) * sizeof(pid_t));     // realocare memorie
            if (pid == NULL) {
                perror("Eroare realocare memorie procese\n");
                exit(1);
            }

            // Creează proces pentru statistică
            if ((pid[processCount++] = fork()) < 0) {
                perror("Eroare creare proces\n");
                exit(1);
            }

            // daca suntem in codul unuia dintre procesele fiu
            if (pid[processCount - 1] == 0) {

                // un proces va scrie numarul de linii, iar celalalt va modifica imaginea
                nr = processRegularFile(files[i].file_name, pipe1);
                exit(nr);
            }

            // Creează proces pentru conținut
            if ((pid[processCount++] = fork()) < 0)
            {
                perror("Eroare creare proces\n");
                exit(EXIT_FAILURE);
            }

            if (pid[processCount - 1] == 0)
            {
                // // Închide capătul de citire al pipe-ului de statistici
                // close(pipes_stat[1]);

                // // Închide capătul de scriere al pipe-ului de conținut
                // close(pipes_content[1]);

                processFileContent(pipe1, pipe2);
                exit(0);
            }
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
                        nr = processImage1(files[i].file_name);
                    else
                        processImage2(files[i].file_name);
                    exit(nr);
                }
            }
        }
    }

    // inchidere capete pipe
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]);

    // deschidem fisier statistica
    int sfd = open("statistica.txt", O_APPEND | O_WRONLY | O_CREAT,  0666);

    if (sfd < 0)
    {
        perror("Eroare creare fisier statistica!");
        exit(1);
    }

    int status;
    // pentru fiecare proces, asteptam valoarea de return
    for (int i = 0; i < processCount; i++) {
        wpid = wait(&status);
        if (WIFEXITED(status))
            printf("Procesul fiu %d s-a terminat cu codul %d\n", wpid, WEXITSTATUS(status));
        else
            printf("Procesul fiu %d s-a terminat anormal\n", wpid);

        if (WEXITSTATUS(status) > 0)
        {
            dprintf(sfd, "%d\n", WEXITSTATUS(status));      // scriem in fisierul statistica din directorul de iesire numarul de linii scrise de procesul fiu curent
        }
    }

    // inchidere fisier statistica
    if (close(sfd) < 0)
    {
        perror("Eroare inchidere fisier statistica!");
        exit(EXIT_FAILURE);
    }

    // Citește din pipe 2
    char buffer2[100];
    read(pipe2[0], buffer2, sizeof(buffer2));
    printf("Procesul parinte a citit: %s\n", buffer2);

    // eliberare memorie
    free(pid);
    free(files);
    
    // inchidere director iesire
    if (closedir(dir2) < 0)
    {
        perror("Eroare inchidere director iesire!");
        exit(EXIT_FAILURE);
    }

}