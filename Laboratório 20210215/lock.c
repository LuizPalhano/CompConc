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
pthread_mutex_t mutex; //mutex usado nos locks
pthread_cond_t condLeit, condEscr; //variáveis de condição usadas nos locks
int canWrite = 1; //booleano de condição
int canRead = 1; //booleano de condição

//tarefa dos leitores
void * readerTask (void * arg) {
	int i = 0;
	int aux;
	
	printf("Iniciando leitor\n");
	while(i < NITERATIONS) {
		pthread_mutex_lock(&mutex);
		while(!canRead) {
			pthread_cond_wait(&condLeit, &mutex);
		}
		l++;
		if(l == 1) {
			canWrite = 0;
		}
		pthread_mutex_unlock(&mutex);
		
		//lê
		aux = target[nextRead];
		printf("Lido: %d\n", aux);
		nextRead = (nextRead + 1) % TARGETSIZE;
		
		pthread_mutex_lock(&mutex);
		l--;
		if(l == 0) {
			canWrite = 1;
			pthread_cond_signal(&condEscr);
		}
		pthread_mutex_unlock(&mutex);
		
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
		pthread_mutex_lock(&mutex);
		e++;
		canRead = 0;
		while(!canWrite) {
			pthread_cond_wait(&condEscr, &mutex);
		}
		pthread_mutex_unlock(&mutex);
		
		pthread_mutex_lock(&mutex);
		//escreve
		target[nextWrite] = nextWrite % 3;
		printf("Escrito: %d\n", (nextWrite % 3));
		nextWrite = (nextWrite + 1) % TARGETSIZE;

		e--;
		pthread_cond_signal(&condEscr);
		if(e == 0) {
			pthread_cond_broadcast(&condLeit);
			canRead = 1;
		}
		
		pthread_mutex_unlock(&mutex);
		
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
