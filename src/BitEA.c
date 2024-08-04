#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "BitEA.h"
#include "stdgraph.h"



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
) {
    // Create the random population.
    block_t *population[population_size];
    int color_count[population_size];
    int uncolored[population_size];
    int fitness[population_size];
    for (int i = 0; i < population_size; i++) {
        population[i] = calloc(base_color_count * TOTAL_BLOCK_NUM((size_t)graph_size), sizeof(block_t));
        uncolored[i] = base_color_count;
        color_count[i] = base_color_count;
        fitness[i] = __INT_MAX__;
    }

    pop_complex_random(
        graph_size,
        edges,
        weights,
        population_size,
        population,
        base_color_count
    );

    // Create and initialize the list of used parents.
    block_t used_parents[(TOTAL_BLOCK_NUM(population_size))];
    memset(used_parents, 0, (TOTAL_BLOCK_NUM(population_size))*sizeof(block_t));

    // Initialize generator parameters.
    int best_i = 0;
    int target_color = base_color_count;

    struct timeval t1, t2;
    *best_solution_time = 0;
    gettimeofday(&t1, NULL);

    // Keep generating solutions for max_gen_num generations.
    block_t *child = malloc(base_color_count * TOTAL_BLOCK_NUM(graph_size) * sizeof(block_t));
    int temp_uncolored;
    int parent1, parent2, child_colors, temp_fitness;
    int bad_parent;
    // int best_iteration = 0;
    for(int i = 0; i < max_gen_num; i++) {
        if(target_color == 0)
            break;

        // Initialize the child
        memset(child, 0, (TOTAL_BLOCK_NUM(graph_size))*base_color_count*sizeof(block_t));

        // Pick 2 random parents
        do { parent1 = rand()%population_size; } while (CHECK_COLOR(used_parents, parent1));
        SET_COLOR(used_parents, parent1);
        do { parent2 = rand()%population_size; } while (CHECK_COLOR(used_parents, parent2));
        SET_COLOR(used_parents, parent2);

        // Do a crossover
        temp_fitness = generate_child (
            graph_size, 
            edges, 
            weights,
            color_count[parent1], 
            color_count[parent2], 
            population[parent1], 
            population[parent2], 
            target_color,
            child, 
            &child_colors,
            &temp_uncolored
        );


        if(fitness[parent1] <= fitness[parent2] && color_count[parent1] <= color_count[parent2])
            bad_parent = parent2;
        else
            bad_parent = parent1;

        // Replace a dead parent.
        if(child_colors <= color_count[bad_parent] && temp_fitness <= fitness[bad_parent]) {
            memmove(population[bad_parent], child, (TOTAL_BLOCK_NUM(graph_size))*base_color_count*sizeof(block_t));
            color_count[bad_parent] = child_colors;
            fitness[bad_parent] = temp_fitness;
            uncolored[bad_parent] = temp_uncolored;

            if (temp_fitness < fitness[best_i] ||
                (temp_fitness == fitness[best_i] && child_colors < color_count[best_i])
            ) {
                best_i = bad_parent;
                gettimeofday(&t2, NULL);
                *best_solution_time = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;   // us to ms
            }
        }

        RESET_COLOR(used_parents, parent1);
        RESET_COLOR(used_parents, parent2);

        // Make the target harder if it was found.
        if(temp_fitness == 0)
            target_color = child_colors - 1;
    }

    // Return the best solution
    *best_fitness = fitness[best_i];
    *uncolored_num = uncolored[best_i];
    memcpy(best_solution, population[best_i], base_color_count * (TOTAL_BLOCK_NUM(graph_size)) * sizeof(block_t));

    // Free allocated space.
    free(child);
    for(int i = 0; i < population_size; i++)
        free(population[i]);

    return color_count[best_i];
}

void fix_conflicts(
    int graph_size,
    const block_t *edges, 
    const int *weights,
    int *conflict_count,
    int *total_conflicts,
    block_t *color,
    block_t *pool,
    int *pool_total
) {
    block_t (*edges_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])edges;

    // Keep removing problematic vertices until all conflicts are gone.
    int i, worst_vert = 0, vert_block;
    block_t vert_mask;
    while(*total_conflicts > 0) {
        // Find the vertex with the most conflicts.
        for(i = 0; i < graph_size; i++) {
            if (CHECK_COLOR(color, i) &&
                (conflict_count[worst_vert] < conflict_count[i] ||
                 (conflict_count[worst_vert] == conflict_count[i] && 
                  (weights[worst_vert] > weights[i] || (weights[worst_vert] == weights[i] && rand()%2))))            ) {
                worst_vert = i;
            }
        }

        // Update other conflict counters.
        vert_mask = MASK(worst_vert);
        vert_block = BLOCK_INDEX(worst_vert);
        for(i = 0; i < graph_size; i++)
            if(CHECK_COLOR(color, i) && ((*edges_p)[i][vert_block] & vert_mask))
                conflict_count[i]--;

        // Remove the vertex.
        color[vert_block] &= ~vert_mask;
        pool[vert_block] |= vert_mask;
        (*pool_total)++;

        // Update the total number of conflicts.
        (*total_conflicts) -= conflict_count[worst_vert];
        conflict_count[worst_vert] = 0;
    }
}

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
) {
    // Merge the two colors
    int temp_v_count = 0;  
    if(parent_color[0] != NULL && parent_color[1] != NULL)
        for(int i = 0; i < (TOTAL_BLOCK_NUM(graph_size)); i++) {
            child_color[i] = ((parent_color[0][i] | parent_color[1][i]) & ~(used_vertex_list[i]));
            temp_v_count += popcountl(child_color[i]);
        }

    else if(parent_color[0] != NULL)
        for(int i = 0; i < (TOTAL_BLOCK_NUM(graph_size)); i++) {
            child_color[i] = (parent_color[0][i] & ~(used_vertex_list[i]));
            temp_v_count += popcountl(child_color[i]);
        }

    else if(parent_color[1] != NULL)
        for(int i = 0; i < (TOTAL_BLOCK_NUM(graph_size)); i++) {
            child_color[i] = (parent_color[1][i] & ~(used_vertex_list[i]));
            temp_v_count += popcountl(child_color[i]);
        }

    (*used_vertex_count) += temp_v_count;

    // Merge the pool with the new color
    for(int i = 0; i < (TOTAL_BLOCK_NUM(graph_size)); i++) {
        child_color[i] |= pool[i];
        used_vertex_list[i] |= child_color[i];
    }

    memset(pool, 0, (TOTAL_BLOCK_NUM(graph_size))*sizeof(block_t));
    (*pool_count) = 0;


    // List of conflict count per vertex.
    int conflict_count[graph_size];
    memset(conflict_count, 0, graph_size*sizeof(int));

    // Count conflicts.
    int total_conflicts = count_conflicts(
        graph_size,
        child_color,
        edges,
        conflict_count
    );

    // Fix the conflicts.
    fix_conflicts(
        graph_size,
        edges,
        weights,
        conflict_count,
        &total_conflicts,
        child_color,
        pool,
        pool_count
    );
}

void search_back(
    int graph_size,
    const block_t *edges, 
    const int *weights,
    block_t *child, 
    int color_count,
    block_t *pool,
    int *pool_count
) {
    block_t (*edges_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])edges;
    block_t (*child_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])child;

    int conflict_count, last_conflict, last_conflict_block = 0;
    block_t i_mask, temp_mask, last_conflict_mask = 0;
    int i, j, k, i_block;

    // Search back and try placing vertices from the pool in previous colors.
    for(i = 0; i < graph_size && (*pool_count) > 0; i++) {
        i_block = BLOCK_INDEX(i);
        i_mask = MASK(i);

        // Check if the vertex is in the pool.
        if(pool[i_block] & i_mask) {
            // Loop through every previous color.
            for(j = 0; j < color_count; j++) {
                // Count the possible conflicts in this color.
                conflict_count = 0;
                for(k = 0; k < TOTAL_BLOCK_NUM(graph_size); k++) {
                    temp_mask = (*child_p)[j][k] & (*edges_p)[i][k];
                    if(temp_mask) {
                        conflict_count += popcountl(temp_mask);
                        if(conflict_count > 1)
                            break;
                        last_conflict = sizeof(block_t)*8*(k + 1) - 1 - __builtin_clzl(temp_mask);
                        last_conflict_mask = temp_mask;
                        last_conflict_block = k;
                    }
                }

                // Place immediately if there are no conflicts.
                if(conflict_count == 0) {
                    (*child_p)[j][i_block] |= i_mask;
                    pool[i_block] &= ~i_mask;
                    (*pool_count)--;
                    break;

                // If only 1 conflict exists and its weight is smaller
                // than that of the vertex in question, replace it.
                } else if (conflict_count == 1 && weights[last_conflict] < weights[i]) {
                    (*child_p)[j][i_block] |= i_mask;
                    pool[i_block] &= ~i_mask;

                    (*child_p)[j][last_conflict_block] &= ~last_conflict_mask;
                    pool[last_conflict_block] |= last_conflict_mask;
                    break;
                }
            }
        }
    }
}

void local_search(
    int graph_size,
    const block_t *edges, 
    const int *weights,
    block_t *child, 
    int color_count,
    block_t *pool,
    int *pool_count
) {
    block_t (*edges_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])edges;
    block_t (*child_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])child;

    int i, j, k, h, i_block;
    block_t i_mask, temp_mask;
    int competition;
    int conflict_count;
    block_t conflict_array[TOTAL_BLOCK_NUM(graph_size)];

    // Search back and try placing vertices from the pool in the colors.
    for(i = 0; i < graph_size && (*pool_count) > 0; i++) {
        i_block = BLOCK_INDEX(i);
        i_mask = MASK(i);

        // Check if the vertex is in the pool.
        if(pool[i_block] & i_mask) {
            // Loop through every color.
            for(j = 0; j < color_count; j++) {  
                // Count conflicts and calculate competition
                conflict_count = 0;
                competition = 0;
                for(k = 0; k < TOTAL_BLOCK_NUM(graph_size); k++) {
                    conflict_array[k] = (*edges_p)[i][k] & (*child_p)[j][k];
                    if(conflict_array[k]) {
                        temp_mask = conflict_array[k];
                        conflict_count += popcountl(temp_mask);
                        for(h = 0; h < sizeof(block_t)*8; h++)
                            if((temp_mask >> h) & (block_t)1)
                                competition += weights[k*8*sizeof(block_t)+h];
                    }
                }

                // Place immediately if there are no conflicts.
                if(competition == 0) {
                    (*child_p)[j][i_block] |= i_mask;
                    pool[i_block] &= ~i_mask;
                    (*pool_count) += conflict_count - 1;
                    break;

                /**
                 * If the total competition is smaller than the weight
                 * of the vertex in question, move all the conflicts to the 
                 * pool, and place the vertex in the color.
                */
                } else if(competition < weights[i]) {
                    for(k = 0; k < TOTAL_BLOCK_NUM(graph_size); k++) {
                        (*child_p)[j][k] &= ~conflict_array[k];
                        pool[k] |= conflict_array[k];
                    }

                    (*child_p)[j][i_block] |= i_mask;
                    pool[i_block] &= ~i_mask;
                    (*pool_count) += conflict_count - 1;
                    break;
                }
            }
        }
    }
}

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
) {
    const block_t (*parent1_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])parent1;
    const block_t (*parent2_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])parent2;
    block_t (*child_p)[][TOTAL_BLOCK_NUM(graph_size)] = (block_t (*)[][TOTAL_BLOCK_NUM(graph_size)])child;

    // max number of colors of the two parents.
    int max_color_num = color_num1 > color_num2 ? color_num1 : color_num2;

    // list of used colors in the parents.
    block_t used_color_list[2][TOTAL_BLOCK_NUM(max_color_num)];
    memset(used_color_list, 0, 2*TOTAL_BLOCK_NUM(max_color_num)*sizeof(block_t));

    // list of used vertices in the parents.
    block_t used_vertex_list[TOTAL_BLOCK_NUM(graph_size)];
    memset(used_vertex_list, 0, (TOTAL_BLOCK_NUM(graph_size))*sizeof(block_t));
    int used_vertex_count = 0;

    // Pool.
    block_t pool[TOTAL_BLOCK_NUM(graph_size)];
    memset(pool, 0, (TOTAL_BLOCK_NUM(graph_size))*sizeof(block_t));
    int pool_count = 0;


    int color1, color2, last_color = 0;
    int i, j;
    const block_t *chosen_parent_colors[2];
    for(i = 0; i < target_color_count; i++) {
        // The child still has vertices that weren't used.
        if(used_vertex_count < graph_size) {
            // Pick 2 random colors.
            color1 = get_rand_color(color_num1, i, used_color_list[0]);
            color2 = get_rand_color(color_num2, i, used_color_list[1]);
            chosen_parent_colors[0] = color1 == -1 ? NULL : (*parent1_p)[color1];
            chosen_parent_colors[1] = color2 == -1 ? NULL : (*parent2_p)[color2];

            crossover(
                graph_size,
                edges,
                weights,
                chosen_parent_colors,
                (*child_p)[i],
                pool,
                &pool_count,
                used_vertex_list,
                &used_vertex_count
            );

        // If all of the vertices were used and the pool is empty, exit the loop.
        } else if(pool_count == 0) {
            break;
        }

        search_back(
            graph_size,
            edges,
            weights,
            child, 
            i,
            pool,
            &pool_count
        );
    }

    // Record the last color.
    last_color = i;


    // If not all the vertices were visited, drop them in the pool.
    if(used_vertex_count < graph_size) {
        for(j = 0; j < (TOTAL_BLOCK_NUM(graph_size)); j++)
            pool[j] |= ~used_vertex_list[j];
        pool[TOTAL_BLOCK_NUM(graph_size)] &= ((0xFFFFFFFFFFFFFFFF) >> (TOTAL_BLOCK_NUM(graph_size)*sizeof(block_t)*8 - graph_size));

        pool_count += (graph_size - used_vertex_count);
        used_vertex_count = graph_size;
        memset(used_vertex_list, 0xFF, (TOTAL_BLOCK_NUM(graph_size))*sizeof(block_t));
    }

    local_search(
        graph_size,
        edges,
        weights,
        child,
        target_color_count,
        pool,
        &pool_count
    );

    // If the pool is not empty, randomly allocate the remaining vertices in the colors.
    int fitness = 0, temp_block;
    block_t temp_mask;
    if(pool_count > 0) {
        int color_num;
        for(i = 0; i < graph_size; i++) {
            temp_block = BLOCK_INDEX(i);
            temp_mask = MASK(i);
            if(pool[temp_block] & temp_mask) {
                color_num = rand()%target_color_count;
                (*child_p)[color_num][temp_block] |= temp_mask;

                if(color_num + 1 > last_color)
                    last_color = color_num + 1;

                fitness += weights[i];
            }
        }

    // All of the vertices were allocated and no conflicts were detected.
    } else {
        fitness = 0;
    }

    *uncolored = pool_count;
    *child_color_count = last_color;
    return fitness;
}
