#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct node {
    long int id;
    size_t connections;
    int *adjacent;
    double value;
}* Node;

// Initializes a node with the given number of connections
Node create_node(size_t connections, long int id);
// Sets the value of the node to 15% (of what it was) and stores the value that each of its
// adjacent nodes will receive in the thread_values array
void page_rank(Node node, double* thread_values);
// Adds the given share to the value of the node based on the values in the thread_values array
void distribute(Node node, double** thread_values, int num_threads);
// Prints the given node
void print_node(Node node);
// Prints the value of the given node
void print_value(Node node);
// Frees the memory allocated for the given node
void free_node(Node node);

#endif // _NODE_H_