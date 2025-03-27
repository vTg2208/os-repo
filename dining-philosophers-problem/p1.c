#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <time.h> 

// Number of philosophers (and forks)
#define NUM_PHILOSOPHERS 5 

// Philosopher states
#define THINKING 0 
#define HUNGRY 1 
#define EATING 2 

// Macros to get left and right neighbors of a philosopher
#define LEFT (phil_id + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS 
#define RIGHT (phil_id + 1) % NUM_PHILOSOPHERS 

// Semaphores and shared data 
sem_t mutex;  // Controls access to critical section
sem_t forks[NUM_PHILOSOPHERS]; // One semaphore per fork
int state[NUM_PHILOSOPHERS];  // Track each philosopher's state

// Function prototypes 
void *philosopher(void *arg); 
void take_forks(int phil_id); 
void put_forks(int phil_id); 
void test(int phil_id); 
void think(int phil_id); 
void eat(int phil_id); 

int main() { 
    pthread_t philosophers[NUM_PHILOSOPHERS]; 
    int phil_ids[NUM_PHILOSOPHERS]; 

    // Seed the random number generator for varied delays 
    srand(time(NULL)); 

    // Initialize semaphores 
    sem_init(&mutex, 0, 1);  // Binary semaphore for critical section
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) { 
        sem_init(&forks[i], 0, 0); // Each fork semaphore starts at 0 (locked)
        state[i] = THINKING; // Initially, all philosophers are thinking
    } 
        
    // Create philosopher threads 
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) { 
        phil_ids[i] = i; // Assign philosopher ID
        if (pthread_create(&philosophers[i], NULL, philosopher, &phil_ids[i]) != 0) { 
            perror("pthread_create failed"); 
            exit(EXIT_FAILURE); 
        } 
    } 
    
    // Wait for philosophers to finish 
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) { 
        if (pthread_join(philosophers[i], NULL) != 0) { 
            perror("pthread_join failed"); 
            exit(EXIT_FAILURE); 
        } 
    } 
    
    // Clean up semaphores 
    sem_destroy(&mutex); 
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) { 
        sem_destroy(&forks[i]); 
    } 
    
    return 0; 
} 

// Function representing philosopher behavior
void *philosopher(void *arg) { 
    int phil_id = *((int *)arg); 
    printf("Philosopher %d has joined the table\n", phil_id); 
 
    // Each philosopher thinks and eats multiple times 
    for (int i = 0; i < 3; i++) { 
        think(phil_id);  // Thinking phase
        take_forks(phil_id);  // Try to acquire forks
        eat(phil_id);  // Eating phase
        put_forks(phil_id);  // Release forks
    } 
 
    printf("Philosopher %d has left the table\n", phil_id); 
    pthread_exit(NULL); 
} 

// Function for acquiring forks
void take_forks(int phil_id) { 
    sem_wait(&mutex);  // Enter critical section
 
    state[phil_id] = HUNGRY;  // Mark philosopher as hungry
    printf("Philosopher %d is hungry\n", phil_id); 
    test(phil_id);  // Check if forks can be acquired
    
    sem_post(&mutex);  // Exit critical section
    sem_wait(&forks[phil_id]); // Wait if forks are not available
} 

// Function for putting down forks
void put_forks(int phil_id) { 
    sem_wait(&mutex);  // Enter critical section
    state[phil_id] = THINKING; // Mark as thinking
    printf("Philosopher %d putting down forks\n", phil_id); 
    
    // Notify neighbors that forks may be available
    test(LEFT); 
    test(RIGHT); 
    
    sem_post(&mutex);  // Exit critical section
} 

// Function to check if a philosopher can eat
void test(int phil_id) { 
    // Philosopher can eat only if not blocked by neighbors
    if (state[phil_id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) { 
        state[phil_id] = EATING; // Change state to eating
        printf("Philosopher %d is eating\n", phil_id); 
        sem_post(&forks[phil_id]); // Allow philosopher to proceed
    } 
} 

// Function simulating thinking
void think(int phil_id) { 
    printf("Philosopher %d is thinking\n", phil_id); 
    usleep((rand() % 500 + 100) * 1000); // Random delay to simulate thinking
} 

// Function simulating eating
void eat(int phil_id) { 
    usleep((rand() % 300 + 100) * 1000); // Random delay to simulate eating
}
