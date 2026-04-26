#include "graph.h"

// Adds a new vertex if it doesn't already exist
void Graph::addVertex(const std::string& name) {
    // std::unordered_map::find checks if the key exists
    if (vertices.find(name) == vertices.end()) {
        vertices[name] = Vertex{name, std::vector<Edge>()};
    }
}

// Adds an undirected edge
void Graph::addEdge(const std::string& from, const std::string& to, int weight) {
    // Safety check: Ensure both vertices exist before connecting them
    addVertex(from);
    addVertex(to);

    // Add edge from 'from' to 'to'
    vertices[from].edges.push_back({to, weight});
    
    // Add edge from 'to' to 'from' (because it's an undirected graph)
    vertices[to].edges.push_back({from, weight});
}

// Iterates through the map and prints each vertex and its connections
void Graph::printGraph() const {
    std::cout << "--- Graph Structure ---\n";
    for (const auto& pair : vertices) {
        // pair.first is the name (key), pair.second is the Vertex object (value)
        std::cout << "Vertex '" << pair.first << "' connects to: ";
        for (const auto& edge : pair.second.edges) {
            std::cout << "[" << edge.to << " | w:" << edge.weight << "] ";
        }
        std::cout << "\n";
    }
}

// Returns the total number of unique vertices stored
size_t Graph::getVerticesCount() const {
    return vertices.size();
}
