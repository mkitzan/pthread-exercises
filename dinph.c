/*
 * Dining Philosophers exploration with pthreads
 * 
 * pthread usage: create, detach, semaphore
 *
 * Lessons:
 *      clever use of synchronization tools can reduce the amount of them needed
 *      mutexes do not garuntee a solution to the CS problem
 *      synchronization complexity increases with quantity of synchronization tools 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// static data
enum {
    MAX_THREADS = 5,
};

#define HUNGRY      "HUNGRY"
#define EATING      "       EATING"
#define THINKING    "              THINKING"

// user defined types
typedef struct arg arg_t;
struct arg {
    int right;
    int left;
};

// static data strcutures
pthread_t philosophers[MAX_THREADS];
arg_t params[MAX_THREADS];
sem_t chopsticks[MAX_THREADS];


// initialize semaphores
void set_semaphores() {
    int i;
    
    for(i = 0; i < MAX_THREADS; i++) sem_init(&chopsticks[i], 0, 1);
}

// destroy semaphores
void destroy_semaphores() {
    int i;
    
    for(i = 0; i < MAX_THREADS; i++) sem_destroy(&chopsticks[i]);
}

// seed the argument array with chopsitck position values
void set_args() {
    int i;
    
    params[0].right = 0;
    params[0].left = MAX_THREADS-1;
    
    for(i = 1; i < MAX_THREADS; i++) {
        params[i].right = i;
        params[i].left = i-1;
    }
}

// aqcuire two specific chopsticks
void pickup(arg_t param) {
    if(param.right % 2) {
        sem_wait(&chopsticks[param.right]);
        sem_wait(&chopsticks[param.left]);
    } else {
        sem_wait(&chopsticks[param.left]);
        sem_wait(&chopsticks[param.right]);
    }
}

// relinquish the two aqcuired chopsticks 
void release(arg_t param) {
    if(param.right % 2) {
        sem_post(&chopsticks[param.right]);
        sem_post(&chopsticks[param.left]);
    } else {
        sem_post(&chopsticks[param.left]);
        sem_post(&chopsticks[param.right]);
    }
}

// thread routine
void *philosopher(void *arg) {
    arg_t param = *(arg_t *)arg;
    
    for(;;) {
        fprintf(stdout, "Phl %d: %s\n", param.right, HUNGRY);
        
        pickup(param);
        
        fprintf(stdout, "Phl %d: %s\n", param.right, EATING);
        
        release(param);
        
        fprintf(stdout, "Phl %d: %s\n", param.right, THINKING);
        
        // allow time for others to run
        usleep(100);
    }
}


int main() {
    int i;
    
    set_semaphores();
    set_args();
    
    // create / detach threads
    for(i = 0; i < MAX_THREADS; i++) {
        if(pthread_create(&philosophers[i], NULL, philosopher, &params[i])) {
            fprintf(stderr, "Error creating thread, exiting\n");
        }
        
        pthread_detach(philosophers[i]);
    }
    
    // wait for proc to run
    usleep(10000);
    // close semaphores
    destroy_semaphores();
    return 0;
}


