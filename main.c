#include <stdio.h>
#include <pthread.h>
#include "node.h"
#include "random.h"

#define BUFFER_SIZE 1024

// Parses the input file and returns an alloated array of nodes
Node* parse_file(FILE* inp);

// Usage: ./main <input_file> <number_of_threads> (<number_of_iterations>)
// Default value of iterations is 50
int main(int argc, char* argv[]){
    /* --------------------------- Argument Checking --------------------------- */
    if(argc < 3) {
        printf("Usage: ./main <input_file> <number_of_threads> (<number_of_iterations>)\n");
        return 0;
    }

    // Default value of iterations is 50
    int iterations = 50;

    // If the user specified the number of iterations, we use that value
    if(argc == 4) {
        iterations = atoi(argv[3]);
    }

    /* --------------------------- Input Parsing --------------------------- */

    FILE* input_file = fopen(argv[1], "r");
    if(input_file == NULL) {
        printf("Error: Could not open input file\n");
        return 0;
    }

    // Parses the input file and returns an array of nodes
    Node* nodes = parse_file(input_file);

    // Find the number of nodes and save it to a variable
    int num_nodes = 0;
    while(nodes[num_nodes]->id != -1) num_nodes++;

    /* --------------------------- Thread Init --------------------------- */

    // Number of threads to use
    int num_threads = atoi(argv[2]);

    // Creates a table of threads and initializes them
    pthread_t threads[num_threads];

    for(int i = 0; i < num_threads; i++) {
        //pthread_create(&threads[i], NULL, thread_main, NULL);
        break;
    }

    /* ------------------------ Node Queue Init ------------------------ */

    // Creates a queue of node indexes to be processed
    // The queu is implemented with an array and an index to the next node to be processed
    int* node_queue = random_array(num_nodes);

    /* --------------------------- Main Loop --------------------------- */

    // Prints the node_queue array
    printf("Node Queue: ");
    for(int i = 0; i < num_nodes; i++) {
        printf("%d ", node_queue[i]);
    }
    printf("\n");

    // Main Loop
    for(int i = 0; i < iterations; i++) {
        // Prints the nodes
        printf("Iteration %d
    }

    /* --------------------------- Clean-up --------------------------- */

    for(int i = 0; i < num_threads; i++) {
        //pthread_join(threads[i], NULL);
        break;
    }

    // Frees the memory allocated for the nodes
    for(int i = 0; i < num_threads; i++) {
        free_node(nodes[i]);
    }

    return 0;
}

Node* parse_file(FILE* inp){
    long int source, destination;
    char buffer[BUFFER_SIZE];

    // First we need to find the maximum node id
    // to determine the size of the nodes array
    long int max_node_id = 0;
    while(fgets(buffer, BUFFER_SIZE, inp) != NULL) {
        // Skip comments
        if(buffer[0] == '#') {
            continue;
        }
        
        // Parse the line
        sscanf(buffer, "%ld %ld", &source, &destination);

        // Update the maximum node id
        if(source > max_node_id) {
            max_node_id = source;
        }
        if(destination > max_node_id) {
            max_node_id = destination;
        }
    }

    // Allocate the nodes array (+ 2 because the ids start from 0 and
    // we also need a dummy node at the end)
    Node* nodes = malloc(sizeof(Node) * (max_node_id + 2));

    // Now we need to parse the file again to initialize the nodes array
    // and find the number of connections for each node
    // We know that <source> field are in ascending order
    // so we can use that to determine the number of connections
    // for each node
    rewind(inp);

    long int current_node_id = 0;
    long int current_node_connections = 0;
    while(fgets(buffer, BUFFER_SIZE, inp) != NULL) {
        // Skip comments
        if(buffer[0] == '#') {
            continue;
        }
        
        // Parse the line
        sscanf(buffer, "%ld %ld", &source, &destination);

        // If we have reached a new node, we need to initialize the previous node
        if(source != current_node_id) {
            // Initialize the previous node
            nodes[current_node_id] = create_node(current_node_connections, current_node_id);

            // Check if the new node is more than 1 away from the previous node
            // If so, we need to initialize the nodes in between with 0 connections
            if(source - current_node_id > 1) {
                for(int i = current_node_id + 1; i < source; i++) {
                    nodes[i] = create_node(0, i);
                }
            }

            // Update the current node id
            current_node_id = source;

            // Reset the number of connections
            current_node_connections = 0;
        }

        // Update the number of connections
        current_node_connections++;
    }

    // Initialize the last node
    nodes[current_node_id] = create_node(current_node_connections, current_node_id);

    // Check if the last node id is less than the maximum node id
    // If so, we need to initialize the nodes in between with 0 connections
    if(current_node_id < max_node_id) {
        for(int i = current_node_id + 1; i <= max_node_id; i++) {
            nodes[i] = create_node(0, i);
        }
    }

    // Initialize the dummy node
    nodes[max_node_id + 1] = create_node(0, -1);

    // Now we need to parse the file one last time to fill the adjacent array
    // of each node
    rewind(inp);

    current_node_id = 0;
    current_node_connections = 0;
    while(fgets(buffer, BUFFER_SIZE, inp) != NULL) {
        // Skip comments
        if(buffer[0] == '#') {
            continue;
        }
        
        // Parse the line
        sscanf(buffer, "%ld %ld", &source, &destination);

        // If we have reached a new node, we need to initialize the previous node
        if(source != current_node_id) {
            // Update the current node id
            current_node_id = source;

            // Reset the number of connections
            current_node_connections = 0;
        }

        nodes[current_node_id]->adjacent[current_node_connections] = destination;

        // Update the number of connections
        current_node_connections++;
    }

    return nodes;
}