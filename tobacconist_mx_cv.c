#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


pthread_t tobacconist;
pthread_t smokers[3];
pthread_cond_t cv_smoker, cv_tobacconist;
pthread_mutex_t m;

int PAPER   = 1;
int TOBACCO = 2;
int MATCHES = 4;
int TABLE_SET = 0;


void *smoker_thread(void *args) {
    int item = *(int *) args;
    
    for(;;) {
        pthread_mutex_lock(&m);
        
        while(TABLE_SET == 0 || TABLE_SET + item != 7) pthread_cond_wait(&cv_smoker, &m);
        TABLE_SET = 0;
        pthread_cond_signal(&cv_tobacconist);
        pthread_mutex_unlock(&m);
        
        printf("Smoker %d\n", item);
        usleep(10);
    }
} 


void *tobacconist_thread(void *args) {
    int sets[3] = {6, 5, 3};
    int next = 2;
    
    for(;;) {
        pthread_mutex_lock(&m);
        
        while(TABLE_SET != 0) pthread_cond_wait(&cv_tobacconist, &m);
        TABLE_SET = sets[next = (next + 1) % 3];
        pthread_cond_broadcast(&cv_smoker);
        
        pthread_mutex_unlock(&m);
        
        printf("Tobacconist\n");
    }
}


int main() {
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cv_tobacconist, NULL);
    pthread_cond_init(&cv_smoker, NULL);
 
    if(pthread_create(&tobacconist, NULL, tobacconist_thread, NULL)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    if(pthread_create(&smokers[0], NULL, smoker_thread, &PAPER)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    if(pthread_create(&smokers[1], NULL, smoker_thread, &TOBACCO)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    if(pthread_create(&smokers[2], NULL, smoker_thread, &MATCHES)) {
        fprintf(stderr, "Error occured creating thread, exiting\n");
        exit(1);
    }
    
    usleep(10000);
}
