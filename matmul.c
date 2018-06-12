/*
 * Matrix multiplication with pthreads
 *
 * pthread usage: create, exit, join
 *
 * Lessons: 
 *      all threads must rejoin main before proc can terminate, else erratic behavior
 *      create takes address to pthread, join takes pthread datastructure
 *      unlike in this ex, maybe structure threads to last whole program life to reduce spawning cycles
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// static data
enum {
    N = 10,
    M = 25,
    P = 20,
    MAX_THREADS = 8,
};

// static data structures 
int matrix1[M][N];
int matrix2[N][P];
long int result[M][P];

mat_lines_t thread_args[MAX_THREADS];
pthread_t threads[MAX_THREADS];

// user defined types
typedef struct mat_lines mat_lines_t;
struct mat_lines {
    int row;
    int col;
};


// thread routine
void *threader(void *args) {
    int i;
    long int res;
    mat_lines_t loc = *(mat_lines_t *)args;
    
    // compute the result of one value in results matrix
    res = 0;
    for(i = 0; i < N; i++) {
        res += matrix1[loc.row][i] * matrix2[i][loc.col];
    }
    
    // set the result into the matrix
    result[loc.row][loc.col] = res;
    
    pthread_exit(0);
}


int main() {
    int i, j, c, modc;

    // seed values in matrix one
    for(i = 0; i < M; i++) {
        for(j = 0; j < N; j++) {
            matrix1[i][j] = (i+1) * (j+1);
        }
    }
    
    // seed values in matrix two
    for(i = 0; i < N; i++) {
        for(j = 0; j < P; j++) {
            matrix2[i][j] = (i+1) * (j+1);
        }
    }
    
    // start computations
    c = 0;
    for(i = 0; i < M; i++) {
        for(j = 0; j < P; j++) {
            modc = c % MAX_THREADS;
            
            // if all threads up, wait for next to close
            if(c >= MAX_THREADS) {
                fprintf(stdout, "\twaiting for thread\n");
                pthread_join(threads[modc], NULL);
            }
            
            // set values into argument
            thread_args[modc].row = i;
            thread_args[modc].col = j;
            
            // create new thread
            fprintf(stdout, "spawning new thread\n");
            if(pthread_create(&threads[modc], NULL, threader, &thread_args[modc])) {
                fprintf(stderr, "Thread creation failed, exiting\n");
            }
            
            c++;
        }
    }
    
    // wait for remaining threads to finish
    for(i = 0; i < MAX_THREADS; i++) {
        fprintf(stdout, "\twaiting for thread\n");
        pthread_join(threads[i], NULL);
    }
    
    fprintf(stdout, "\n");
    
    // print results
    for(i = 0; i < M; i++) {
        for(j = 0; j < P; j++) {
            fprintf(stdout, "%7ld\t", result[i][j]);
        }
        
        fprintf(stdout, "\n");
    }
    
    return 0;
}

