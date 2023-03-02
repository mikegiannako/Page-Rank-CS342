#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct node {
    size_t connections;
    int *adjacent;
    float value;
    pthread_mutex_t lock;
}* Node;

// Initializes a node with the given number of connections
Node create_node(size_t connections);
// Applies the page rank algorithm to the given node.
// The node holds 15% of its value and distributes the remaining 85% to its
// adjacent nodes.
void page_rank(Node node);
// Distributes the given share to the particular node. This is a
// thread-safe operation. The node's lock is acquired before the
// operation and released after the operation.
void distribute(Node node, float share);
// Frees the memory allocated for the given node
void free_node(Node node);

#endif // _NODE_H_