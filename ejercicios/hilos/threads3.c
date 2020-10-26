#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct dato
{
    int id;
    char c[50];
} DATO;

void *codigo_hilo_hijo(void *arg)
{
    DATO *d = (DATO *)arg;
    printf("Hilo = %d \n", d->id);
    //RETORNARA EL ARGUMENTO EL CUAL SE GUARDA EN EL SEGUNDO PARAMETRO DE LA LLAMADA AL SISTEMA pthread_join().
    sleep(1);
    pthread_exit((void *)&d->id);
}

int main(int argc, char *argv[])
{
    int num_hilos = atoi(argv[1]);
    if(num_hilos > 2){
        pthread_t hilos[num_hilos];
        DATO datos[num_hilos];
        int error;
        int *salida; //VARIABLE QUE RECIBE LO RETORNADO POR LA FUNCIÓN pthread_exit()
        for (int i = 0; i < num_hilos; i++)
        {
            datos[i].id = i;
            pthread_create(&hilos[i], NULL, codigo_hilo_hijo, &datos[i]);
        }
        for (int i = 0; i < num_hilos; i++)
        {
            error = pthread_join(hilos[i], (void **)&salida);
            if (error)
                fprintf(stderr, "Error %d: %s\n", error, strerror(error));
            else
                //MUESTRO LO QUE RECIBE EL HILO PADRE DEL HIJO
                printf("Hilo %d terminado\n", *salida);
        }
    }else{
        printf("usage: %s <num_hilos>\n", argv[0]);
    }
    
    return 0;
}