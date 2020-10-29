#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //PARA USAR sleep(2)
#include <time.h> //PARA USAR time(0)
#include <pthread.h> //API THREADS

/*VARIABLES GLOBALES*/
int **matrixA;
int **matrixB;
int **matrixC; //Matriz resulatdo
int A_rows, A_cols;
int B_rows, B_cols;
int num_threads;

/*PARMENTROS DEL HILO*/
typedef struct thread_param{
    int num_rows; //Numero de filas que le corresponden a cada hilo
    int start_index; //Índice de la primer fila que le corresponde al hilo
    int end_index; //Índice de la última fila que le corresponde al hilo
}thread_param;

/*FUNCION PARA CREAR UNA MATRIZ Y RELLENARLA CON VALORES DEL 0 AL 5*/
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

/*IMPRIMIR MATRIZ EN UN ARCHIVO*/
void fprint_matrix(FILE *file, int **matrix, int rows, int cols){
    if(file != NULL){
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < cols; j++){
				fprintf(file, "%10d", matrix[i][j]);
			}
			fprintf(file,"\n");
		}
	}
}

/*HILO*/
void *threadf(void *arg) {
    thread_param *param = (thread_param *) arg;
    for (int i = param->start_index; i < param->end_index; i++){ // Solo multiplica las filas que le corresponden
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
        /*OBTENEMOS LOS PARAMETROS DESDE CONSOLA*/
        A_rows = atoi(argv[1]);
        A_cols = atoi(argv[2]);
        B_rows = atoi(argv[3]);
        B_cols = atoi(argv[4]);
        num_threads = atoi(argv[5]);
        /*CREAMOS LAS MATRICES*/
        matrixA = create_matrix(A_rows, A_cols);
        matrixB = create_matrix(B_rows, B_cols);
        matrixC = create_matrix(A_rows, B_cols);
              
        if(A_cols == B_rows && num_threads <= A_rows && num_threads > 0){
            pthread_t threads[num_threads];
            thread_param params[num_threads];
            
            int div = A_rows / num_threads;
            int res = A_rows % num_threads;
            int position = 0;
            for (int i = 0; i < num_threads; i++){
                if(i == 0){
                    params[i].num_rows = div + res; //Las filas restantes se asignan siempre al primer hilo
                }else{
                    params[i].num_rows = div; //Al resto se le asigna el mismo número
                }
                //Calculamos la posicion de inicio y de fin de cada fila
                params[i].start_index = position;
                position += params[i].num_rows;
                params[i].end_index = position;
            }
             
            for(int i = 0; i < num_threads; i++ ) {
                pthread_create(&threads[i], NULL, threadf, &params[i]);
            }

            for(int i = 0; i < num_threads; ++i){
                pthread_join(threads[i], NULL);
            }
            
            /*IMPRIMIMOS EL RESULTADO EN UN ARCHIVO*/
            FILE *f = fopen("results.txt", "w"); //Archivo en donde vamos a escribir los resultados
            fprintf(f, "%s","MULTIPLICACIÓN DE MATRICES\n\tA * B = C\n");
            fprintf(f, "%s","the matrix A is:\n");
            fprint_matrix(f, matrixA, A_rows, A_cols);
            fprintf(f, "%s","the matrix B is:\n");
            fprint_matrix(f, matrixB, B_rows, B_cols);
            fprintf(f, "%s","the matrix C is:\n");
            fprint_matrix(f, matrixC, A_rows, B_cols);

            printf("Se guardo el resultado en result.txt\n");
        }else{
            printf("Error: no sé puede realizar la multiplicación con los paramétros especificados\n");
        }
        pthread_exit(NULL);
    }else{
        printf("Faltan paramétros\nUsar así: %s [FilasA] [ColumnasA] [FilasB] [ColumnasB] [NúmeroHilos]\n", argv[0]);
        return 0;
    }

}