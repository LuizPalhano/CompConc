/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TARGETSIZE 10000 //tamanho do vetor que vai ser organizado

int * target; //vetor a ser organizado. Global para poder ser acessado por todas as threads

//estrutura de dados que passa os argumentos para as threads
typedef struct {
	int low;
	int high;
} threadArg;

//função auxiliar usada durante a execução do QuickSort não concorrente
int partition(int low, int high) {
	int pivot = target[high]; //pivô que vai ser comparado durante a execução do algoritmo
	int i = (low - 1); //variável auxiliar
	int aux; //variável auxiliar
	
	for(int j = low; j < high; j++) {
		if(target[j] <= pivot) {
			//se for menor ou igual, vai para a esquerda
			i++;
			aux = target[j];
			target[j] = target[i];
			target[i] = aux;
		}
	}
	//coloca o pivô na posição certa
	aux = target[i + 1];
	target[i + 1] = target[high];
	target[high] = aux;
	
	//retorna o índice do pivô
	return i + 1;
}

//função auxiliar que determina a corretude do sort
void showResults(double time) {
	int aux = target[0];
	for(int i = 1; i < TARGETSIZE; i++) {
		if(aux > target[i]) {
			printf("Falha no sort.\n");
			return;
		}
		aux = target[i];
	}
	printf("Sort realizado corretamente.\n");
	printf("Tempo gasto: %.0fms\n", time);
}

//QuickSort sequencial, usado tanto sozinho quanto dentro da versão concorrente
void quickSort(int low, int high) {
	int index;
	
	//só faz o sort se o tamanho do array a ser sortado não é 1 (se fosse, low seria igual a high)
	if(low < high) {
		index = partition(low, high);
		
		//resolve os menores ou iguais
		quickSort(low, index - 1);
		//resolve os maiores
		quickSort(index + 1, high);
	}
}

//QuickSort concorrente é a tarefa a ser executada pelas threads.
//Não recebe como argumento vetor a ser sortado porque este é global
void * quickTask (void * arg) {
	int index; //índice do pivô usado na execução do algoritmo
	threadArg * localArg = (threadArg *) arg;
	int small = localArg->low; //recebe o índice baixo de dentro dos argumentos
	int big = localArg->high; //recebe o índice alto de dentro dos argumentos
	pthread_t leftID; //ID interno da thread subsequente, para ser usado se necessário
	threadArg * leftArg; //passa argumentos para a thread subsequente
	
	//executa o QuickSort normalmente, criando threads se necessário
	if(small < big) {
		index = partition(small, big); //como a criação da nova thread só acontece depois dessa linha terminar, só tem uma 
									   //thread mexendo no vetor inteiro nesse momento. Depois, quando a threads é criada,
									   //ela mexe sozinha no vetor de novo, evitando condição de corrida
		//aloca e define quais são os argumentos da nova thread
		leftArg = malloc(sizeof(threadArg));
		if(leftArg == NULL) {
			printf("Falha na criação de argumentos. (Thread subsequente)\n");
			exit(-1);
		}
		leftArg->low = small;
		leftArg->high = index - 1;
		
		//cria a nova thread para organizar um lado do vetor
		if(pthread_create(&leftID, NULL, quickTask, (void *) leftArg)) {
			printf("Falha na criação das threads. (Thread subsequente)\n");
			exit(-1);
		}
		//organiza o outro lado do vetor sequencialmente
		quickSort(index + 1, big);
		
	}
	
	//não é necessário esperar a execução da thread subsequente terminar
	pthread_exit(NULL);
}

//main usada para testar a execução dos dois algoritmos. Só um dos dois deve estar descomentado a qualquer momento
int main () {
	pthread_t firstID; //ID da primeira thread, usada para iniciar o QuickSort
	threadArg * firstArg; //argumentos da primeira thread
	double timeSpent; //marca-tempo
	struct timespec start, finish; //usado pelo marca-tempo
	
	//aloca e inicializa o vetor a ser sortado
	target = malloc(sizeof(int) * TARGETSIZE);
	if(target == NULL) {
		printf("Falha na alocação de memória. (Vetor alvo)\n");
		return 1;
	}
	for(int i = 0; i < TARGETSIZE; i++) {
		target[i] = i % 7;
	}
	
	/*
	//usando o QuickSort sequencial
	//começa a marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &start);
	quickSort(0, TARGETSIZE);
	//termina de marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &finish);
	timeSpent = (finish.tv_sec - start.tv_sec);
	timeSpent += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	timeSpent *= 1000;
	showResults(timeSpent);
	*/
	
	//usando o QuickSort concorrente
	//começa a marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &start);
	firstArg = malloc(sizeof(threadArg));
	if(firstArg == NULL) {
		printf("Falha na criação de argumento. (Thread inicial)\n");
		return 1;
	}
	firstArg->low = 0;
	firstArg->high = TARGETSIZE;
	
	if(pthread_create(&firstID, NULL, quickTask, (void *) firstArg)) {
		printf("Falha na criação das threads. (Thread inicial)\n");
		return 2;
	}
	
	if(pthread_join(firstID, NULL)) {
		printf("Falha na união das threads. (Thread inicial)\n");
		return 3;
	}
	
	//termina de marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &finish);
	timeSpent = (finish.tv_sec - start.tv_sec);
	timeSpent += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	timeSpent *= 1000;
	showResults(timeSpent);
	
	//libera as memórias
	free(target);
	free(firstArg);
	
	return 0;
}
