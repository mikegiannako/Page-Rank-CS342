#ifndef _RANDOM_H_
#define _RANDOM_H_

// Returns a random number between [min, max]
int randomc(int min, int max);

// Returns a randomized arrary with number from 1 to n
// The array has to be freed by the caller of the function
int* random_array(int n);

#endif // _RANDOM_H_