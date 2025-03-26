#include <stdio.h> //For printf()
#include <pthread.h> //For multi-threading (pthread_create, pthread_join)
#include <unistd.h> //sleep()

volatile int flag[2] = {0, 0};
volatile int turn;

void *process(void *arg) {
    int id = *(int *)arg;
    int other = 1 - id;

    flag[id] = 1;
    turn = other;

    while (flag[other] && turn == other);  // Busy-wait loop

    // Critical Section
    printf("Process %d is in the critical section\n", id);
    sleep(2);
    printf("Process %d is leaving the critical section\n", id);

    flag[id] = 0;  // Exit Section
    return NULL;
}

int main() {
    pthread_t p1, p2;
    int id1 = 0, id2 = 1;

    pthread_create(&p1, NULL, process, &id1);
    pthread_create(&p2, NULL, process, &id2);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    return 0;
}
