/*
 * Reader / Writer exploration with pthreads and cond_vars
 *
 * pthread usage: cond_t, mutex
 *
 * Lessons:
 *      unexpected behavior may be caused by incorrect guard conditions
 *      condition variables allow programmer to create virtual atomic-code 
 *          (atomic within the relavance of the operations to other threads)
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// static data
enum {
    MAX_READERS = 8,
    MAX_WRITERS = 2,
    BUFF_SIZE = 34,
};

// static data structures
pthread_t writers[MAX_WRITERS];
pthread_t readers[MAX_READERS];
pthread_cond_t access_wrt, access_rdr;
pthread_mutex_t m;

int wrts, act_wrts, rdrs;
int rdr_vals[MAX_READERS];
char wrt_vals[MAX_WRITERS];
char buffer[] = "csc360 operating systems in depth";


// writer thread routine
void *write_thread(void *arg) {
    int pos;
    char val = *(char *)arg;
    
    for(;;) {        
        pthread_mutex_lock(&m);
        
        wrts++;
        while(act_wrts == 0 && rdrs != 0) pthread_cond_wait(&access_wrt, &m);
        act_wrts++;
        
        pthread_mutex_unlock(&m);
        
        pos = rand() % BUFF_SIZE;
        buffer[pos] = val;
        
        pthread_mutex_lock(&m);
        
        act_wrts--;
        if(--wrts) pthread_cond_signal(&access_wrt);
        else pthread_cond_broadcast(&access_rdr);
        
        pthread_mutex_unlock(&m);
        
        usleep(100);
    }
}

// reader thread routine
void *read_thread(void *arg) {
    int val = *(int *)arg;

    for(;;) {
        pthread_mutex_lock(&m);
        
        while(wrts) pthread_cond_wait(&access_rdr, &m);
        rdrs++;
        
        pthread_mutex_unlock(&m);
        
        fprintf(stdout, "Reader %d: %s\n", val, buffer);
        
        pthread_mutex_lock(&m);
        
        if(--rdrs) pthread_cond_signal(&access_wrt);
        
        pthread_mutex_unlock(&m);
        
        usleep(100);
    }
}

int main() {
    int i;
    
    // allocate memory for mutex and cond vars
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&access_wrt, NULL);
    pthread_cond_init(&access_rdr, NULL);
    
    // seed global data structures
    wrts = act_wrts = rdrs = 0;
    for(i = 0; i < MAX_READERS; i++) rdr_vals[i] = i;
    wrt_vals[0] = '#';
    wrt_vals[1] = '@';
    
    // create and dettach reader threads
    for(i = 0; i < MAX_READERS; i++) {
        if(pthread_create(&readers[i], NULL, read_thread, &rdr_vals[i])) {
            fprintf(stderr, "Error occured creating thread, exiting\n");
            exit(1);
        }
        pthread_detach(readers[i]);
    }
    
    // create and dettach writer threads
    for(i = 0; i < MAX_WRITERS; i++) {
        if(pthread_create(&writers[i], NULL, write_thread, &wrt_vals[i])) {
            fprintf(stderr, "Error occured creating thread, exiting\n");
            exit(1);
        }
        pthread_detach(writers[i]);
    }
    
    // delay termination of parent : allow threads to work
    usleep(1000);
    // secure and destroy the mutex
    pthread_mutex_lock(&m);
    pthread_mutex_destroy(&m);
    // destroy condition variables
    pthread_cond_destroy(&access_wrt);
    pthread_cond_destroy(&access_rdr);
    return 0;
}

