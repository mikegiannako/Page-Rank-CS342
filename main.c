#include <stdio.h>
#include <pthread.h>
#include "node.h"


// Usage: ./main <input_file> <number_of_threads> (<number_of_iterations>)
// Default value of iterations is 50
int main(int argc, char* argv[]){
    if(argc < 3) {
        printf("Usage: ./main <input_file> <number_of_threads> (<number_of_iterations>)\n");
        return 0;
    }

    int iterations = 50;

    if(argc == 4) {
        iterations = atoi(argv[3]);
    }

    int num_threads = atoi(argv[2]);

    // Creates a table of threads and initializes them
    pthread_t threads[num_threads];
    for(int i = 0; i < num_threads; i++) {
        //pthread_create(&threads[i], NULL, thread_main, NULL);
    }

    return 0;
}