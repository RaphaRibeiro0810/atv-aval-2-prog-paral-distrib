// Exemplo 1: Leitores e Escritores
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // Incluída para a função sleep

#define NUM_LEITORES 5
#define NUM_ESCRITORES 2

pthread_mutex_t mutex;
sem_t db;
int leitores = 0;
int escritores_esperando = 0;

// Função para threads de leitores
void* leitor(void* id) {
    int tid = *(int*)id;
    while (1) {
        pthread_mutex_lock(&mutex);
        // Verifica se há escritores esperando para dar prioridade a eles
        while (escritores_esperando > 0) {
            pthread_mutex_unlock(&mutex);
            sched_yield(); // Cede a CPU para outras threads
            pthread_mutex_lock(&mutex);
        }
        leitores++;
        if (leitores == 1) sem_wait(&db); // Primeiro leitor bloqueia acesso à escrita
        pthread_mutex_unlock(&mutex);

        // Leitura (simulação)
        printf("Leitor %d está lendo\n", tid);
        sleep(1);

        pthread_mutex_lock(&mutex);
        leitores--;
        if (leitores == 0) sem_post(&db); // Último leitor libera o acesso à escrita
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL); // Finaliza a thread após a execução
}

// Função para threads de escritores
void* escritor(void* id) {
    int tid = *(int*)id;
    while (1) {
        pthread_mutex_lock(&mutex);
        escritores_esperando++;
        pthread_mutex_unlock(&mutex);

        sem_wait(&db); // Escritor espera acesso exclusivo ao recurso

        pthread_mutex_lock(&mutex);
        escritores_esperando--;
        pthread_mutex_unlock(&mutex);

        // Escrita (simulação)
        printf("Escritor %d está escrevendo\n", tid);
        sleep(2);

        sem_post(&db); // Libera o recurso após escrever
    }
    pthread_exit(NULL); // Finaliza a thread após a execução
}

int main() {
    pthread_t threads_leitores[NUM_LEITORES], threads_escritores[NUM_ESCRITORES];
    int ids_leitores[NUM_LEITORES], ids_escritores[NUM_ESCRITORES];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&db, 0, 1);

    // Cria threads de leitores
    for (int i = 0; i < NUM_LEITORES; i++) {
        ids_leitores[i] = i + 1;
        pthread_create(&threads_leitores[i], NULL, leitor, &ids_leitores[i]);
    }

    // Cria threads de escritores
    for (int i = 0; i < NUM_ESCRITORES; i++) {
        ids_escritores[i] = i + 1;
        pthread_create(&threads_escritores[i], NULL, escritor, &ids_escritores[i]);
    }

    // Aguarda a conclusão das threads de leitores
    for (int i = 0; i < NUM_LEITORES; i++) {
        pthread_join(threads_leitores[i], NULL);
    }

    // Aguarda a conclusão das threads de escritores
    for (int i = 0; i < NUM_ESCRITORES; i++) {
        pthread_join(threads_escritores[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&db);

    return 0;
}