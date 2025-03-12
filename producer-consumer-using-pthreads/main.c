#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"
#define TRUE 1

pthread_mutex_t mutex;
sem_t full, empty;
buffer_item buffer[BUFFER_SIZE];
int counter;

pthread_attr_t attr;

void *producer(void *param);
void *consumer(void *param);
int insert_item(buffer_item item);
int remove_item(buffer_item *item);

void initializeData() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
    pthread_attr_init(&attr);
    counter = 0;
}

void *producer(void *param) {
    buffer_item item;
    while (TRUE) {
        int rNum = rand() % 3 + 1;
        sleep(rNum);
        item = rand();
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (insert_item(item)) {
            fprintf(stderr, "Producer error\n");
        } else {
            printf("Producer produced %d\n", item);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (TRUE) {
        int rNum = rand() % 3 + 1;
        sleep(rNum);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        if (remove_item(&item)) {
            fprintf(stderr, "Consumer error\n");
        } else {
            printf("Consumer consumed %d\n", item);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int insert_item(buffer_item item) {
    if (counter < BUFFER_SIZE) {
        buffer[counter++] = item;
        return 0;
    } else {
        return -1;
    }
}

int remove_item(buffer_item *item) {
    if (counter > 0) {
        *item = buffer[--counter];
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "USAGE: ./main.out <sleep_time> <num_producers> <num_consumers>\n");
        exit(1);
    }

    int mainSleepTime = atoi(argv[1]);
    int numProd = atoi(argv[2]);
    int numCons = atoi(argv[3]);

    initializeData();

    pthread_t producer_threads[numProd];
    pthread_t consumer_threads[numCons];

    for (int i = 0; i < numProd; i++) {
        if (pthread_create(&producer_threads[i], &attr, producer, NULL) != 0) {
            fprintf(stderr, "Error creating producer thread\n");
        }
    }

    for (int i = 0; i < numCons; i++) {
        if (pthread_create(&consumer_threads[i], &attr, consumer, NULL) != 0) {
            fprintf(stderr, "Error creating consumer thread\n");
        }
    }

    sleep(mainSleepTime);
    printf("Exiting program...\n");
    return 0;
}
