/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

//todo
//computar a soma a partir do vetor de resultados (no pthread_join())
//PROBLEMA: SEQUENCIAL NÃO PROCESSA COM SIZE=10^9 (congela o computador)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

long int size; //tamanho do vetor do somatório
int nthreads;  //número de threads
double *array; //vetor de entrada (com tamanho size)
double *threadResults; //vetor para guardar os resultados das threads

typedef struct {
	int startPoint;
	int endPoint;
	int index;
} threadAtt;

//tarefa a ser executada por cada thread
void * task (void * arg) {
	threadAtt *local = (threadAtt*) arg;
	int startIndex = local->startPoint;
	int endIndex = local->endPoint;
	int localIndex = local->index;
	double sum = 0;
	
	for(long int i = startIndex; i >= endIndex; i--) {
		sum -= array[i];
	}
	
	threadResults[localIndex] = sum;
	
	pthread_exit(NULL);
}

//exibe os resultados
void showResults (double result, double time) {
	double error;
	if(M_PI > result) {
		error = M_PI - result;
	} else {
		error = result - M_PI;
	}
	printf("Resultado do somatório: %.15f\n", result);
	printf("Erro numérico: %.15f\n", error);
	printf("Tempo total gasto: %.0fms\n", time);
}

//função main
int main (int argc, char *argv[]) {
	long int i; 					//usado em iterações
	double timeSpent; 				//marca-tempo
	struct timespec start, finish;  //usado pelo marca-tempo
	double result = 0; 				//usado para guardar o resultado
	long int aux;					//variável auxiliar
	long int threadLoad;			//variável auxiliar
	pthread_t *threadIDs;			//guarda os IDs internos das threads
	threadAtt *att;					//argumentos passados para cada thread
	
	//início do programa
	clock_gettime(CLOCK_MONOTONIC, &start);
	
	//valida os parâmetros de entrada
	if (argc < 3) {
		printf("Erro de formatação!\nDigite: %s, <tamanho do vetor>, <número de threads usadas>\n", argv[0]);
		return 1;
	}
	
	//recebe os parâmetros de entrada
	size = atoll(argv[1]);
	nthreads = atoi(argv[2]);
	
	//inicializa e valida o vetor do somatório
	array = (double *) malloc(sizeof(double) * size);
	if (array == NULL) {
		printf("Falha na alocação de memória! (Vetor do somatório)\n");
		return 2;
	}
	//cria o vetor do somatório direto
	for(i = 0; i<size; i++) {
		if(i%2 != 0) {
			array[i] = 1.0/(1.0 + (2.0*i));
		} else {
			array[i] = (-1.0)/(1.0 + (2.0*i));
		}
	}
	
	//aloca e valida o vetor de threadIDs
	threadIDs = malloc(sizeof(pthread_t) * nthreads);
	if (threadIDs == NULL) {
		printf("Falha na alocação de memória! (Vetor de thread IDs)\n");
		return 2;
	}
	
	/*//sequencial
	//começa pelo final para evitar erro de truncamento
	for(i = size; i>-1; i--) {
		result -= array[i];
	}
	result *= 4;
	*/
	
	//concorrente
	threadLoad = size/nthreads;
	aux = size;
	
	//aloca o vetor de respostas
	threadResults = malloc(sizeof(double) * nthreads);
	
	//cria as threads
	for (i = 0; i<nthreads - 1; i++) {
		att = malloc(sizeof(att));
		if (att == NULL) {
			printf("Falha na alocação de memória! (Vetor de atributos)\n");
			return 2;
		}
		att->startPoint = aux;
		att->endPoint = aux - threadLoad;
		if(pthread_create(&threadIDs[i], NULL, task, (void *) att)) {
			printf("Falha na criação das threads!\n");
			return 3;
		}
		aux -= threadLoad;
	}
	att = malloc(sizeof(att));
	if (att == NULL) {
		printf("Falha na alocação de memória! (Vetor de atributos)\n");
		return 2;
	}
	att->startPoint = aux;
	att->endPoint = 0;
	if(pthread_create(&threadIDs[i], NULL, task, (void *) att)) {
		printf("Falha na criação das threads!\n");
		return 3;
	}
	
	//aguarda as threads
	for(i = 0; i<nthreads; i++) {
		if(pthread_join(threadIDs[i], NULL)) {
			printf("Falha na união das threads!\n");
			return 3;
		}
	}
	
	//computa os resultados
	for (i = 0; i<nthreads; i++) {
		result += threadResults[i];
	}
	result *= 4;
	
	
	free(att);
	free(array);
	free(threadIDs);
	free(threadResults);
	
	
	//termina de marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &finish);
	timeSpent = (finish.tv_sec - start.tv_sec);
	timeSpent += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	timeSpent *= 1000;
	
	//exibe os resultados
	showResults(result, timeSpent);
	
	return 0;
}
