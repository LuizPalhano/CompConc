/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

//incluindo as bibliotecas necessarias
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

//esse numero pode ser mudado
#define NTHREADS 10

//vetor que tera os valores aumentados (global)
int vetor[NTHREADS];


//task que sera executada por cada thread (aumenta o valor de cada endereço do vetor em 1)
void * task (void *arg) {
	int aux = * (int *) arg;
	vetor[aux]++;

	//cada thread recebe como argumento uma posicao do vetor. Dessa forma, ao incrementar apenas essa posicao
	//por 1 em cada thread, todas as posicoes serao incrementadas em 1.
	
	pthread_exit(NULL); //"retornando" quando a thread acaba
}


int main () {
	pthread_t threadIds[NTHREADS]; //identificadores internos de cada thread
	int threadNames[NTHREADS]; //identificadores para uso na funcao de cada thread
	int i; //auxiliar
	
	//populando o vetor e mostrando os valores antes da incrementacao
	for(i = 0; i<NTHREADS; i++) {
		vetor[i] = 0;
		printf("\nValor da posicao %d do vetor: %d", i, vetor[i]); //mostrando o vetor antes de incrementar
	}
	
	//criando as threads
	for(i = 0; i<NTHREADS; i++) {
		threadNames[i] = i; //identificando as threads para uso proprio
		printf("\nCriando a thread %d", threadNames[i]);
		if(pthread_create(&threadIds[i], NULL, task, (void *) &threadNames[i])) { //criacao das threads
			printf("\nErro no pthread_create!"); //se der erro, entra aqui
			exit(-1);
		}
	}
	
	//esperando as threads terminarem
	for(i = 0; i<NTHREADS; i++) {
		if(pthread_join(threadIds[i], NULL)) {
			printf("\nErro no pthread_join!");
			exit(-1);
		}
	}
	
	//exibindo os resultados apos a incrementacao
	for(i = 0; i<NTHREADS; i++) {
		printf("\nValor da posicao %d do vetor: %d", i, vetor[i]);
	}
	
	printf("\nFim da main");
	
	return 0;
}