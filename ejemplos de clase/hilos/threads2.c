#include <pthread.h> //api threads
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct{
   int id;
   char *name;
} student; 

void *helloThread(void *st) {
   student* pst = (student *) st;
   int id = pst->id;
   char *name = pst->name;
   printf("thread with student id #%d and name %s\n", id, name);
   pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
   if(argc == 2){
      int num_threads = atoi(argv[1]);
      int i;
      pthread_t threads[num_threads];
      student students[num_threads];
      for(i = 0; i < num_threads; i++ ) {
         students[i].id = i;
         students[i].name = "thread";
         pthread_create(&threads[i], NULL, helloThread, &students[i]);
      }

      for(i = 0; i < num_threads; ++i){
         pthread_join(threads[i], NULL);
      }

   }else{
      printf("usage: %s <num_threads>\n", argv[0]);
      return 0;
   }
}