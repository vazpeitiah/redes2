/*
  Descripcion: programa que tiene 1 productor y un consumidor

*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 10

int contador;
sem_t sem_con, sem_pro;

void *func_prod(void *arg)
{
  for (int i = 0; i < MAX; i++)  
  {  
    sem_wait(&sem_pro);
    contador = contador + 1;
    printf("se produjo un item \t contador = %d \n", contador);
    sem_post(&sem_con);
  }
}

void *func_cons(void *arg)
{
  for (int i = 0; i < MAX; i++) 
  {
    sem_wait(&sem_con);
    contador = contador - 1;
    printf("se consumio un item \t contador = %d \n", contador);
    sem_post(&sem_pro);
  }
}

int main(void)
{
  srand(time(0));
  pthread_t hcons, hprod;
  contador = 0;
  sem_init(&sem_con, 0, 0);
  sem_init(&sem_pro, 0, 1);
  pthread_create(&hcons, NULL, func_cons, NULL);
  pthread_create(&hprod, NULL, func_prod, NULL);
  pthread_join(hprod, NULL);
  pthread_join(hcons, NULL);
}