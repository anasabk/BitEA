#ifndef BITEA_H
#define BITEA_H


#include "stdgraph.h"


/**
 * @brief Color a graph using BitEA algorithm.
 * 
 * @param size Size of the graph.
 * @param edges The edge matrix of the graph.
 * @param weights The array of weights of vertices.
 * @param base_color_count The base number of colors to start from.
 * @param max_gen_num The maximum number allowed of generated children.
 * @param best_solution Output pointer to the result color matrix.
 * @param best_fitness Output pointer to the result solution fitness.
 * @param best_solution_time Output pointer to the time it took to find the best solution.
 * @param uncolored_num Output pointer to the number of uncolored vertices in the best solution.
 * @returns Number of colors in the solution.
 */
int BitEA(
    int graph_size, 
    const block_t *edges, 
    int *weights, 
    int population_size,
    int base_color_count, 
    int max_gen_num, 
    block_t *best_solution, 
    int *best_fitness, 
    float *best_solution_time,
    int *uncolored_num
);


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
    const block_t *edges, 
    const int *weights,
    const block_t **parent_color,
    block_t *child_color,
    block_t *pool,
    int *pool_count,
    block_t *used_vertex_list,
    int *used_vertex_count
);


void fix_conflicts(
    int graph_size,
    const block_t *edges, 
    const int *weights,
    int *conflict_count,
    int *total_conflicts,
    block_t *color,
    block_t *pool,
    int *pool_total
);


void search_back(
    int graph_size,
    const block_t *edges, 
    const int *weights,
    block_t *child, 
    int color_count,
    block_t *pool,
    int *pool_count
);


void local_search(
    int graph_size,
    const block_t *edges, 
    const int *weights,
    block_t *child, 
    int color_count,
    block_t *pool,
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
    const block_t *edges, 
    const int *weights,
    int color_num1, 
    int color_num2, 
    const block_t *parent1, 
    const block_t *parent2, 
    int target_color_count,
    block_t *child,
    int *child_color_count,
    int *uncolored
);


void* generator(void *param);


#endif