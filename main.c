#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "node.h"
#include "random.h"

#define BUFFER_SIZE 1024
#define DAMPING_FACTOR 0.85
#define ITERATIONS 500
 
typedef struct args {
    GraphNode* nodes;
    int* node_queue;
    double* share_values;

    int num_nodes;
    int iters;
    int thread_index;
}* ThreadArg;

static pthread_barrier_t barrier;

/* --------------------------- Function Declarations --------------------------- */

// Creates a ThreadArg struct
ThreadArg create_thread_arg(GraphNode* nodes, int* node_queue, double* share_values, int num_nodes, int iters, int thread_index);
// Parses the input file and returns an alloated array of nodes
GraphNode* parse_file(FILE* inp);
// Writes the results to the output file (which is in CSV format)
void write_results(GraphNode* nodes, int num_nodes, int num_threads, int iters, char* filename);
// The main function of the thread
void* thread_main(void* arg);


// Usage: ./page_rank <input_file> <number_of_threads> (-i <number_of_iterations>) (-m <mode>)
// Default value of iterations is 50
int main(int argc, char* argv[]){

    clock_t begin = clock();

    /* --------------------------- Argument Checking --------------------------- */
    if(argc < 3) {
        printf("Usage: ./main <input_file> <number_of_threads> (-i <number_of_iterations>) (-m <mode>)\n");
        return 0;
    }

    // Default value of iterations is 50
    int iterations = ITERATIONS;

    // Default value of mode is 0 mode is used so the algorithm doesn't write to a file when
    // measuring speeds of different numbers of threads
    int mode = 0;

    // If the user specified the number of iterations, save it to a variable
    if(argc > 3) {
        if(strcmp(argv[3], "-i") == 0) {
            iterations = atoi(argv[4]);
        }
        else if(strcmp(argv[3], "-m") == 0) {
            mode = atoi(argv[4]);
        }
    }

    // If the user specified the mode, save it to a variable
    if(argc > 5) {
        if(strcmp(argv[5], "-i") == 0) {
            iterations = atoi(argv[6]);
        }
        else if(strcmp(argv[5], "-m") == 0) {
            mode = atoi(argv[6]);
        }
    }

    /* --------------------------- Input Parsing --------------------------- */

    FILE* input_file = fopen(argv[1], "r");
    if(input_file == NULL) {
        printf("Error: Could not open input file\n");
        return 0;
    }

    // Parses the input file and returns an array of nodes
    GraphNode* nodes = parse_file(input_file);

    // Find the number of nodes and save it to a variable
    int num_nodes = 0;
    while(nodes[num_nodes]->id != -1) num_nodes++;

    double* pr_values = calloc(num_nodes, sizeof(double));

    /* ------------------------ GraphNode Queue Init ------------------------ */

    // Creates a queue of node indexes to be processed
    // The queu is implemented with an array and an index to the next node to be processed
    int* node_queue = random_array(num_nodes);

    /* --------------------------- Thread Init --------------------------- */

    // Number of threads to use
    int num_threads = atoi(argv[2]);

    // Creates a table of threads and initializes them
    pthread_t threads[num_threads];

    // Splits the node_queue array between the threads, the last thread gets the remaining nodes
    int nodes_per_thread = num_nodes / num_threads;
    int nodes_left = num_nodes % num_threads;

    // Initializes the barrier
    pthread_barrier_init(&barrier, NULL, num_threads);


    for(int i = 0; i < num_threads; i++) {
        // Create the arguments for the thread
        ThreadArg arg = create_thread_arg(nodes, node_queue, pr_values, nodes_per_thread, 
                                          iterations, i * nodes_per_thread);

        // Checks if the thread is the last one
        if(i == num_threads - 1) arg->num_nodes += nodes_left;

        pthread_create(&threads[i], NULL, thread_main, arg);
    }

    /* --------------------------- Clean-up --------------------------- */


    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Saves the results to the output file
    if(!mode) write_results(nodes, num_nodes, num_threads, iterations, argv[1]);

    // Frees the memory allocated for the nodes
    for(int i = 0; i < num_threads; i++)
        free_graph_node(nodes[i]);

    free(nodes);
    free(pr_values);
    free(node_queue);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f\n", time_spent);
    return 0;
}


/* --------------------------- Function Definitions --------------------------- */

ThreadArg create_thread_arg(GraphNode* nodes, int* node_queue, double* share_values, int num_nodes, int iters, int thread_index){
    ThreadArg arg = malloc(sizeof(struct args));
    arg->nodes = nodes;
    arg->node_queue = node_queue;
    arg->share_values = share_values;

    arg->num_nodes = num_nodes;
    arg->iters = iters;
    arg->thread_index = thread_index;
    return arg;
}

void* thread_main(void* arg){
    ThreadArg t_arg = (ThreadArg) arg;
    GraphNode* nodes = t_arg->nodes;
    int* node_queue = t_arg->node_queue;
    double* share_values = t_arg->share_values;

    int thread_index = t_arg->thread_index;
    int iters = t_arg->iters;
    int end = thread_index + t_arg->num_nodes;


    for(int i = 0; i < iters; i++){
        // -------------------------- PageRank --------------------------

        pthread_barrier_wait(&barrier);

        // For each node that this thread is responsible for, it calculates 
        // the PageRank share and saves it to the share_values array
        for(int j = thread_index; j < end; j++)
            share_values[node_queue[j]] = DAMPING_FACTOR * (nodes[node_queue[j]]->value / nodes[node_queue[j]]->connections);


        // -------------------------- Update Values --------------------------

        pthread_barrier_wait(&barrier);

        // For each node that this thread is responsible for, it updates the PageRank value
        // by adding the share values of the nodes that point to it after setting its share value to 1 - DAMPING_FACTOR
        for(int j = thread_index; j < end; j++){
            nodes[node_queue[j]]->value = (1 - DAMPING_FACTOR);
            nodes[node_queue[j]]->value += sum_incoming(nodes[node_queue[j]], share_values);
        }

    }

    free(arg);

    return NULL;
}

GraphNode* parse_file(FILE* inp){
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
    GraphNode* nodes = malloc(sizeof(struct graph_node) * (max_node_id + 2));

    // Initialize each node
    for(int i = 0; i < max_node_id + 1; i++) nodes[i] = create_node(i);
    // Initialize the dummy node
    nodes[max_node_id + 1] = create_node(-1);

    // Now we need to parse the file again to find the number of connections 
    // for each node and for each node find which ones are pointing to it
    // We know that <source> field are in ascending order
    // so we can use that to determine the number of connections
    // for each node
    rewind(inp);

    while(fgets(buffer, BUFFER_SIZE, inp) != NULL) {
        // Skip comments
        if(buffer[0] == '#') {
            continue;
        }
        
        // Parse the line
        sscanf(buffer, "%ld %ld", &source, &destination);

        // Update the number of connections for the source node
        nodes[source]->connections++;

        // For each line, we want to add the source node to the destination node's
        // incoming list
        add_incoming(nodes[destination], source);
    }

    return nodes;
}

// Writes the results to the output file (which is in CSV format)
void write_results(GraphNode* nodes, int num_nodes, int num_threads, int iters, char* filename){
    FILE *fptr;

    // Construct the filename, which is in the format:
    // <filename>_<num_threads>_<iters>.csv

    char temp[BUFFER_SIZE];

    // We need to remove the test/ prefix and the .txt suffix from the filename
    char* file = strtok(filename, "/");
    while(file != NULL){
        strcpy(temp, file);
        file = strtok(NULL, "/");
    }

    // Remove the .txt suffix
    temp[strlen(temp) - 4] = '\0';


    char finalname[BUFFER_SIZE * 2];
    sprintf(finalname, "%s_%d_%d.csv", temp, num_threads, iters);

    // Checking if the file already exists, else we create a new one
    if(!(fptr = fopen(finalname, "rb+"))) fptr = fopen(finalname, "wb");

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