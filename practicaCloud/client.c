#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <libgen.h>

#include "handleFile.h" // handle file functions

#define PORT 5000
#define MAX_LENGHT 1024

int flag = 0; // flag for lock inotify
char dname[20] = {0}; // dname for save files

/* thread to receive files from the server  */
void *doReceiving(void *sock)
{
    int client_fd = *((int *)sock);
    char buffer[MAX_LENGHT] = {0};
    int varlen;
    char DESK_CLIENT[20] = {0};
    
    varlen = recv(client_fd, dname, 20, 0); // recive folder name
    dname[varlen] = '\0';

    strcat(DESK_CLIENT, dname);
    strcat(DESK_CLIENT, "/");

    while (1)
    {   
        varlen = recv(client_fd, buffer, MAX_LENGHT, 0); // Recive filename
        buffer[varlen] = '\0';

        if(strcmp(buffer, "START") == 0) { // command to start to notify dirmane client
            flag = 1;
            break;
        }
        if (recv_file(client_fd, buffer, DESK_CLIENT) != 0) { // Recive file data(save in dirname)
            printf("error\n");
        } 
    }

    pthread_exit(NULL);
}

/* thread to notify changes in directory and send files to server */
void *notify_dir(void *sock)
{
    int client_fd = *((int *)sock);
    char buffer[MAX_LENGHT] = {0};
    int varlen;

    while (1)
    {   
        if(strcmp(dname, "") != 0 && flag == 1) {
            printf("watching: %s\n", dname);
            inotify(client_fd, dname);
            sleep(1);
        }
    }

    pthread_exit(NULL);
}


int menu(int sock_fd)
{
    int resp = 0;
    printf("-- Options: --\n");
    printf("1. Upload File\n");
    printf("2. Delete File\n");
    printf("0. EXIT\n");
    printf("Choose an option: ");
    scanf("%d", &resp);
    return resp;
}

int main(int argc, char const *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[MAX_LENGHT] = {0};
    char command[MAX_LENGHT] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("[-] Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("[-] Connection Failed \n");
        return -1;
    }

    printf("[+] Connected to server\n");
    printf("Reciving data from server...\n");

    pthread_t thread, thread_notify;
    pthread_create(&thread, NULL, doReceiving, (void *) &sock);
    pthread_create(&thread_notify, NULL, notify_dir, (void *) &sock);
    while (1)
    {
        int resp = -1;

        if(flag == 1)
            resp = menu(sock);

        if (resp == 1)
        {
            strcat(command, "ADD");
            printf("%ld\n", strlen(command));
            send(sock, command, strlen(command), 0); //send command to server

            printf("type file name and press enter: ");
            scanf("%s", buffer);                   // read filename from console
            char *bname = basename(buffer); // extrac only filename from path
            send(sock, bname, strlen(bname), 0); // send only file name to server
            if (send_file(sock, buffer) == 0)
            { // send file data to server
                printf("File \"%s\" send successfully to server\n", buffer);
            }
            else
            {
                printf("Can not send file \"%s\" to server\n", buffer);
            }
        }
        else if (resp == 2)
        {
            strcat(command, "REMOVE");
            send(sock, command, strlen(command), 0); //send command to server

            printf("type file name and press enter: ");
            scanf("%s", buffer);
            send(sock, buffer, strlen(buffer), 0); // send only file name to server
        }
        else if (resp == 0)
        {
            strcat(command, "EXIT");
            send(sock, command, strlen(command), 0); //send command to server
            break;
        }

        bzero(command, MAX_LENGHT);
        bzero(buffer, MAX_LENGHT);
    }

    return 0;
}