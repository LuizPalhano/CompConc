/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define NPROFILES 10000000
#define BASEDATE 20000101

//struct de "perfis" pelos quais o programa itera
typedef struct {
	int birthDate;
	char name[50];
	int surname;
	int isFound;
} Profile;

//struct de atributos passados para cada thread
typedef struct {
	int startIndex;
	int endIndex;
	int threadIndex;
	int begin;
	int end;
} threadAtt;

//global, pois é acessado por todas as threads
Profile * profiles;
int * threadResults;

//tarefa executada pelas threads
void * task (void * arg) {
	threadAtt * local = (threadAtt *) arg;
	int startLocal = local->begin;
	int endLocal = local->end;
	int startIndexLocal = local->startIndex;
	int endIndexLocal = local->endIndex;
	int localIndex = local->threadIndex;
	int currentDate;
	
	threadResults[localIndex] = 0;
	for(int i = startIndexLocal; i<endIndexLocal; i++) {
		currentDate = profiles[i].birthDate;
		if (currentDate >= startLocal && currentDate <= endLocal) {
			profiles[i].isFound = 1;
			threadResults[localIndex] += 1;
		}
		else {
			profiles[i].isFound = 0;
		}
	}
	
	free(local);
	
	pthread_exit(NULL);
}

//função auxiliar de exibição de resultados
void showResults(int start, int end, int nprofiles, int time) {
	printf("Data mínima da procura: %d\n", start);
	printf("Data máxima da procura: %d\n", end);
	printf("Perfis encontrados: %d\n\n", nprofiles);
	if (nprofiles == 0) {
		printf("Tempo gasto: %d ms\n\n", time);
		return;
	}
	//não recomendo descomentar esse bloco com NPROFILES alto
	/*
	for(int i = 0; i < NPROFILES; i++) {
		if(profiles[i].isFound)
			printf("Nome: %s %d\nData de nascimento: %d\n", profiles[i].name, profiles[i].surname, profiles[i].birthDate);
	}*/
	printf("Tempo gasto: %d ms\n\n", time);
}

//o programa recebe uma data inicial e uma data final, e percorre um vetor de dados pessoais fictícios para procurar quais
//dos "perfis" atendem ao requisito desejado (no caso, quem nasceu num determinado período de tempo)
int main (int argc, char *argv[]) {
	int i;							//variável auxiliar
	int aux;						//variável auxiliar
	int nthreads;					//conta quantas threads vão ser usadas
	int threadLoad;					//conta quanto trabalho cada thread vai ter
	threadAtt * att;				//atributos passados para cada thread
	pthread_t * threadIDs;			//identificadores internos de cada thread
	int startDate;					//marca o começo da pesquisa
	int endDate;					//marca o final da pesquisa
	double timeSpent; 				//marca-tempo
	struct timespec start, finish;  //usado pelo marca-tempo
	
	//início do programa
	//inicializa o vetor de perfis
	profiles = malloc(sizeof(Profile) * NPROFILES);
	if(profiles == NULL) {
		printf("Falha na alocação de memória! (Vetor de perfis)\n");
	}
	for(i = 0; i<NPROFILES; i++) {
		profiles[i].birthDate = BASEDATE + (i % 30); //mantém todas as datas dentro de janeiro de 2000
		strcpy(profiles[i].name, "Joao da Silva"); 	 //o nome é irrelevante,
		profiles[i].surname = i;				   	 //mas vale a pena poder identificar de quem é o perfil
	}
	
	//começa a marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &start);
	
	//confere se os argumentos existem
	if (argc < 4) {
		printf("Erro de formatação! Digite <%s> <número de threads> <data inicial> <data final>\n", argv[0]);
		printf("As datas inicial e final devem ser dadas no formato ano-mês-dia. Exemplo: 20210108\n");
		return 1;
	}
	
	//transforma os argumentos em variáveis
	nthreads = atoi(argv[1]);
	startDate = atoi(argv[2]);
	endDate = atoi(argv[3]);
	if (startDate > endDate) {
		aux = endDate;
		endDate = startDate;
		startDate = aux;
	}
	if (nthreads > NPROFILES) {
		nthreads = NPROFILES;
	}
	
	//aloca as memórias necessárias
	threadIDs = malloc(sizeof(pthread_t) * nthreads);
	threadResults = malloc(sizeof(int) * nthreads);
	
	//inicializa as threads
	threadLoad = NPROFILES/nthreads;
	aux = 0;
	for (i = 0; i<nthreads - 1; i++) {
		att = malloc(sizeof(att));
		if (att == NULL) {
			printf("Falha na alocação de memória! (Vetor de atributos)\n");
			return 2;
		}
		att->startIndex = aux;
		att->endIndex = aux + threadLoad;
		att->threadIndex = i;
		att->begin = startDate;
		att->end = endDate;
		if(pthread_create(&threadIDs[i], NULL, task, (void *) att)) {
			printf("Falha na criação das threads!\n");
			return 3;
		}
		aux += threadLoad;
	}
	att = malloc(sizeof(att));
	if (att == NULL) {
		printf("Falha na alocação de memória! (Vetor de atributos)\n");
		return 2;
	}
	att->startIndex = aux;
	att->endIndex = NPROFILES;
	att->threadIndex = i;
	att->begin = startDate;
	att->end = endDate;
	if(pthread_create(&threadIDs[i], NULL, task, (void *) att)) {
		printf("Falha na criação das threads!\n");
		return 3;
	}
	
	//aguarda o retorno das threads
	for (i = 0; i<nthreads; i++) {
		if(pthread_join(threadIDs[i], NULL)) {
			printf("Falha na união das threads!\n");
			return 4;
		}
	}
	
	//processa os resultados das threads
	aux = 0;
	for (i = 0; i<nthreads; i++) {
		aux += threadResults[i];
	}
	
	//libera as memórias alocadas
	free(threadIDs);
	free(threadResults);
	free(profiles);
	
	//termina de marcar o tempo
	clock_gettime(CLOCK_MONOTONIC, &finish);
	timeSpent = (finish.tv_sec - start.tv_sec);
	timeSpent += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	timeSpent *= 1000;
	
	//exibe os resultados
	showResults(startDate, endDate, aux, timeSpent);
	
	return 0;
}
