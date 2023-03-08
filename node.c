#include <stdio.h>
#include "node.h"

// Initializes a node with the given number of connections
Node create_node(size_t connections, long int id){
    Node node = malloc(sizeof(struct node));
    node->id = id;
    node->connections = connections;
    node->adjacent = malloc(sizeof(int) * connections);
    node->value = 1.0;
    return node;
}

// Sets the value of the node to 15% (of what it was) and stores the value that each of its
// adjacent nodes will receive in the thread_values array at the given index
void page_rank(Node node, double* thread_values){
    double new = node->value * 0.15;
    double share = (node->value - new) / node->connections;
    node->value = new;

    for (int i = 0; i < node->connections; i++) {
        thread_values[node->adjacent[i]] += share;
    }
}

// Adds the given share to the value of the node based on the values in the thread_values array
void distribute(Node node, double** thread_values, int num_threads){
    for (int i = 0; i < num_threads; i++) {
        node->value += thread_values[i][node->id];
        thread_values[i][node->id] = 0;
    }
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

void print_value(Node node){
    printf("Node %ld: %f\n", node->id, node->value);
}

// Frees the memory allocated for the given node
void free_node(Node node){
    free(node->adjacent);
    free(node);
}
