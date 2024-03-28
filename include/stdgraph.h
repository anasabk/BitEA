#ifndef STDGRAPH_H
#define STDGRAPH_H


#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>


#define block_t uint64_t
// #define EDGE_BIT_INDEX(small, large)        (((large) * (large - 1)/2) + small)
#define BLOCK_INDEX(bit_index)              ((bit_index)/(sizeof(block_t)*8))
#define MASK_INDEX(bit_index)               ((bit_index)%(sizeof(block_t)*8))
#define MASK(bit_index)                     ((block_t)1 << MASK_INDEX(bit_index))
#define TOTAL_BLOCK_NUM(vertex_num)         (BLOCK_INDEX(vertex_num-1)+1)

#define CHECK_EDGE(vertex1, vertex2, edges) (edges[vertex1][BLOCK_INDEX(vertex2)] & MASK(vertex2))
#define SET_EDGE(vertex1, vertex2, edges)   edges[vertex1][BLOCK_INDEX(vertex2)] |=  MASK(vertex2); edges[vertex2][BLOCK_INDEX(vertex1)] |=  MASK(vertex1); 
#define RESET_EDGE(vertex1, vertex2, edges) edges[vertex1][BLOCK_INDEX(vertex2)] &= ~MASK(vertex2); edges[vertex2][BLOCK_INDEX(vertex1)] &= ~MASK(vertex1);

#define CHECK_COLOR(color, vertex)      (color[BLOCK_INDEX(vertex)] & MASK(vertex))
#define SET_COLOR(color, vertex)        color[BLOCK_INDEX(vertex)] |=  MASK(vertex)
#define RESET_COLOR(color, vertex)      color[BLOCK_INDEX(vertex)] &= ~MASK(vertex)

#define FOR_EACH_EDGE(vertex, counter, graph_size, edges, action) \
int __temp_block = BLOCK_INDEX(vertex), __temp_mask = MASK(vertex); \
for(counter = 0; counter < graph_size; counter++) { \
    if(edges[counter][__temp_block] & __temp_mask) {\
        action \
    }\
}


bool read_graph(const char* filename, int graph_size, block_t edges[][TOTAL_BLOCK_NUM(graph_size)], int offset_i);

bool read_weights(const char* filename, int size, int weights[]);

bool is_valid(
    int graph_size, 
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    int color_num, 
    const block_t colors[][TOTAL_BLOCK_NUM(graph_size)]
);

int count_edges(int graph_size, const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], int degrees[]);

void print_colors(
    const char *filename, 
    const char *header, 
    int color_num, 
    int graph_size, 
    const block_t colors[][TOTAL_BLOCK_NUM(graph_size)]
);

int graph_color_greedy(
    int graph_size, 
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    block_t colors[][TOTAL_BLOCK_NUM(graph_size)], 
    int max_color_possible
);

/**
 * @brief randomly color the graph with max_color being the
 * upper bound of colors used.
 * 
 * @param size Size of the graph.
 * @param edges The edge matrix of the graph.
 * @param colors The result color matrix of the graph.
 * @param max_color The upper bound of colors to be used.
 */
void graph_color_random(
    int graph_size, 
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)],  
    block_t colors[][TOTAL_BLOCK_NUM(graph_size)], 
    int max_color
);

int count_conflicts(
    int graph_size, 
    const block_t color[], 
    const block_t edges[][TOTAL_BLOCK_NUM(graph_size)], 
    int conflict_count[]
);


#endif
