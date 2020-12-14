/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/
 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

float *matrixA; //matriz de entrada
float *matrixB; //matriz de entrada
float *matrixC; //matriz de saída
double timeSpent; //marca-tempo
struct timespec start, finish; //usado pelo marca-tempo

//argumentos a serem passados para as threads
typedef struct {
	int size;
	int startID;
	int endID;
} threadAtt;

//tarefa executada por cada thread
void * multMat (void * arg) {
	//cada thread vai receber uma quantidade de linhas para fazer a multiplicação
	threadAtt *argument = (threadAtt *) arg;
	int side = argument->size;
	int startLine = argument->startID; //primeira linha que a thread vai processar
	int endLine = argument->endID;      //última linha que a thread vai processar
	int i, j, k;
	
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = startLine; i < endLine; i++) {
        for (j = 0; j < side; j++) {
            matrixC[i*side + j] = 0;
            for (k = 0; k < side; k++)
                matrixC[i*side + j] += matrixA[i*side + k] * matrixB[k*side + j];
        }
    }
	clock_gettime(CLOCK_MONOTONIC, &finish);

	timeSpent = (finish.tv_sec - start.tv_sec);
	timeSpent += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	
	pthread_exit(NULL);
}

//mostra as matrizes na tela
void showMatrix (int size) {
	int i;
	int j;
	printf("Matriz A:\n");
	for(i = 0; i<size; i++) {
		for(j=0; j<size; j++) {
			printf("%.1f ", matrixA[i*size + j]);
		}
		printf("\n");
	}
	
	printf("Matriz B:\n");
	for(i = 0; i<size; i++) {
		for(j=0; j<size; j++) {
			printf("%.1f ", matrixB[i*size + j]);
		}
		printf("\n");
	}
	
	printf("Matriz C:\n");
	for(i = 0; i<size; i++) {
		for(j=0; j<size; j++) {
			printf("%.1f ", matrixC[i*size + j]);
		}
		printf("\n");
	}
}

//função de teste para verificar a validade da multiplicação de matrizes
void multiplica (int side) {
	int i, j, k;
	for (i = 0; i < side; i++) {
        for (j = 0; j < side; j++) {
            matrixC[i*side + j] = 0;
            for (k = 0; k < side; k++)
                matrixC[i*side + j] += matrixA[i*side + k] * matrixB[k*side + j];
        }
    }
	showMatrix(side);
}

int main (int argc, char* argv[]) {
	int matrixSide; 					//tamanho das matrizes
	int i; 								//usado em iterações
	int j; 								//usado em iterações
	threadAtt *parameters;				//passa os parâmetros para cada thread
	int nthreads;						//guarda quantas threads existem
	pthread_t *internalID;				//IDs internos das threads
	int threadLoad; 					//usado para guardar quantas linhas cada thread vai executar
	int aux = 0; 						//variável auxiliar
	
	//evita falhas de formatação nos argumentos da linha de comando
	if (argc<3) {
		printf("Formatação incorreta!\nDigite: %s <dimensão da matriz> <número de threads>\n", argv[0]);
		return 1;
	}
	matrixSide = atoi(argv[1]);
	nthreads = atoi(argv[2]);
	
	//aloca memória para as três matrizes a serem usadas e para o array de IDs internos
	matrixA = malloc(sizeof(float) * matrixSide * matrixSide);
	matrixB = malloc(sizeof(float) * matrixSide * matrixSide);
	matrixC = malloc(sizeof(float) * matrixSide * matrixSide);
	internalID = malloc(sizeof(pthread_t) * nthreads);
	
	//evita falha por falta de memória ou falta de permissão de alocação de memória
	if(matrixA == NULL || matrixB == NULL || matrixC == NULL) {
		printf("Falha na alocação de memória! (Matrizes)\n");
		return 2;
	}
	
	if (internalID == NULL) {
		printf("Falha na alocação de memória! (ID Interno)\n");
		return 2;
	}
	
	//inicializa as matrizes
	for(i = 0; i<matrixSide; i++) {
		for(j=0; j<matrixSide; j++) {
			matrixA[i*matrixSide + j] = 1;
			matrixB[i*matrixSide + j] = 1;
			matrixC[i*matrixSide + j] = 0;
		}
	}
	
	threadLoad = matrixSide/nthreads;
	i = 0;
	
	//se o número de threads não for igual a 1, divide a load entre as threads existentes
	if (nthreads != 1) {
		for(i = 0; i<nthreads-1; i++) {
			parameters = malloc(sizeof(threadAtt));
			if (parameters == NULL) {
				printf("Falha na alocação de memória! (Parâmetros)\n");
				return 2;
			}
			parameters->size = matrixSide;
			parameters->startID = aux;
			parameters->endID = aux + threadLoad;
			pthread_create(&internalID[i], NULL, multMat, (void*) parameters);
			aux += threadLoad;
		}
	}
	//a última thread é criada separadamente para poder ir até o final da matriz (a última thread recebe uma load maior)
	//se só existir uma thread a ser criada, ela é criada aqui
	parameters = malloc(sizeof(threadAtt));
	if (parameters == NULL) {
		printf("Falha na alocação de memória! (Parâmetros)\n");
		return 2;
	}
	parameters->size = matrixSide;
	parameters->startID = aux;
	parameters->endID = matrixSide;
	pthread_create(&internalID[i], NULL, multMat, (void*) parameters);
	
	//testa a função de exibir as matrizes
	//showMatrix(matrixSide);
	
	//testa as contas de multiplicação de matrizes
	//multiplica(matrixSide);

	for(i = 0; i<nthreads; i++) {
		if(pthread_join(internalID[i], NULL)) {
			printf("Falha na união das threads!\n");
			return 3;
		}
	}
	
	printf("Tempo gasto: %.0fms\n", timeSpent*1000);
	
	//libera a memória
	free(matrixA);
	free(matrixB);
	free(matrixC);
	free(internalID);
	free(parameters);
	
	return 0;
}
