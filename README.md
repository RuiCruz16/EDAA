# C++ Chinese Postman Problem Solver

This project is a C++ implementation of the **Chinese Postman Problem (CPP)**, focusing on optimization using a **Fibonacci Heap**.

## Current File Structure
* `main.cpp` - Application entry point.
* `graph.h` / `graph.cpp` - `Graph` class representing the network using an adjacency list (supporting custom vertex names via Hash Map).
* `reader.h` / `reader.cpp` - Dedicated module for reading and parsing text files.
* `graphs/` - Directory containing all the graphs.

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

**1. Compile the Code**
Open your terminal in the project folder and use the `g++` compiler to link all source files together:

```bash
g++ main.cpp graph.cpp fibonacci_heap.cpp reader.cpp -o exec
```

**2. Run the Program**
After compilation generates the exec file, you can run the program with the following command:

```bash
./exec
```
