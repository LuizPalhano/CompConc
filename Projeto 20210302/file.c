/*
Nome: Luiz Henrique Gopfert Palhano Leal
DRE: 118061663
CompConc 2020.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//TODO
//Implementar as três tasks

//CONSTANTES
//buffer com m entradas
#define BUFFERSLOTS 10
//blocos de tamanho n
#define SLOTSIZE 10

//ESTRUTURA DE DADOS
//estrutura de dados que guarda as respostas das threads ao terminarem de executar
typedef struct {
	//usado para indicar a posição inicial da sequência na thread de sequência
	int startPos;
	//usado para indicar o tamanho da sequência na thread de sequência
	int size;
	//usado por todas as threads para as respostas
	int value;
} answerType;

//VARIÁVEIS GLOBAIS
//estruturas de dados que vão guardar as respostas das três threads
answerType sameValue;
answerType triplets;
answerType flush;
//buffer usado pelas threads. Tratado como uma matriz m por n
int *buffer;
//vetor de tamanho m que é atualizado pelas três threads: main inicializa cada posição com -1.
//Depois que a posição termina de receber o input do arquivo, a própria main coloca que aquela posição vale 0 e as threads que estavam
//esperando a posição se tornar 0 são acordadas. Depois que cada thread termina a execução, o valor da posição que a thread acabou de
//analisar é incrementado em 1, a um máximo de 3. Quando o 3 é alcançado, dentro da thread que terminou aquela posição é marcada novamente com -1 e a
//posição passa a poder ser preenchida novamente. Quando a leitura do arquivo terminar, o isFinished é trocado de falso para verdadeiro, indicando
//que quando as threads chegarem numa posição do buffer que está marcada com -1, a thread termina.
int *positionAvailability;
//usado para indicar que o arquivo terminou
int isFinished = 0;
//usados durante as partes de sincronização
pthread_mutex_t mutex;
pthread_cond_t sleep;

//TAREFAS DAS THREADS
//tarefa da thread que computa o maior número de ocorrências seguidas de um número
void * sameValueTask (void * arg) {
	pthread_exit(NULL);
}

//tarefa da thread que busca trincas
void * tripletsTask (void * arg) {
	pthread_exit(NULL);
}

//tarefa da thread que busca a sequência 012345
void * flushTask (void * arg) {
	pthread_exit(NULL);
}

//FUNÇÃO AUXILIAR
//função auxiliar para exibir os resultados
void showResults() {
	printf("Maior sequência de valores idênticos: %d %d %d\n", sameValue.startPos, sameValue.size, sameValue.value);
	printf("Quantidade de triplas: %d\n", triplets.value);
	printf("Quantidade de ocorrências da sequência <012345>: %d\n", flush.value);
}

//MAIN
int main () {
	FILE *fp; //ponteiro do arquivo
	pthread_t threadIDs[3]; //vetor de thread ID's
	long long int * placeHolder; //guarda valores temporariamente durante a leitura
	long long int fileSize; //guarda o número de inteiros na sequência
	long long int currentSize = 0; //usado para a thread principal saber quando parar
	int * bufferWorker; //passa os conteúdos do arquivo para o buffer
	int currentRow = 0; //usado na execução do buffer para a main saber em qual linha deve escrever
	int aux; //variável auxiliar
	int i; //variável auxiliar
	
	//abre o arquivo
	fp = fopen("arquivo.bin", "rb");
	if(fp == NULL) {
		printf("Falha na abertura do arquivo!\n");
		return 1;
	}
	
	//inicializa as threads
	if(pthread_create(&threadIDS[0], NULL, sameValueTask, NULL)) {
		printf("Falha na criação das threads! \n");
		return 2;
	}
	if(pthread_create(&threadIDS[1], NULL, tripletsTask, NULL)) {
		printf("Falha na criação das threads! \n");
		return 2;
	}
	if(pthread_create(&threadIDS[2], NULL, flushTask, NULL)) {
		printf("Falha na criação das threads! \n");
		return 2;
	}
	
	//inicializa o buffer e os seus auxiliares
	buffer = malloc(sizeof(int) * BUFFERSLOTS * SLOTSIZE);
	if(buffer == NULL) {
		printf("Falha na alocação de memória! (Buffer)\n");
		return 3;
	}
	//passa os valores para dentro do buffer
	bufferWorker = malloc(sizeof(int) * SLOTSIZE);
	if(bufferWorker == NULL) {
		printf("Falha na alocação de memória! (Auxiliar do buffer)\n");
		return 3;
	}
	//guarda se cada slot do buffer pode ser usado
	positionAvailability = malloc(sizeof(int) * BUFFERSLOTS);
	if(positionAvailability == NULL) {
		printf("Falha na alocação de memória! (Vetor de posições)\n");
		return 3;
	}
	for(i = 0; i < BUFFERSLOTS; i++) {
		positionAvailability[i] = -1;
	}
	
	//executa o buffer
	aux = fread(placeHolder, sizeof(long long int), 1, fp);
	if(aux != 1) {
		printf("Falha na leitura do arquivo!\n");
		return 4;
	}
	fileSize = *placeHolder;
	//itera pelo arquivo até ter guardado tudo no buffer
	while(currentSize < fileSize) {
		while(positionAvaliability[currentRow] != -1) { //espera até poder escrever mais, caso o buffer esteja cheio
			;											//(dentro das threads a disponibilidade é colocada em -1 quando todas tiverem terminado)
		}
		
		//se ainda tiver tamanho o suficiente no arquivo, puxa n bytes
		if(currentSize + SLOTSIZE <= fileSize) {
			//tenta ler n itens do arquivo
			aux = fread(bufferWorker, sizeof(int), SLOTSIZE, fp);
			if(aux != 1) {
				printf("Falha na leitura do arquivo!\n");
				return 4;
			}
			//guarda os itens dentro do buffer, uma linha de cada vez
			for(i = 0; i < SLOTSIZE; i++) {
				buffer[currentRow * SLOTSIZE + i] = bufferWorker[i];
			}
			
			pthread_mutex_lock(&mutex);
			positionAvailability[currentRow] = 0;
			pthread_cond_broadcast(&sleep);
			pthread_mutex_unlock(&mutex);
			
			currentRow = (currentRow + 1) % BUFFERSLOTS;
			currentSize += SLOTSIZE;
		}
		
		//se não tiver, puxa bytes até o final do arquivo
		else {
			i = 0;
			while(fread(placeHolder, sizeof(int), 1, fp) == 1) { //lê até o final do arquivo, guardando os valores em bufferWorker
				bufferWorker[i] = *placeHolder;
				i++;
			}
			//indica para as threads que não há mais dados a serem lidos naquela linha, porque o arquivo acabou
			bufferWorker[i] = -1;
		}
	}
	
	//indica que o arquivo terminou
	pthread_mutex_lock(&mutex);
	isFinished = 1;
	pthread_mutex_unlock(&mutex);
	
	//espera as threads terminarem
	for(i = 0; i < 3; i++) {
		if(pthread_join(threadIDs[i], NULL)) {
			printf("Falha na união das threads!\n");
			return 5;
		}
	}
	
	//libera o que foi aberto e alocado
	fclose(fp);
	free(buffer);
	free(bufferWorker);
	free(positionAvailability);
	
	//exibe os resultados
	showResults();
	
	return 0;
}
