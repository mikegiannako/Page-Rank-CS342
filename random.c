#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initializes the random number generator only once
void init_random(){
    static int initialized = 0;
    if(!initialized){
        srand(time(NULL));
        initialized = 1;
    }
    return;
}

// Returns a random number between [min, max]
int randomc(int min, int max){
    init_random();
    return rand() % (max - min + 1) + min;
}

// Returns a randomized arrary with number from 0 to n - 1
int* random_array(int n){
    init_random();
    // Create an array with numbers from 0 to n - 1
    int* array = malloc(n * sizeof(int));
    for(int i = 0; i < n; i++){
        array[i] = i;
    }

    // Randomize the array
    for(int i = 0; i < n; i++){
        int j = randomc(0, n - 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }

    return array;
}