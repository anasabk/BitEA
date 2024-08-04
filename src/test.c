#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "BitEA.h"
#include "stdgraph.h"


struct test_return {
    block_t *solution;
    int color_count;
    int fitness;
    int uncolored;
    float best_time;
    char summary[512];
};

struct test_param {
    int size;
    int target_color;
    int iteration_count;
    int test_count;
    int population_size;
    char graph_filename[128];
    char weight_filename[128];
    char result_filename[128];
    FILE* summary_file;
    struct test_return result;
};


void test_graph(void *param, int *best_result) {
    int size = ((struct test_param*)param)->size;
    int iteration_count = ((struct test_param*)param)->iteration_count;
    int target_color = ((struct test_param*)param)->target_color;
    int population_size = ((struct test_param*)param)->population_size;
    char *graph_filename = ((struct test_param*)param)->graph_filename;
    char *weight_filename = ((struct test_param*)param)->weight_filename;
    char *result_filename = ((struct test_param*)param)->result_filename;

    block_t *edges = malloc(sizeof(block_t) * (size_t)size * TOTAL_BLOCK_NUM((size_t)size));
    if(!read_graph(graph_filename, size, edges, 0)) {
        printf("Could not initialize graph from %s, exiting ...\n", graph_filename);
        return;
    }

    int edge_count[size];
    count_edges(size, edges, edge_count);

    int weights[size];
    if(strncmp(weight_filename, "null", 4) != 0) {
        if(!read_weights(weight_filename, size, weights)) {
            printf("Could not initialize graph weights from %s, exiting ...\n", weight_filename);
            return;
        }

    } else {
        memcpy(weights, edge_count, size*sizeof(int));
    }

    
    int max_edge_count = 0;
    for(int i = 0; i < size; i++) 
        if(max_edge_count < edge_count[i])
            max_edge_count = edge_count[i];

    float temp_time;
    int temp_fitness, temp_color_count, temp_uncolored;
    block_t *temp_colors = calloc(max_edge_count, TOTAL_BLOCK_NUM(size)*sizeof(block_t));

    struct timeval t1, t2;
    float total_execution_time = 0;

    gettimeofday(&t1, NULL);
    temp_color_count = BitEA (
        size,
        edges,
        weights,
        population_size,
        target_color,
        iteration_count,
        temp_colors,
        &temp_fitness,
        &temp_time,
        &temp_uncolored
    );
    gettimeofday(&t2, NULL);
    total_execution_time += (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;

    if(temp_fitness == 0)
        is_valid(size, edges, temp_color_count, temp_colors);

    printf(
        "|%s|%3d|%10.6lf|%3d|%5d|%3d|%10.6lf|\n",
        graph_filename, 
        target_color,
        temp_time, 
        temp_color_count,
        temp_fitness,
        temp_uncolored,
        total_execution_time
    );

    fprintf(((struct test_param*)param)->summary_file,
        "|%s|%3d|%10.6lf|%3d|%5d|%3d|%10.6lf|\n",
        graph_filename, 
        target_color,
        temp_time, 
        temp_color_count,
        temp_fitness,
        temp_uncolored,
        total_execution_time
    );

    if(*best_result > temp_fitness) {
        *best_result = temp_fitness;

        char buffer[512];
        sprintf(buffer,
            "|  graph name   | target color | k time | k | cost | uncolored | total time |\n|%s|%3d|%10.6lf|%3d|%5d|%3d|%10.6lf|\n",
            graph_filename, 
            target_color,
            temp_time, 
            temp_color_count,
            temp_fitness,
            temp_uncolored,
            total_execution_time
        );

        print_colors(result_filename, buffer, target_color, size, temp_colors);
    }

    free(edges);
    free(temp_colors);
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Too few arguments.\n");
        return 0;
    }

    FILE *test_list_file = fopen(argv[1], "r");
    if(test_list_file == NULL) {
        printf("test file not found\n");
        return 0;
    }

    FILE *summary_file = fopen(argv[2], "a+");
    if(summary_file == NULL) {
        printf("summary file could not be opened\n");
        return 0;
    }

    srand(time(NULL));


    printf("|  graph name   | target color | k time | k | cost | uncolored | total time |\n");

    char buffer[512];
    struct test_param param;
    param.summary_file = summary_file;
    int test_count;
    while(fgets(buffer, 256, test_list_file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        param.size = atoi(strtok(buffer, " "));
        param.target_color = atoi(strtok(NULL, " "));
        param.iteration_count = atoi(strtok(NULL, " "));
        param.population_size = atoi(strtok(NULL, " "));
        test_count = atoi(strtok(NULL, " "));
        strcpy(param.graph_filename, strtok(NULL, " "));
        strcpy(param.weight_filename, strtok(NULL, " "));
        strcpy(param.result_filename, strtok(NULL, " "));

        int best_fitness = __INT_MAX__;
        for(;  test_count > 0; test_count--)
            test_graph(&param, &best_fitness);
    }

    fclose(test_list_file);
    fclose(summary_file);
}
