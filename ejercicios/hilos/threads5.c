#include <stdio.h>
#include <stdlib.h>
#include <time.h>/*PARA USAR time(0)*/

/*VARIABLES GLOBALES*/
int **matrixA;
int **matrixB;
int **matrixC; //Matriz resulatdo
int A_rows, A_cols;
int B_rows, B_cols;
int num_threads;

/*PARAMENTROS DEL HILO*/
typedef struct param{
    int ncols;
    int inicio;
    int fin;
}param;

/*FUNCION PARA CREAR MATRICES*/
int **create_matrix(int rows, int cols){
    int **matrix = (int**)malloc(rows*sizeof(int*));
	for(int i = 0; i < rows; i++){
		matrix[i] = (int*)malloc(cols*sizeof(int));
	}
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            matrix[i][j] = rand() % 6;
        }
    }
    return matrix;
}

/*IMPRIMIR MATRIZ EN CONSOLA*/
void print_matrix(int **matrix, int rows, int cols){
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("%10d", matrix[i][j]);
        }
        printf("\n");
    }
}

/*IMPRIMIR MATRIZ EN ARCHIVO*/
void fprint_matrix(FILE *f, int **matrix, int rows, int cols){
    if(f != NULL){
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < cols; j++){
				fprintf(f, "%10d", matrix[i][j]);
			}
			fprintf(f,"\n");
		}
	}
}

/*HILOS*/
void *threadf(void *arg) {
    param *parametro = (param *) arg;
    printf("A este hilo le toca %d columnas (de %d a %d)\n", parametro->ncols, parametro->inicio, parametro->fin);
    for (int i = parametro->inicio; i < parametro->fin; i++){
        for (int j = 0; j < B_cols; j++){
            int sum = 0;
            for (int k = 0; k < A_cols; k++){
                sum += matrixA[i][k] * matrixB[k][j];
            }
            matrixC[i][j] = sum;
        }
    }
    sleep(2);
    pthread_exit(NULL);
}

/*FUNCION PRINCIPAL*/
int main(int argc, char const *argv[]){
    srand(time(0));
    if(argc == 6){
        FILE *f = fopen("results.txt", "w"); 
        fprintf(f, "%s","A * B = C\n");
        A_rows = atoi(argv[1]);
        A_cols = atoi(argv[2]);
        B_rows = atoi(argv[3]);
        B_cols = atoi(argv[4]);
        num_threads = atoi(argv[5]);

        matrixA = create_matrix(A_rows, A_cols);
        matrixB = create_matrix(B_rows, B_cols);
        matrixC = create_matrix(A_rows, B_cols);

        fprintf(f, "%s","the matrix A is:\n");
        fprint_matrix(f, matrixA, A_rows, A_cols);
        
        fprintf(f, "%s","the matrix B is:\n");
        fprint_matrix(f, matrixB, B_rows, B_cols);
              
        if(A_cols == B_rows && num_threads <= A_rows && num_threads > 0){
            pthread_t threads[num_threads];
            int div = A_rows / num_threads;
            int res = A_rows % num_threads;

            param parametros[num_threads];
            int position = 0;
            for (int i = 0; i < num_threads; i++){
                if(i == 0){
                    parametros[i].ncols = div + res;
                }else{
                    parametros[i].ncols = div;
                }
                parametros[i].inicio = position;
                position += parametros[i].ncols;
                parametros[i].fin = position;
            }
             
            for(int i = 0; i < num_threads; i++ ) {
                pthread_create(&threads[i], NULL, threadf, &parametros[i]);
            }

            for(int i = 0; i < num_threads; ++i){
                pthread_join(threads[i], NULL);
            }
            fprintf(f, "%s","the matrix C is:\n");
            printf("the result's saved in result.txt\n");
            fprint_matrix(f, matrixC, A_rows, B_cols);
        }else{
            printf("error: this multiplication isn't possible --> A_cols = B_rows && num_threads <= A_rows\n");
        }

    }else{
        printf("usage: %s <A_rows> <A_cols> <B_rows> <B_cols> <NUM_THREADS>\n", argv[0]);
        return 0;
    }

}