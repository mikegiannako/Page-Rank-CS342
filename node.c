#include <stdio.h>
#include "node.h"

// Initializes a node with the given number of connections
Node create_node(size_t connections, long int id){
    Node node = malloc(sizeof(struct node));
    node->id = id;
    node->connections = connections;
    node->adjacent = malloc(sizeof(int) * connections);
    node->value = 1.0;
    node->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    return node;
}

// Applies the page rank algorithm to the given node.
// The node holds 15% of its value and distributes the remaining 85% to its
// adjacent nodes.
void page_rank(Node node, Node* node_list){

    pthread_mutex_lock(&node->lock);
    // Hold 15% of the value
    float value = node->value * 0.15;
    node->value = value;

    // Distribute 85% of the value to the adjacent nodes
    float share = (node->value - value) / node->connections;
    pthread_mutex_lock(&node->lock);

    for (int i = 0; i < node->connections; i++) {
        distribute(node_list[node->adjacent[i]], share);
    }
}

// Distributes the given share to the particular node. This is a
// thread-safe operation. The node's lock is acquired before the
// operation and released after the operation.
void distribute(Node node, float share){
    pthread_mutex_lock(&node->lock);
    node->value += share;
    pthread_mutex_unlock(&node->lock);
}

// Prints the given node
void print_node(Node node){
    printf("Node %ld: %f ", node->id, node->value);
    
    // Prints the adjacent nodes
    printf("[");
    for (int i = 0; i < node->connections; i++) {
        printf("%d", node->adjacent[i]);
        if(i != node->connections - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Frees the memory allocated for the given node
void free_node(Node node){
    free(node->adjacent);
    free(node);
}
