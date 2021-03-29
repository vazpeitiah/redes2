#include <pthread.h>     // Biblioteca para hilos POSIX
#include <semaphore.h>   // Biblioteca para semaforos POSIX
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NP 10000  // Numero de producciones (por cada productor)
#define PRO 4  // Numero de productores
#define CON 3  // Numero de consumidores
#define NZON 6 // Numero de zonas criticas

int buffer[NZON];                              // Zonas Criticas
sem_t sem_prod[NZON + 1], sem_cons[NZON + 1];  // Semaforos

FILE *f1, *f2, *f3, *f4;                       // Archivos 

typedef struct parameters{
    int id;
    int bnum;
} parameters;

void *prod_thread(void *arg)
{
    int id_producer = *((int *) arg);
    for (int i = 0; i < NP; i++)
    {
        sem_wait(&sem_prod[0]);
        for (int j = 0; j < NZON; j++)
        {
            int val;
            sem_getvalue(&sem_prod[j + 1], &val);
            if (val > 0)
            {
                sem_wait(&sem_prod[j + 1]);
                buffer[j] = id_producer * 1111;
                //printf("P%d: Se produjo %d en la zona critica %d\n", id_producer, buffer[j], j + 1);
                sem_post(&sem_cons[j + 1]);
                break;
            }
        }
        sem_post(&sem_cons[0]);
    }
    printf("P%d: Termine mi trabajo\n", id_producer);
    pthread_exit(NULL);
}

void *cons_thread(void *arg)
{
    parameters params = *((parameters *) arg);
    int id_consumer = params.id;
    int max = params.bnum;
    for (int i = 0; i < max; i++)
    {
        sem_wait(&sem_cons[0]);
        for (int j = 0; j < NZON; j++)
        {
            int val;
            sem_getvalue(&sem_cons[j + 1], &val);
            if (val > 0)
            {
                sem_wait(&sem_cons[j + 1]);
                
                //printf("C%d: Se consumio %d en la zona critica %d\n", id_consumer, buffer[j], j + 1);
                switch (buffer[j]) // Escribir en archivos de texto
                {
                    case 1111:
                        fprintf(f1, "%s\n", "1111");
                        break;
                    case 2222:
                        fprintf(f2, "%s\n", "2222");
                        break;
                    case 3333:
                        fprintf(f3, "%s\n", "3333");
                        break;
                    case 4444:
                        fprintf(f4, "%s\n", "4444");
                        break;
                    default:
                        printf("C%d: Hubo un error al momento de consumir\n", id_consumer);
                        break;
                }
                buffer[j] = -1;
                sem_post(&sem_prod[j + 1]);
                break;
            }
        }
        sem_post(&sem_prod[0]);
    }
    printf("C%d: Termine mi trabajo\n", id_consumer);
    pthread_exit(NULL);
}

int main()
{
    //Hilos
    pthread_t h_prod[PRO], h_cons[CON];

    f1 = fopen("1111.txt", "w"); //Archivo donde se guardan 1111
    f2 = fopen("2222.txt", "w"); //Archivo donde se guardan 1111
    f3 = fopen("3333.txt", "w"); //Archivo donde se guardan 1111
    f4 = fopen("4444.txt", "w"); //Archivo donde se guardan 1111

    // inicializacion de semaforos
    for (int i = 1; i < NZON + 1; i++)
    {
        sem_init(&sem_prod[i], 0, 1);
        sem_init(&sem_cons[i], 0, 0);
    }

    sem_init(&sem_prod[0], 0, NZON);
    sem_init(&sem_cons[0], 0, 0);


    //Arreglo de identificadores para productores y consumidores
    int ids_prod[PRO];
    for (int i = 0; i < PRO; i++)
    {
        ids_prod[i] = i + 1;
    }

    parameters params[CON];
    int div = floor(NP * PRO / CON);
    int mod = (NP * PRO) % CON;
    for (int i = 0; i < CON; i++)
    {
        params[i].id = i + 1;
        params[i].bnum = div;
        if(i == 0)
            params[i].bnum += mod;
    }

    
 
    // creacion de hilos productores
    for (int i = 0; i < PRO; i++)
    {
        pthread_create(&h_prod[i], NULL, prod_thread, (void *)&ids_prod[i]);
    }

    // creacion de hilos consumidores
    for (int i = 0; i < CON; i++)
    {
        pthread_create(&h_cons[i], NULL, cons_thread, (void *)&params[i]);
    }

    // Esperar a que terminen los hilos
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