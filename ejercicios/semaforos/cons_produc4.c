/*
  AUTOR: VLADIMIR AZPEITIA
  DESCRIPCION: PROGRAMA DE CONSUMIDORES Y PRODUCTORES QUE ACCEDEN A 2 SECCIONES CRITICAS
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 5 //NÚMERO MÁXIMO DE ITERACIONES POR HILO

int buffer[2]; //SECCIONES CRITICAS
int cont[2] = {0, 0};
sem_t sem_cons[2], sem_prod[2]; //SEMAFOROS

void *func_prod(void *arg) //FUNCION PARA PRODUCIR EN CUALQUIERA DE LAS 2 SECCIONES CRITICAS
{
    int *pvalue = (int *) arg;
    for (int i = 0; i < MAX; i++)
    {
        for (int i = 0; i < 2; i++)
        {
            sem_wait(&sem_cons[i]);
            buffer[i] = *pvalue;
            cont[i]++;
            printf("Se produjo %d en buffer[%d]\n", buffer[i], i);
            sem_post(&sem_prod[i]);
        }
        
    }
}

void *func_cons(void *arg) //FUNCION QUE CONSUME DE ALGUNA DE LAS 2 SECCIONES 
{
    int *pindex = (int *) arg;
    for (int i = 0; i < MAX; i++)
    {
        for (int i = 0; i < 2; i++)
        {
            sem_wait(&sem_prod[i]);
            cont[i]--;
            printf("C%d: Se consumio %d en buffer[%d] \n", *pindex, buffer[i], i);
            buffer[i] = 0;
            sem_post(&sem_cons[i]);
        }
    }
}

int main(void)
{
    pthread_t hcons[3], hprod[3];
    
    sem_init(&sem_cons[0], 0, 1);
    sem_init(&sem_prod[0], 0, 0);
    sem_init(&sem_cons[1], 0, 1);
    sem_init(&sem_prod[1], 0, 0);

    int a[3] = {11, 22, 33}; //VALORES QUE PRODUCE CADA HILO RESPECTIVAMENTE
    int b[3] = {1, 2, 3}; //INDICE DE LOS CONSUMIDORES

    for (int i = 0; i < 3; i++)
    {
        pthread_create(&hprod[i], NULL, func_prod, (void *)&a[i]);
        pthread_create(&hcons[i], NULL, func_cons, (void *)&b[i]);
    }
    
    for (int i = 0; i < 3; i++){
        pthread_join(hprod[i], NULL);
        pthread_join(hcons[i], NULL);
    }
    
    sem_destroy(&sem_cons[0]);
    sem_destroy(&sem_prod[0]);
    sem_destroy(&sem_cons[1]);
    sem_destroy(&sem_prod[1]);

    return 0;
}