/*EJERCICIO EN CLASE NUM 2*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define NP 20
#define PRO 3  // # productores
#define CON 3  // # consumidores
#define NZON 2 // # zonas criticas

int Zona[NZON][2]; // Zonas Criticas
sem_t s_prod[NZON + 1], s_cons[NZON + 1]; // Arreglo de Semaforos

void *Productor(void *a)
{
    int *ai = (int *)a;
    printf("Productor %d creado \n", *ai);
    for (int i = 0; i < NP; i++)
    {
        printf("Productor %d esperando \n", *ai);
        sem_wait(&s_prod[0]);
        printf("Productor %d trabajando \n", *ai);
        for (int j = 0; j < NZON; j++)
        {
            int val;
            
            sem_getvalue(&s_prod[j + 1], &val);
            printf("Productor %d checando valor %d \n", *ai, val);
            if (val > 0)
            {
                printf("Productor %d sí paso, ahora espera su turno \n", *ai);
                sem_wait(&s_prod[j + 1]);
                Zona[j][i%2] = *ai;
                printf("P%d: Se produjo en la zona critica %d y en la fila %d \n", *ai, j, i%2);
                sem_post(&s_cons[j + 1]);
                printf("Productor %d rompe ciclo con i = %d y j = %d\n", *ai, i, j);
                break;
            }
        }
        sem_post(&s_cons[0]);
    }
    pthread_exit(NULL);
}

void *Consumidor(void *a)
{
    int *ai = (int *)a;
    printf("Consumidor %d creado \n", *ai);
    for (int i = 0; i < NP; i++)
    {
        printf("Consumidor %d esperando \n", *ai);
        sem_wait(&s_cons[0]);
        printf("Consumidor %d trabajando \n", *ai);
        for (int j = 0; j < NZON; j++)
        {
            int val;
            sem_getvalue(&s_cons[j + 1], &val);
            if (val > 0)
            {
                sem_wait(&s_cons[j + 1]);
                printf("C%d: Se consumio en la zona critica %d y en la fila %d\n", *ai, j, i%2);
                sem_post(&s_prod[j + 1]);
                break;
            }
        }
        sem_post(&s_prod[0]);
    }
    pthread_exit(NULL);
}

int main()
{

    // inicializacion de semaforos
    for (int i = 1; i < NZON + 1; i++)
    {
        sem_init(&s_prod[i], 0, 1);
        sem_init(&s_cons[i], 0, 0);
    }

    sem_init(&s_prod[0], 0, 2);
    sem_init(&s_cons[0], 0, 0);

    // inicializacion de hilos
    pthread_t h_prod[PRO], h_cons[CON];

    int p[PRO], c[CON];

    p[0] = 111;
    p[1] = 222;
    p[2] = 333;

    for (int i = 0; i < CON; i++)
    {
        c[i] = i;
    }

    // creacion de hilos productores
    for (int i = 0; i < PRO; i++)
    {
        pthread_create(&h_prod[i], NULL, Productor, (void *)&p[i]);
    }

    // creacion de hilos consumidores
    for (int i = 0; i < CON; i++)
    {
        pthread_create(&h_cons[i], NULL, Consumidor, (void *)&c[i]);
    }

    // joiners
    for (int i = 0; i < PRO; i++)
    {
        pthread_join(h_prod[i], NULL);
    }

    for (int i = 0; i < CON; i++)
    {
        pthread_join(h_cons[i], NULL);
    }

    return 0;
}