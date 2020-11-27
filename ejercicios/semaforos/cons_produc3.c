/*
  Descripcion: programa que tiene 2 productores y 3 consumidores
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 20

int cont;
sem_t sem_con, sem_pro, sem_productores;

void *func_prod1(void *arg)
{
  for (int i = 0; i < MAX; i++)  
  {  
    sem_wait(&sem_pro);
    cont++;
     printf("p1 %d: se produjo 0 1 2 \t cont = %d \n", i+1, cont);
    sem_post(&sem_con);
  }
}

void *func_prod2(void *arg)
{
  for (int i = 0; i < MAX; i++)  
  {  
    sem_wait(&sem_pro);
    cont++;
    printf("p2 %d: se produjo 3 4 5 \t cont = %d \n", i+1, cont);
    sem_post(&sem_con);
  }
}

void *func_cons1(void *arg)
{
  for (int i = 0; i < 20; i++) 
  {
    sem_wait(&sem_con);
    cont--;
    printf("c1: se consumio un item \t cont = %d \n", cont);
    sem_post(&sem_pro);
  }
}

void *func_cons2(void *arg)
{
  for (int i = 0; i < 20; i++) 
  {
    sem_wait(&sem_con);
    cont--;
    printf("c2: se consumio un item \t cont = %d \n", cont);
    sem_post(&sem_pro);
  }
}

int main(void)
{
  srand(time(0));
  pthread_t hcons, hprod;
  cont = 0;
  sem_init(&sem_con, 0, 0);
  sem_init(&sem_pro, 0, 1);
  pthread_create(&hprod, NULL, func_prod1, NULL);
  pthread_create(&hprod, NULL, func_prod2, NULL);
  pthread_create(&hcons, NULL, func_cons1, NULL);
  pthread_create(&hcons, NULL, func_cons2, NULL);
  pthread_join(hprod, NULL);
  pthread_join(hcons, NULL);
}