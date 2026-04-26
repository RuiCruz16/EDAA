#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

// Represents a connection to another vertex
struct Edge {
    std::string to; // Name of the destination vertex
    int weight;
};

// Represents a single vertex in the graph
struct Vertex {
    std::string name;        // The identifier read from the file
    std::vector<Edge> edges; // Adjacency list for this specific vertex
};

class Graph {
private:
    // Maps a vertex's name to its corresponding Vertex object
    std::unordered_map<std::string, Vertex> vertices; 

public:
    // Adds a new vertex to the graph by its name
    void addVertex(const std::string& name);

    // Adds an undirected edge between two vertices
    void addEdge(const std::string& from, const std::string& to, int weight);

    // Prints the graph structure for debugging
    void printGraph() const;

    // Getters
    size_t getVerticesCount() const;
};

#endif // GRAPH_H
