#include <stdio.h>
#include "node.h"

// Initializes a graph node with the given number of connections
GraphNode create_node(long int id){
    GraphNode node = malloc(sizeof(struct graph_node));
    node->id = id;
    node->connections = 0;
    node->value = 1.0;
    node->incoming = NULL;
    return node;
}

// initializes a list node with the given id
ListNode create_list_node(long int id){
    ListNode node = malloc(sizeof(struct list_node));
    node->id = id;
    node->next = NULL;
    return node;
}

// Adds the given node to the list of incoming nodes
void add_incoming(GraphNode node, long int id){
    ListNode new_node = create_list_node(id);
    new_node->next = node->incoming;
    node->incoming = new_node;
}

// Calculates the sum of the values of the incoming nodes
double sum_incoming(GraphNode node, double* share_values){
    double sum = 0;
    ListNode current = node->incoming;
    while(current != NULL){
        sum += share_values[current->id];
        current = current->next;
    }
    return sum;
}

// Free the memory allocated for the given list
void free_list(ListNode list){
    if(list == NULL){
        return;
    }
    free_list(list->next);
    free(list);
}

// Free the memory allocated for the given graph node
void free_graph_node(GraphNode graph){
    if(graph == NULL){
        return;
    }
    free_list(graph->incoming);
    free(graph);
}

// Prints the value of the given graph node
void print_value(GraphNode node){
    printf("Node %ld: %f\n", node->id, node->value);
}