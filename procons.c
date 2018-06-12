/*
 * Producer/Consumer exploration with pthreads, mutexes and semaphores
 *
 * pthread usage: create, detach, mutex, semaphore
 *
 * Lessons:
 *      threads may term when parent terms, but in time while parent is exiting threads will continue to run producing erratic behavior
 *      have main secure mutex (or other protective struct) halting thread work, before term main
 *      have pro/con wait semaphore before securing mutex, and release mutex before post semaphore
 *      detached threads without logic for exiting will not free their memory when parent term
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// static data
enum {
    BUFF_SIZE = 10,
    MAX_PROS = 8,
    MAX_CONS = 4,
};

// static data structures
int nextin, nextout, KILL;
char buffer[BUFF_SIZE];

pthread_t producers[MAX_PROS];
pthread_t consumers[MAX_CONS];
pthread_mutex_t m;
sem_t empty, occupied;


// producer routine
void *produce(void *prod) {
    for(;;) {
        sem_wait(&empty);
        pthread_mutex_lock(&m);
        
        buffer[nextin] = *(char *)prod;
        nextin = (nextin + 1) % BUFF_SIZE;
        
        pthread_mutex_unlock(&m);
        sem_post(&occupied);
    }
}

// consumer routine
void *consume(void *cons) {
    for(;;) {
        sem_wait(&occupied);
        pthread_mutex_lock(&m);
        
        fprintf(stdout, "%c : %c\n", *(char *)cons, buffer[nextout]);
        nextout = (nextout + 1) % BUFF_SIZE;
        
        pthread_mutex_unlock(&m);
        sem_post(&empty);
    }
}


int main() {
    char prod_values[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    char cons_values[] = {'1', '2', '3', '4'};
    
    int i;
    
    // init mutex and semaphores
    pthread_mutex_init(&m, NULL);
    sem_init(&empty, 0, BUFF_SIZE);
    sem_init(&occupied, 0, 0);
    
    // create / detach consumer threads
    for(i = 0; i < MAX_CONS; i++) {
        if(pthread_create(&consumers[i], NULL, consume, &cons_values[i])) {
            fprintf(stdout, "Error creating pthread, exiting\n");
            exit(1);
        }
        
        pthread_detach(consumers[i]);
    }
    
    // create / detach producer threads
    for(i = 0; i < MAX_PROS; i++) {
        if(pthread_create(&producers[i], NULL, produce, &prod_values[i])) {
            fprintf(stdout, "Error creating pthread, exiting\n");
            exit(1);
        }
        
        pthread_detach(producers[i]);
    }
    
    // delay return : allow threads to run
    usleep(10000);
    // enqueue main to grab control of mutex : stoping threads work
    pthread_mutex_lock(&m);
    pthread_mutex_destroy(&m);
    sem_destroy(&empty);
    sem_destroy(&occupied);
    return 0;
}
