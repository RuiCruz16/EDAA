# C++ Chinese Postman Problem Solver

This project is a C++ implementation of the **Chinese Postman Problem (CPP)**, focusing on optimization using a **Fibonacci Heap**.

## Current File Structure
* `main.cpp` - Application entry point.
* `graph.h` / `graph.cpp` - `Graph` class representing the network using an adjacency list (supporting custom vertex names via Hash Map).
* `fibonacci_heap.h` / `fibonacci_heap.cpp` - Fibonacci Heap implementation for optimized Dijkstra.
* `reader.h` / `reader.cpp` - Dedicated module for reading and parsing text files.
* `graphs/` - Directory containing all test graphs.
* `blossom5/` - Blossom V library (Kolmogorov) for minimum weight perfect matching.

## Test Graph Format (`.txt`)
The program reads graphs from text files structured as follows:
1. The first line contains the number of Vertices (`V`) and Edges (`E`).
2. The following lines represent the connections: `Source Destination Weight`.

Example:
```text
6 9
0 1 5
1 2 4
...
```

## How to Compile and Run

**1. Download Blossom V (required for minimum matching)**

```bash
wget https://pub.ist.ac.at/~vnk/software/blossom5-v2.05.src.tar.gz
tar -xzf blossom5-v2.05.src.tar.gz
mv blossom5-v2.05.src blossom5
```

**2. Compile the Code**
Open your terminal in the project folder and use the `g++` compiler to link all source files together:

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

**3. Run the Program**
After compilation generates the exec file, you can run the program with the following command:

```bash
./exec
```
