#include <stdio.h>
#include <pthread.h>
#include "node.h"
#include "random.h"

#define BUFFER_SIZE 1024
 
typedef struct args {
    Node* nodes;
    int* node_queue;
    double** thread_values;

    int id;
    int num_nodes;
    int iters;
    int num_threads;
    int thread_index;
}* ThreadArg;

static pthread_barrier_t barrier;

/* --------------------------- Function Declarations --------------------------- */

// Creates a ThreadArg struct
ThreadArg create_thread_arg(Node* nodes, int* node_queue, double** thread_values, int id,
                            int num_nodes, int iters, int num_threads, int thread_index);
// Parses the input file and returns an alloated array of nodes
Node* parse_file(FILE* inp);
// Writes the results to the output file (which is in CSV format)
void write_results(Node* nodes, int num_nodes, int num_threads, int iters);
// The main function of the thread
void* thread_main(void* arg);


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

    /* ------------------------ Node Queue Init ------------------------ */

    // Creates a queue of node indexes to be processed
    // The queu is implemented with an array and an index to the next node to be processed
    int* node_queue = random_array(num_nodes);

    /* --------------------------- Thread Init --------------------------- */

    // Number of threads to use
    int num_threads = atoi(argv[2]);

    // Creates a table of threads and initializes them
    pthread_t threads[num_threads];

    // Creates an array of size num_nodes for each thread to keep the values to be added to each node
    double** thread_values = malloc(sizeof(double*) * num_threads);
    
    for(int i = 0; i < num_threads; i++)
        thread_values[i] = calloc(num_nodes, sizeof(double));


    // Splits the node_queue array between the threads, the last thread gets the remaining nodes
    int nodes_per_thread = num_nodes / num_threads;
    int nodes_left = num_nodes % num_threads;

    // Initializes the barrier
    pthread_barrier_init(&barrier, NULL, num_threads);


    for(int i = 0; i < num_threads; i++) {
        // Create the arguments for the thread
        ThreadArg arg = create_thread_arg(nodes, node_queue, thread_values, i, nodes_per_thread, 
                                          iterations, num_threads, i * nodes_per_thread);

        // Checks if the thread is the last one
        if(i == num_threads - 1) arg->num_nodes += nodes_left;

        pthread_create(&threads[i], NULL, thread_main, arg);
    }

    /* --------------------------- Clean-up --------------------------- */


    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Saves the results to the output file
    write_results(nodes, num_nodes, num_threads, iterations);

    // Frees the memory allocated for the nodes
    for(int i = 0; i < num_threads; i++) {
        free_node(nodes[i]);
    }

    return 0;
}


/* --------------------------- Function Definitions --------------------------- */

ThreadArg create_thread_arg(Node* nodes, int* node_queue, double** thread_values, int id,
                            int num_nodes, int iters, int num_threads, int thread_index){
    ThreadArg arg = malloc(sizeof(struct args));
    arg->nodes = nodes;
    arg->node_queue = node_queue;
    arg->thread_values = thread_values;
    arg->num_nodes = num_nodes;
    arg->iters = iters;
    arg->num_threads = num_threads;
    arg->thread_index = thread_index;
    arg->id = id;
    return arg;
}

void* thread_main(void* arg){
    ThreadArg t_arg = (ThreadArg) arg;
    Node* nodes = t_arg->nodes;
    int* node_queue = t_arg->node_queue;
    double** thread_values = t_arg->thread_values;
    int thread_index = t_arg->thread_index;
    int num_threads = t_arg->num_threads;
    int iters = t_arg->iters;
    int thread_id = t_arg->id;
    int end = thread_index + t_arg->num_nodes;


    for(int i = 0; i < iters; i++){
        // -------------------------- PageRank --------------------------

        pthread_barrier_wait(&barrier);

        // For each node that this thread is responsible for,
        // it calls the page_rank function on it
        for(int j = thread_index; j < end; j++){
                page_rank(nodes[node_queue[j]], thread_values[thread_id]);
        }


        // -------------------------- Update Values --------------------------

        pthread_barrier_wait(&barrier);

        for(int j = thread_index; j < end; j++)
            distribute(nodes[node_queue[j]], thread_values, num_threads);

    }


    free(arg);

    return NULL;
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

// Writes the results to the output file (which is in CSV format)
void write_results(Node* nodes, int num_nodes, int num_threads, int iters){
    FILE *fptr;

    // Construct the filename, which is in the format:
    // page_rank_<num_threads>_<iters>.csv

    char filename[100];
    sprintf(filename, "page_rank_%d_%d.csv", num_threads, iters);

    // Checking if the file already exists, else we create a new one
    if(!(fptr = fopen(filename, "rb+"))) fptr = fopen(filename, "wb");

    // Write the results to the file, the format is:
    // node, pagerank
    // node1, pagerank1
    // node2, pagerank2
    // ...

    fprintf(fptr, "node, pagerank\n");
    for(int i = 0; i < num_nodes; i++)
        fprintf(fptr, "%ld, %f\n", nodes[i]->id, nodes[i]->value);
    
    fclose(fptr);
}