/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4
#define NITERATIONS 50
#define TARGETSIZE 100

int e = 0; //conta o número de escritores
int l = 0; //conta o número de leitores
int * target; //vetor compartilhado
int nextWrite = 0; //próxima posição a ser escrita
int nextRead = 0; //próxima posição a ser lida
sem_t em_e, em_l, escr, leit; //semáforos

//tarefa dos leitores
void * readerTask (void * arg) {
	int i = 0;
	int aux;
	
	printf("Iniciando leitor\n");
	while(i < NITERATIONS) {
		sem_wait(&leit);
		sem_wait(&em_l);
		l++;
		if(l == 1) {
			sem_wait(&escr);
		}
		sem_post(&em_l);
		sem_post(&leit);
		
		//lê
		aux = target[nextRead];
		printf("Lido: %d\n", aux);
		nextRead = (nextRead + 1) % TARGETSIZE;
		
		sem_wait(&em_l);
		l--;
		if(l == 0) {
			sem_post(&escr);
		}
		sem_post(&em_l);
				
		i++;
	}
	
	printf("Terminando leitor\n");
	pthread_exit(NULL);
}

//tarefa dos escritores
void * writerTask (void * arg) {
	int i = 0;
	
	printf("Iniciando escritor\n");
	while(i < NITERATIONS) {
		sem_wait(&em_e);
		e++;
		if(e == 1) {
			sem_wait(&leit);
		}
		sem_post(&em_e);
		sem_wait(&escr);
		
		//escreve
		target[nextWrite] = nextWrite % 3;
		printf("Escrito: %d\n", (nextWrite % 3));
		nextWrite = (nextWrite + 1) % TARGETSIZE;
		
		sem_post(&escr);
		sem_wait(&em_e);
		e--;
		if(e == 0) {
			sem_post(&leit);
		}
		sem_post(&em_e);
		
		i++;
	}
	
	printf("Terminando escritor\n");
	pthread_exit(NULL);
}

//main
int main () {
	pthread_t * threadIDs;
	int i;
	
	//aloca os vetores
	target = malloc(sizeof(int) * TARGETSIZE);
	if(target == NULL) {
		printf("Falha na alocação de memória! (Área compartilhada)\n");
		return 1;
	}
	threadIDs = malloc(sizeof(pthread_t) * NTHREADS);
	if(threadIDs == NULL) {
		printf("Falha na alocação de memória! (Vetor de thread IDs)\n");
		return 1;
	}
	
	//inicializa a área compartilhada
	for(i = 0; i < TARGETSIZE; i++) {
		target[i] = 0;
	}
	
	//inicializa os semáforos
	printf("Inicializando o semáforo de exclusão mútua dos escritores\n");
	sem_init(&em_e, 0, 1);
	printf("Inicializando o semáforo de exclusão mútua dos leitores\n");
	sem_init(&em_l, 0, 1);
	printf("Inicializando o semáforo dos escritores\n");
	sem_init(&escr, 0, 1);
	printf("Inicializando o semáforo dos leitores\n");
	sem_init(&leit, 0, 1);
	
	//cria as threads
	for(i = 0; i < NTHREADS; i++) {
		if (i%2 == 0) {
			//criando leitores
			if (pthread_create(&threadIDs[i], NULL, readerTask, NULL)) {
				printf("Falha na criação de thread! (Leitor)\n");
				return 2;
			}
		}
		
		else {
			//criando escritores
			if(pthread_create(&threadIDs[i], NULL, writerTask, NULL)) {
				printf("Falha na criação de thread! (Escritor)\n");
				return 2;
			}
		}
	}
	
	//espera as threads
	for(i = 0; i < NTHREADS; i++) {
		if(pthread_join(threadIDs[i], NULL)) {
			printf("Falha na união das threads!\n");
			return 3;
		}
	}
	
	//libera as memórias alocadas
	free(target);
	free(threadIDs);
	
	//fim
	return 0;
}
