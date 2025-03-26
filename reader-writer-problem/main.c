#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TRUE 1
#define NUM_READERS 4
#define NUM_WRITERS 2
#define RAND_DIVISOR 100000000

/* Global variables */
int shared_value = 0;
int readers_count = 0;
int writer_active = 0;
int waiting_writers = 0;

/* Mutex and condition variables */
pthread_mutex_t mutex;
pthread_cond_t read_phase, write_phase;

pthread_t tid;
pthread_attr_t attr;

void initializeData() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&read_phase, NULL);
    pthread_cond_init(&write_phase, NULL);
}

void reader_enter() {
    pthread_mutex_lock(&mutex);
    while (writer_active || waiting_writers > 0) {
        pthread_cond_wait(&read_phase, &mutex);
    }
    readers_count++;
    pthread_mutex_unlock(&mutex);
}

void reader_exit() {
    pthread_mutex_lock(&mutex);
    readers_count--;
    if (readers_count == 0 && waiting_writers > 0) {
        pthread_cond_signal(&write_phase);
    }
    pthread_mutex_unlock(&mutex);
}

void writer_enter() {
    pthread_mutex_lock(&mutex);
    waiting_writers++;
    while (readers_count > 0 || writer_active) {
        pthread_cond_wait(&write_phase, &mutex);
    }
    waiting_writers--;
    writer_active = 1;
    pthread_mutex_unlock(&mutex);
}

void writer_exit() {
    pthread_mutex_lock(&mutex);
    writer_active = 0;
    if (waiting_writers > 0) {
        pthread_cond_signal(&write_phase);
    } else {
        pthread_cond_broadcast(&read_phase);
    }
    pthread_mutex_unlock(&mutex);
}

void *reader(void *param) {
    int id = *((int *)param);
    while (TRUE) {
        reader_enter();
        printf("Reader %d reading value: %d\n", id, shared_value);
        usleep(rand() / RAND_DIVISOR);
        reader_exit();
        printf("Reader %d thinking...\n", id);
        usleep(rand() / RAND_DIVISOR);
    }
}

void *writer(void *param) {
    int id = *((int *)param);
    while (TRUE) {
        writer_enter();
        shared_value++;
        printf("Writer %d writing value: %d\n", id, shared_value);
        usleep(rand() / RAND_DIVISOR);
        writer_exit();
        printf("Writer %d thinking...\n", id);
        usleep(rand() / RAND_DIVISOR);
    }
}

int main(int argc, char *argv[]) {
    int i;
    initializeData();

    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    for (i = 0; i < NUM_READERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&readers[i], &attr, reader, id);
    }

    for (i = 0; i < NUM_WRITERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&writers[i], &attr, writer, id);
    }

    sleep(10);  // Run for 10 seconds, then exit
    printf("Exit the program\n");
    exit(0);
}
