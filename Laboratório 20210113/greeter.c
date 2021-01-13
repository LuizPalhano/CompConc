/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

#include <stdio.h>
#include <pthread.h>

//variáveis utilizadas
//tranca a liberação das threads 3 e 4 até as threads 1 e 2 terem terminado
int lock1 = 1;
//permite que as threads 3 e 4 sejam liberadas
int lock2 = 1;
//usadas para as threads 3 e 4 esperarem a 1 e a 2 executarem
pthread_mutex_t mutex;
pthread_cond_t condVar;

//tarefa a ser executada pela thread 1
void * task1 (void * arg) {
	printf("Tudo bem?\n");
	
	lock1 = 0;
	
	pthread_exit(NULL);
}

//tarefa a ser executada pela thread 2
void * task2 (void * arg) {
	printf("Bom dia!\n");
	
	//thread 2 espera a thread 1 terminar
	while(lock1) {
		;
	}
	
	//thread 2 libera as threads 3 e 4 quando ela e a 1 já terminaram
	lock2 = 0;
	
	pthread_cond_broadcast(&condVar);
	
	pthread_exit(NULL);
}

//tarefa a ser executada pela thread 3
void * task3 (void * arg) {
	while (lock2) { //wait dentro de um loop de condição
		pthread_cond_wait(&condVar, &mutex);
	}
	
	printf("Até mais!\n");
	
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
}

//tarefa a ser executada pela thread 4
void * task4 (void * arg) {
	while (lock2) { //wait dentro de um loop de condição
		pthread_cond_wait(&condVar, &mutex);
	}
	
	printf("Boa tarde!\n");
	
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
}

//função principal
int main () {
	pthread_t threadIDS[4]; //vetor de thread ID's
	
	//inicializa as threads
	if(pthread_create(&threadIDS[0], NULL, task1, NULL)) {
		printf("Falha na criação das threads! \n");
		return 1;
	}
	if(pthread_create(&threadIDS[1], NULL, task2, NULL)) {
		printf("Falha na criação das threads! \n");
		return 1;
	}
	if(pthread_create(&threadIDS[2], NULL, task3, NULL)) {
		printf("Falha na criação das threads! \n");
		return 1;
	}
	if(pthread_create(&threadIDS[3], NULL, task4, NULL)) {
		printf("Falha na criação das threads! \n");
		return 1;
	}
	
	//espera as threads terminarem
	pthread_exit(NULL);
}
