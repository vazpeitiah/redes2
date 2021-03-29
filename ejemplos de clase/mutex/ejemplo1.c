#include <pthread.h>
#include <stdio.h>
int V = 1000;
/* Variable global */
pthread_mutex_t m =
    PTHREAD_MUTEX_INITIALIZER;
/* mutex que controla acceso a V */
void *hilo1(void *arg)
{
    int i;
    for (i = 0; i < 1000; i++)
    {
        pthread_mutex_lock(&m);
        V = V + 1;
        pthread_mutex_unlock(&m);
    }
}

void *hilo2(void *arg)
{
    int i;
    for (i = 0; i < 1000; i++)
    {
        pthread_mutex_lock(&m);
        V = V - 1;
        pthread_mutex_unlock(&m);
    }
}
int main(void)
{
    pthread_t h1, h2;
    pthread_attr_t atrib;
    pthread_attr_init(&atrib);
    pthread_create(&h1, &atrib, hilo1, NULL);
    pthread_create(&h2, &atrib, hilo2, NULL);
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);
    printf("Hilo principal: V = % d\n", V);
}