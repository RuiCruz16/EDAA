# EDAA Projects

Repository for the *EDAA* course, containing two independent projects:

1. **Chinese Postman Problem (CPP) Solver** — C++ implementation focusing on optimization using a **Fibonacci Heap**.
2. **Quadtree Visualization** — real-time 2D spatial partitioning simulation in Python demonstrating the efficiency of the Quadtree for collision detection.

---

# Chinese Postman Problem Solver

A C++ implementation of the Chinese Postman Problem. When the graph is not Eulerian, the solver pairs the odd-degree vertices using **Minimum Weight Perfect Matching** (Blossom V), finding the optimal set of edges to duplicate so a postman can traverse every street exactly once and return to the start.

Dijkstra's algorithm is available with two priority engines — a custom **Fibonacci Heap** and a standard priority queue — so their performance can be benchmarked.

## File Structure

* `main.cpp` - Application entry point (holds hardcoded graph selection and benchmarking logic).
* `graph.h` / `graph.cpp` - `Graph` class representing the network using an adjacency list (supports custom vertex names via Hash Map).
* `fibonacci_heap.h` / `fibonacci_heap.cpp` - Fibonacci Heap implementation for optimized Dijkstra.
* `reader.h` / `reader.cpp` - Module for reading and parsing the graph text files.
* `graphs/` - Directory containing all test graphs.
* `blossom5/` - Blossom V library (Kolmogorov) for minimum weight perfect matching.

## Test Graph Format (`.txt`)

Each graph file follows a simple format:

1. The first line contains the number of Vertices (`V`) and Edges (`E`).
2. The following `E` lines represent the connections: `Source Destination Weight`.

Example:

```text
6 9
0 1 5
1 2 4
...
```

### Available Test Graphs

| File | Vertices | Edges | Notes |
| ---- | -------- | ----- | ----- |
| `graphs/small_graph_1.txt` | 6 | 9 | Default graph used by `main.cpp` |
| `graphs/small_graph_2.txt` | 8 | 14 | |
| `graphs/medium_graph_1.txt` | 16 | 24 | |
| `graphs/medium_graph_2.txt` | 20 | 30 | |
| `graphs/test_graph_1.txt` | 100 | 4,950 | Complete graph |
| `graphs/test_graph_2.txt` | 300 | 44,850 | Complete graph |
| `graphs/test_graph_3.txt` | 500 | 124,750 | Complete graph |

> **Note:** `main.cpp` loads a fixed file (`graphs/small_graph_1.txt`).
> To run the solver on a different graph, change the `filename` variable in `main.cpp` (`main.cpp:181`) and recompile.

## How to Compile and Run

**Prerequisites:** `g++` with C++11 or later.

`blossom5/` is already bundled in this repository, so no download is needed to build.
(To re-fetch a fresh copy: `wget https://pub.ist.ac.at/~vnk/software/blossom5-v2.05.src.tar.gz` and extract it, renaming the folder to `blossom5`.)

**1. Compile the Code**

Open a terminal in the project folder and link all the source files:

```bash
g++ -O3 main.cpp graph.cpp fibonacci_heap.cpp reader.cpp \
    blossom5/PMinterface.cpp \
    blossom5/PMmain.cpp \
    blossom5/PMinit.cpp \
    blossom5/PMshrink.cpp \
    blossom5/PMexpand.cpp \
    blossom5/PMduals.cpp \
    blossom5/misc.cpp \
    blossom5/MinCost/MinCost.cpp \
    -o exec
```

**2. Run the Program**

```bash
./exec
```

The program loads the graph, checks whether the vertices all have even degree (Eulerian circuit) and, if not, computes the optimal Chinese Postman solution while printing a benchmark of the Dijkstra phase timing.

---

# Quadtree Visualization

Real-time simulation demonstrating the algorithmic efficiency of the Quadtree data structure for 2D collision detection, benchmarked against brute force.

## How to Run

**Prerequisites:** Python 3 with `pygame`.

```bash
pip install pygame
python3 quadtree_visualization.py
```

## Controls

| Control | Action |
| ------- | ------ |
| **Mode** button | Toggle between Quadtree and Brute-Force collision detection |
| **Grid** button | Toggle the Quadtree partitioning grid overlay |
| **Ball count** input box | Type a number and press `Enter` to spawn that many balls (capped between 2 and 3000) |
| `Esc` or window close | Exit the simulation |
