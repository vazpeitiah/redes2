#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include "handleFile.h"

#define PORT 5000
#define MAX_LENGHT 1024
#define MAX_CLIENT 1024

typedef struct client {
  int index;
  int socket;
  char *desk_client;
} client;

client client_data[MAX_CLIENT];
int clientCount = 0;
sem_t sem_client;

void *thread_function(void *data)
{
  client *cli = (client *)data;
  int index = cli->index;
  int client_fd = cli->socket;
  char *dirname = cli->desk_client;

  int varlen = 0;
  char buffer[MAX_LENGHT] = {0};
  char command[MAX_LENGHT] = {0};
  char filename[MAX_LENGHT] = {0};

  printf("[+] Client with index %d is connected\n", index);
  
  send(client_fd, dirname, strlen(dirname), 0); // send directory name
  send_upfiles(client_fd, "dsk_server"); // send all uploaded files to the client

  while (1)
  {
    varlen = recv(client_fd, command, MAX_LENGHT, 0); // Recive command from clients
    command[varlen] = '\0';

    /* ADD A NEW FILE IN CLOUD SERVER AND SEND TO THE ALL CLIENTS */
    if (strcmp(command, "ADD") == 0)
    {                                                  
      varlen = recv(client_fd, buffer, MAX_LENGHT, 0); // recive file name
      buffer[varlen] = '\0';
      if (recv_file(client_fd, buffer, DSK_SERVER) == 0){ // recive data file
        printf("File \"%s\" uploaded successfully to server\n", buffer);
      }else{
        printf("Can not recive file \"%s\" to client with index %d\n", buffer, index);
      }
      for (int i = 0; i < clientCount; i++) // send the new file to all clients(except this)
      {
        if ( (client_data[i].index != -1) && (client_data[i].index != index) )
        {
          send(client_data[i].socket, buffer, strlen(buffer), 0); // send file name

          char filename[20] = {0};
          strcat(filename, DSK_SERVER);
          strcat(filename, buffer);

          send_file(client_data[i].socket, filename); // send file data to server
        }
      }
    }
    else if (strcmp(command, "REMOVE") == 0)
    {
      varlen = recv(client_fd, buffer, MAX_LENGHT, 0); // recive filename
      buffer[varlen] = '\0';

      strcat(filename, DSK_SERVER);
      strcat(filename, buffer);

      remove_file(filename); // remove file in desk server

      for (int i = 0; i < clientCount; i++)
      {
        if ( (client_data[i].index != -1) && (client_data[i].index =! index) )
        {
          printf("%d = %d\n", client_data[i].index, index);
          bzero(filename, MAX_LENGHT);
          strcat(filename, client_data[i].desk_client);
          strcat(filename, "/");
          strcat(filename, buffer);

          remove_file(filename); // Remove file in dsk client i
        }
      }
    }
    else if (strcmp(command, "EXIT") == 0)
    {
      break;
    }

    bzero(command, MAX_LENGHT); // clear command buffer
    bzero(buffer, MAX_LENGHT);  // clear buffer
  }
  printf("Closing client %d connection...\n", index);
  remove_dir(dirname);

  sem_wait(&sem_client);
  client_data[index].index = -1;
  sem_post(&sem_client);

  printf("[+] Client with index %d is desconnected\n", index);
  pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
  int server_fd, new_sock;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  int opt = 1;
  
  pthread_t threads[MAX_CLIENT];

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("[-] socket failed");
    exit(EXIT_FAILURE);
  }
  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    perror("[-] setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Forcefully attaching socket to the port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("[-] bind failed");
    exit(EXIT_FAILURE);
  }
  //Server start listening
  if (listen(server_fd, MAX_CLIENT) < 0)
  {
    perror("[-] listen");
    exit(EXIT_FAILURE);
  }

  printf("[+] Server listening on port %d\n", PORT);

  make_dir("dsk_server"); //make directory desktop server 
  sem_init(&sem_client, 0, 1);

  while (1)
  {
    if ((new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
      perror("[-] accept");
      exit(EXIT_FAILURE);
    }

    char filename[20] = {0};
    snprintf(filename, 20, "dsk_client%d", clientCount); // concat dsk_client wiht clientCount
    make_dir(filename);                                  // create a new directory for this client
    
    sem_wait(&sem_client);
    client_data[clientCount].socket = new_sock;
    client_data[clientCount].index = clientCount;
    client_data[clientCount].desk_client = filename;
    sem_post(&sem_client);

    pthread_create(&threads[clientCount], NULL, thread_function, (void *)&client_data[clientCount]);

    clientCount++;
  }

  for (int i = 0; i < clientCount; i++)
  {
    pthread_join(threads[i], NULL);
  }

  return 0;
}