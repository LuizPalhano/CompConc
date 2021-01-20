/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

//TODO
//Criar as três versões tarefas dos escritores e dos leitores (esqueleto do código já tá pronto)
//OBS: O que muda são as funções auxiliares enter e exit
//Testar se fazer (do jeito que a Silvana fez como base) para o normal, (mesmo código mas espelhado) para a prioridade invertida e
//(botar todo mundo na mesma fila de espera, só resolve uma thread de cada vez) resolve o problema

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

//dados predefinidos
#define NOTEPAD_SIZE 50
#define NTHREADS 4
#define READER_ITERATIONS 20
#define WRITER_ITERATIONS 20

//variáveis compartilhadas
int * notepad;
pthread_cond_t queueVar;
int workerCount;
pthread_mutex_t mutex;

//passa atributos para os escritores
typedef struct {
	int threadNumber;
} writerAtt;

//funções auxiliares. Prioridades iguais
void enterReader () {
	pthread_mutex_lock(&mutex);
	while(workerCount > 0) {
		pthread_cond_wait(&queueVar, &mutex);
	}
	workerCount++;
	pthread_mutex_unlock(&mutex);
}

void enterWriter () {
	pthread_mutex_lock(&mutex);
	while(workerCount > 0) {
		pthread_cond_wait(&queueVar, &mutex);
	}
	workerCount++;
	pthread_mutex_unlock(&mutex);
}

void exitReader () {
	pthread_mutex_lock(&mutex);
	workerCount--;
	pthread_cond_signal(&queueVar);
	pthread_mutex_unlock(&mutex);
}

void exitWriter () {
	pthread_mutex_lock(&mutex);
	workerCount--;
	pthread_cond_signal(&queueVar);
	pthread_mutex_unlock(&mutex);
}

//tarefa a ser executada pelos leitores
void * reader (void * arg) {
	int i = 0;
	int aux;
	float total;
	
	while(i < READER_ITERATIONS) {
		enterReader();
		total = 0;
		for(aux = 0; aux<NOTEPAD_SIZE; aux++) {
			total += notepad[aux];
			printf("Total lido: %.0f\n", total);
			printf("Média atual dos valores lidos: %.2f\n", total/(aux + 1));
		}
		printf("\n");
		exitReader();
		
		i++;
	}

	pthread_exit(NULL);
}

//tarefa a ser executada pelos escritores
void * writer (void * arg) {
	writerAtt * localArg = (writerAtt *) arg;
	int number = localArg->threadNumber;
	int i;
	
	while(i < WRITER_ITERATIONS) {
		enterWriter();
		for(i = 0; i<NOTEPAD_SIZE; i++) {
			if(i == 0 || i == NOTEPAD_SIZE - 1) {
				notepad[i] = number;
			}
			else {
				notepad[i] = 2*number;
			}
		}
		exitWriter();
		
		i++;
	}
	
	free(localArg);
	pthread_exit(NULL);
}

//main
int main () {
	pthread_t * threadIDs;  //vetor dos IDs internos das threads
	int i; 					//variável auxiliar
	writerAtt * att; 		//passa parâmetros para os escritores
	
	//aloca as memórias necessárias
	threadIDs = malloc(sizeof(pthread_t) * NTHREADS);
	if (threadIDs == NULL) {
		printf("Falha na alocação de memória! (Thread IDs)\n");
		return 1;
	}
	notepad = malloc(sizeof(int) * NOTEPAD_SIZE);
	if (notepad == NULL) {
		printf("Falha na alocação de memória! (Área compartilhada)\n");
		return 1;
	}
	
	//inicializa a área compartilhada, caso os leitores cheguem primeiro
	for(i = 0; i<NOTEPAD_SIZE; i++) {
		notepad[i] = 0;
	}
	
	//cria as threads
	for(i = 0; i<NTHREADS; i++) {
		if (i%2 == 0) {
			//leitores
			if (pthread_create(&threadIDs[i], NULL, reader, NULL)) {
				printf("Falha na criação de thread! (Leitor)\n");
				return 2;
			}
		}
		
		else {
			//escritores
			att = malloc(sizeof(writerAtt));
			att->threadNumber = i;
			if(pthread_create(&threadIDs[i], NULL, writer, (void *) att)) {
				printf("Falha na criação de thread! (Escritor)\n");
				return 2;
			}
		}
	}
	
	//espera as threads
	for(i = 0; i<NTHREADS; i++) {
		if(pthread_join(threadIDs[i], NULL)) {
			printf("Falha na união das threads! \n");
			return 3;
		}
	}
	
	//libera as memórias
	free(threadIDs);
	free(notepad);
	
	return 0;
}
