#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct list_node{
    long int id;
    struct list_node *next;
}* ListNode;

typedef struct graph_node {
    long int id;
    double value;
    size_t connections;
    ListNode incoming;
}* GraphNode;

// Initializes a node with the given number of connections
GraphNode create_node(long int id);
// initializes a list node with the given id
ListNode create_list_node(long int id);
// Adds the given node to the list of incoming nodes
void add_incoming(GraphNode node, long int id);
// Calculates the sum of the values of the incoming nodes
double sum_incoming(GraphNode node, double* share_values);
// Free the memory allocated for the given list
void free_list(ListNode list);
// Free the memory allocated for the given graph node
void free_graph_node(GraphNode graph);
// Prints the value of the given graph node
void print_value(GraphNode node);

#endif // _NODE_H_