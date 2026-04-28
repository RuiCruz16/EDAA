#include <iostream>
#include <vector>
#include <string>
#include "graph.h"
#include "reader.h"

using namespace std;
using namespace GraphReader;

// Function to handle the perfect Eulerian Circuit scenario
void calculate_euler(const Graph& g) {
    int optimal_cost = g.getTotalWeight();
    
    cout << "-> Final optimal cost for the Postman: " << optimal_cost << "\n";
}

// Function to handle the odd-degree vertices matching scenario
void calculate_dijkstra(const Graph& g) {
    vector<string> odd_nodes = g.getOddVertices();
    cout << "Found " << odd_nodes.size() << " odd-degree vertices.\n";

    if (odd_nodes.empty()) return;

    // 1. Create a "matrix" (Map of Maps) to store distances between all odd nodes
    unordered_map<string, unordered_map<string, int>> odd_distances;

    cout << "\nCalculating shortest paths between all odd nodes...\n";
    
    // 2. Run Dijkstra with EACH odd node as the starting point
    for (const string& start_node : odd_nodes) {
        
        unordered_map<string, int> dists = g.dijkstra_algorithm(start_node);
        
        // Store the distances to all OTHER odd nodes in our matrix
        for (const string& target_node : odd_nodes) {
            if (start_node != target_node) {
                odd_distances[start_node][target_node] = dists[target_node];
            }
        }
    }

    // 3. Print the results to verify (Using i and j so we don't print A->B and B->A)
    cout << "\n--- Distance Matrix for Odd Nodes ---\n";
    for (size_t i = 0; i < odd_nodes.size(); ++i) {
        for (size_t j = i + 1; j < odd_nodes.size(); ++j) {
            string u = odd_nodes[i];
            string v = odd_nodes[j];
            cout << "Path [" << u << " <-> " << v << "] costs: " << odd_distances[u][v] << "\n";
        }
    }
    
    cout << "\n#TODO: 1. Implement Perfect Matching to choose the best pairs.\n";
    cout << "#TODO: 2. Calculate total cost = cost of the even vertices + cost of the perfect matching.\n";
    cout << "#TODO: 3. Implement Fibonacci Heap optimization inside Dijkstra.\n";
}

int main() {
    Graph g;
    string filename = "graphs/small_graph_1.txt";

    if (!loadFromFile(filename, g)) {
        cerr << "Error: Failed to load the graph." << "\n";
        return 1;
    }

    cout << "Graph loaded. Total vertices: " << g.getVerticesCount() << "\n";

    // Check if the graph is an Eulerian Circuit
    bool is_euler_circuit = true;
    vector<string> allVertices = g.getVertexNames();

    for (const string& v : allVertices) {
        if (g.getDegree(v) % 2 != 0) {
            is_euler_circuit = false;
            break; // Stop checking as soon as one odd vertex is found
        }
    }

    if (is_euler_circuit) {
        cout << "Status: Eulerian Circuit (All vertices have an even degree).\n";
        calculate_euler(g);
    } else {
        cout << "Status: NOT an Eulerian Circuit (Odd-degree vertices detected).\n";
        calculate_dijkstra(g);
    }

    return 0;
}
