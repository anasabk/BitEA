# BitEA: BitVertex Evolutionary Algorithm
This is the implementation of the BitVertex Evolutionary Algorithm (BitEA), a novel approach grounded in bitwise operations 
and bit-based solutions, designed to accelerate computational performance in register allocation.

## Dependencies:
- A computer running Linux.
- Make.
- gcc.

## Usage:
### Building:
    make build
### Running:
    ./BitEA <path to the test file> <path to the summary file>
### Example:
    ./BitEA tests/DSJC125.1g.test.txt DSJC125.1g.summary.txt
### Format of Test File:
    <# of vertices> <# of colors> <# of iterations> <population size> <# of test runs> <Path to graph file> <Path to weight file> <Path to best solution file>
    .
    .
    .
    .
#### Example (DSJC125.1g.test.txt):
    125 6 100000 100 5 instances/DIMACS/DSJC125.1g.edgelist instances/DIMACS/DSJC125.1g.col.w DSJC125.1g.solution.txt
This is a test for the DSJC125.1g graph. The parameters are:
- 125 vertices.
- 6 colors.
- 100000 iterations.
- 100 individuals in the population.
- 5 test runs.
- Path to edgelist file: DSJC125.1g.edgelist
- Path to weight file: DSJC125.1g.col.w
- Path to the best solution file: DSJC125.1g.solution.txt

### Outputs:
- The summary file contains the following information of each run (ex: DSJC125.1g.summary.txt):

        |  graph name   | target color | k time | k | cost | uncolored | total time |
        |graph_datasets/DSJC125.1g.edgelist|  6|  0.662206|  6|    0|  0|  3.412955|
        .
        .
        .
        .

    - graph name: The tested graph instance file name.
    - target color: The number of colors the operation started at.
    - k time: The time when the best solution was obtained with respect to the start of the operation.
    - cost: The fitness value of the best solution obtained in the run.
    - uncolored: The number of uncolored vertices in the best solution of the run.
    - total time: The total time spent during the run. 


- The solution file contains the following information of the best run (ex: DSJC125.1g.solution.txt):

        |  graph name   | target color | k time | k | cost | uncolored | total time |
        |graph_datasets/DSJC125.1g.edgelist|  6|  0.662206|  6|    0|  0|  3.412955|


        0 1
        <Color_id> <Vertex_id>
        .
        .
        .
        .

        This indicates that Vertex_id is colored by Color_id in the solution.

