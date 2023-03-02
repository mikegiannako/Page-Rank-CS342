#include <stdio.h>
#include "node.h"

// Initializes a node with the given number of connections
Node create_node(size_t connections){
    Node node = malloc(sizeof(struct node));
    node->connections = connections;
    node->adjacent = malloc(sizeof(int) * connections);
    node->value = 1.0;
    node->lock = PTHREAD_MUTEX_INITIALIZER;
    return node;
}

// Applies the page rank algorithm to the given node.
// The node holds 15% of its value and distributes the remaining 85% to its
// adjacent nodes.
void page_rank(Node node){
    float value = node->value * 0.15;
    float share = (node->value - value) / node->connections;

    // Distribute the share to the adjacent nodes
    // TODO: Implement this

    node->value = value;
}

// Distributes the given share to the particular node. This is a
// thread-safe operation. The node's lock is acquired before the
// operation and released after the operation.
void distribute(Node node, float share){
    pthread_mutex_lock(&node->lock);
    node->value += share;
    pthread_mutex_unlock(&node->lock);
}

void free_node(Node node){
    free(node->adjacent);
    free(node);
}
