/* ---------------------------------------------------------------
Descripcion: Realizar un programa que calcuale n veces la serie de fibonacci, 
cada calculo se le deberá asignar a un hilo y el usuario asinara
un valor a cada hilo, desde la consola
---------------------------------------------------------------  */

#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *fibonacci(void *threadid) {
	int *num = (int *) threadid;
	printf("thread value %d  \t", *num);

	if(*num >= 0){
		long xf;
		int i;
		if(*num == 0 || *num == 1){
			xf = (*num == 0) ? 0 : 1;
		}else{
			long x1 = 0;
			long x2 = 1;
			for (i = 1; i < *num; i++){
				xf = x1 + x2;
				x1 = x2;
				x2 = xf;
			}
		}
		printf("fibonacci n = %ld\n",xf);
	}else{
		printf("error: incorret value\n");
	}

	sleep(2);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if(argc > 1){
		int i;
        int num_threads = argc-1;
        pthread_t threads[num_threads];

		int valores[num_threads];
		for (i = 0; i < num_threads; i++){
			valores[i] = atoi(argv[i+1]);
		}
		
        for(i = 0; i < num_threads; i++ ) {
        	pthread_create(&threads[i], NULL, fibonacci, &valores[i]);
        }

        for(i = 0; i < num_threads; ++i){
            pthread_join(threads[i], NULL);
        }
    }else{
        printf("usage: %s values ...\n", argv[0]);
        return 0;
    }
}