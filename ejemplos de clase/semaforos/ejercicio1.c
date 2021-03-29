#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MaxItems 5 // Numero maximo de items que se pueden producir o consumir por hilo
#define BufferSize //Tamaño del buffer

int entrada = 0;
int out = 0;
char buffer[3];

sem_t sem_vacio, sem_lleno;
pthread_mutex_t mutex;

void *producer(void *pno)
{   
    int item;
    for(int i = 0; i < MaxItems; i++) {
        item = 11;
        sem_wait(&sem_vacio);
        pthread_mutex_lock(&mutex);
        buffer[entrada] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno),buffer[entrada],entrada);
        entrada = (entrada+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_lleno);
    }
}
void *consumer(void *cno)
{   
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&sem_lleno);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n",*((int *)cno),item, out);
        out = (out+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_vacio);
    }
}

int main()
{   

    pthread_t pro[3],con[3];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_vacio,0,BufferSize);
    sem_init(&sem_lleno,0,0);

    int a[3] = {1,2,3};

    for(int i = 0; i < 3; i++) { //Numero de productores 
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
    for(int i = 0; i < 5; i++) {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    for(int i = 0; i < 5; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem_vacio);
    sem_destroy(&sem_lleno);

    return 0;
    
}