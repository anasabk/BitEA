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
 * @brief Get a random color not used previously in the used_color_list.
 * When a color is returned, it is added to the used_color_list.
 * 
 * @param size Max number of colors.
 * @param colors_used Number of colors used.
 * @param used_color_list List of used colors.
 * @return If an unused color is found, return it. If all colors are 
 * used, return -1.
 */
int get_rand_color(int max_color_num, int colors_used, block_t *used_color_list);


/**
 * @brief Merge two parent colors with the pool into the child color. Vertices are
 * checked if used previously through used_vertex_list before being added
 * to the child color.
 * 
 * @param size Size of the graph.
 * @param parent_color Array of pointers to two parents.
 * @param child_color Pointer to the child color.
 * @param pool Pool.
 * @param pool_total Total number of vertices in the pool.
 * @param used_vertex_list List of used vertices.
 * @return Return the total number of newly used vertices.
 */
void merge_and_fix(
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


/**
 * @brief Remove conflicts from color until no conflicts remain in it.
 * 
 * @param graph_size Size of the graph.
 * @param edges The edge matrix of the graph.
 * @param weights The array of weights of vertices.
 * @param conflict_count Array of number of conflicts for each vertex.
 * @param total_conflicts Total number of conflicts.
 * @param color Color to be modified.
 * @param pool Pool.
 * @param pool_total Number of vertices in the pool.
 */
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
 * @brief Performs a crossover between two parents to produce
 * a new individual.
 * 
 * @param graph_size Size of the graph.
 * @param edges The edge matric of the graph.
 * @param weights The array of weights of vertices.
 * @param color_num1 Number of colors of the first parent.
 * @param color_num2 Number of colors of the second parent.
 * @param parent1 The first parent.
 * @param parent2 The second parent.
 * @param target_color_count Target color count of the new individual.
 * @param child Output pointer to the result individual.
 * @param child_color_count Output pointer to the number of colors of
 * the new individual.
 * @param uncolored Output pointer to the number of uncolored vertices in the best solution.
 * @return Return the fitness of the new individual.
 */
int crossover (
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


#endif