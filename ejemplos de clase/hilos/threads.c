/*Ejericio 1 de hilos: el programa recibe un número n entero desde consolo
y ejectuta n hilos con una función que imprime un hola mundo
*/

#include <pthread.h> //api threads
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *helloThread(void *threadid) {
   printf("hello world! from thread #%d\n", threadid);
   sleep(2);
   pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
   if(argc == 2){
      int num_threads = atoi(argv[1]);
      int i;
      pthread_t threads[num_threads];
      int ids[num_threads];
      for(i = 0; i < num_threads; i++ ) {
         ids[i] = i;
         pthread_create(&threads[i], NULL, helloThread, &ids[i]);
      }

      for(i = 0; i < num_threads; ++i){
         pthread_join(threads[i], NULL);
      }

   }else{
      printf("usage: %s <num_threads>\n", argv[0]);
      return 0;
   }
}