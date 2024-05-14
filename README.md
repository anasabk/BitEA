# BitEA: BitVertex Evolutionary Algorithm
It is an implementation of the evolutionary algorithm InCEA that uses SIMD and logical operation instructions to access and modify data, with some minor modifications to the original method to increase efficiency and performance. This implementation reduces time and space costs by magnitudes compared to the original implementation. The algorithm mainly targets weighted-vertex graph coloring problems, like register allocation.

## Dependencies:
- A computer running Linux.
- Make.
- gcc.

## Usage:
### Building:
    make build
### Running:
    ./BitEA <path to test file> <path to result file>
### Format of Test File:
    <# of vertices> <# of colors> <# of iterations> <population size> <# of test runs> <Path to graph file> <Path to weight file> <Path to result file>
    .
    .
    .
    .
#### Example:
    125 5 160000 100 5 DSJC125.1g.edgelist DSJC125.1g.col.w DSJC125.1g.result.txt
This is a test for the DSJC125.1g graph. The parameters are:
- 125 vertices.
- 5 colors.
- 160000 iterations.
- 100 individuals in the population.
- 5 test runs.
- Path to edgelist file: DSJC125.1g.edgelist
- Path to weight file: DSJC125.1g.col.w
- Path to result file: DSJC125.1g.result.txt
