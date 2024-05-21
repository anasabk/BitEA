#ifndef BITEA_H
#define BITEA_H


#include <stdatomic.h>
#include <inttypes.h>
#include "stdgraph.h"


struct crossover_param_s {
    int base_color_count;
    atomic_int *target_color_count;
    atomic_int *best_i;
    atomic_int *child_count;
    int size;
    int max_gen_num;
    block_t *edges;
    int *weights;
    int *color_count;
    int *fitness;
    int *uncolored;
    int population_size;
    block_t *population;
    atomic_long *used_parents;
};

struct crossover_result_s {
    int best_i;
    int iteration_count;
    double best_time;
};

typedef struct {
    block_t *color_mat;
    int color_num;
    int fitness;
    int uncolored;
} individual_t;



/**
 * @brief Color the graph using a genetic algorithm.
 * 
 * @param size Size of the graph.
 * @param edges The edge matrix of the graph.
 * @param weights The array of weights of vertices.
 * @param base_color_count The base number of colors to start from.
 * @param max_gen_num The maximum number allowed of generated children.
 * @param best_solution Output pointer to the result color matrix.
 * @param best_fitness Output pointer to the result solution fitness.
 * @param best_solution_time Output pointer to the time it took to find the last solution.
 * @returns Number of colors in the solution.
 */
int BitEA(
    int graph_size, 
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    int weights[], 
    int population_size,
    int base_color_count, 
    int max_gen_num, 
    block_t best_solution[][TOTAL_BLOCK_NUM(graph_size)], 
    int *best_fitness, 
    float *best_solution_time,
    int *best_itertion,
    int *uncolored_num
);


/**
 * @brief Get a random color not used previously in the used_color_list.
 * When a color is returned, it is added to the used_color_list.
 * 
 * @param size Max number of colors.
 * @param colors_used Number of colors used.
 * @param used_color_list List of used colors.
 * @return If an unused color is found, return it. If all colors are 
 * used, return -1.
 */
int get_rand_color(int max_color_num, int colors_used, block_t used_color_list[]);


/**
 * @brief Merge two parent colors and the pool into the child color. Vertices are
 * checked if used previously through used_vertex_list before being added
 * to the child color.
 * 
 * @param size Size of the graph.
 * @param parent_color Array of pointers to two parents.
 * @param child_color Pointer to the child color
 * @param pool Pool to be dumped into the child color.
 * @param pool_total Total number of vertices in the pool.
 * @param used_vertex_list List of used vertices.
 * @return Return the total number of newly used vertices.
 */
void crossover(
    int graph_size,
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    const int weights[],
    const block_t *parent_color[2],
    block_t child_color[],
    block_t pool[],
    int *pool_count,
    block_t used_vertex_list[],
    int *used_vertex_count
);


void fix_conflicts(
    int graph_size,
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    const int values[],
    int conflict_count[],
    int *total_conflicts,
    block_t color[],
    block_t pool[],
    int *pool_total
);


void search_back(
    int graph_size,
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    const int weights[],
    block_t child[][TOTAL_BLOCK_NUM(graph_size)], 
    int current_color,
    block_t pool[],
    int *pool_count
);


void local_search(
    int graph_size,
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    const int weights[],
    block_t child[][TOTAL_BLOCK_NUM(graph_size)], 
    int color_count,
    block_t pool[],
    int *pool_count
);


/**
 * @brief Performs crossover between two parents to produce
 * a new individual.
 * 
 * @param size Size of the graph.
 * @param edges The edge matric of the graph.
 * @param num_of_edges List of the degrees of every vertex.
 * @param color_num1 Number of colors of the first parent.
 * @param color_num2 Number of colors of the second parent.
 * @param parent1 The first parent.
 * @param parent2 The second parent.
 * @param target_color_count Target color count of the new individual.
 * @param child Output pointer to the result individual.
 * @param child_color_count Output pointer to the number of colors of
 * the new individual.
 * @return Return the fitness of the new individual.
 */
int generate_child (
    int graph_size, 
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    const int weights[],
    int color_num1, 
    int color_num2, 
    const block_t parent1[][TOTAL_BLOCK_NUM(graph_size)], 
    const block_t parent2[][TOTAL_BLOCK_NUM(graph_size)], 
    int target_color_count,
    block_t child[][TOTAL_BLOCK_NUM(graph_size)],
    int *child_color_count,
    int *uncolored
);


void* generator_thread(void *param);


#endif