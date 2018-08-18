#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


pthread_t tobacconist;
pthread_t smokers[3];
sem_t smoker_line, tobacco_man, hold_up;

int PAPER   = 1;
int TOBACCO = 2;
int MATCHES = 4;
int TABLE_SET = 0;


void *smoker_thread(void *args) {
    int item = *(int *) args;
    
    for(;;) {
        sem_wait(&smoker_line);
        
        if(TABLE_SET == 0) {
            sem_post(&tobacco_man);
            sem_wait(&hold_up);
        }
        
        if(TABLE_SET + item == 7) {
            TABLE_SET = 0;
            printf("Smoker %d\n", item);
        }
        
        sem_post(&smoker_line);
        
        usleep(10);
    }
} 


void *tobacconist_thread(void *args) {
    int sets[3] = {6, 5, 3};
    int next = 2;
    
    for(;;) {
        sem_wait(&tobacco_man);
        TABLE_SET = sets[next = (next + 1) % 3];
        printf("Tobacco man\n");
        sem_post(&hold_up);
    }
}


int main() {
    sem_init(&smoker_line, 0, 1);
    sem_init(&tobacco_man, 0, 0);
    sem_init(&hold_up, 0, 0);
 
    if(pthread_create(&tobacconist, NULL, tobacconist_thread, NULL)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    if(pthread_create(&smokers[0], NULL, smoker_thread, &PAPER)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    if(pthread_create(&smokers[0], NULL, smoker_thread, &TOBACCO)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    if(pthread_create(&smokers[0], NULL, smoker_thread, &MATCHES)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    usleep(10000);
}
