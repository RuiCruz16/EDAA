#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <climits>
#include "graph.h"
#include "reader.h"

using namespace std;
using namespace GraphReader;

// ---------------------------------------------------------
// Recursive Brute Force to find the Minimum Perfect Matching
// ---------------------------------------------------------
int findMinMatching(vector<string> nodes, unordered_map<string, unordered_map<string, int>>& distMatrix) {
    if (nodes.empty()) return 0;

    int min_cost = INT_MAX;
    string first = nodes[0]; // Take the first available node
    
    // Try pairing 'first' with every other available node
    for (size_t i = 1; i < nodes.size(); ++i) {
        string second = nodes[i];
        int pair_cost = distMatrix[first][second];
        
        // Create a list of nodes that are left after this pairing
        vector<string> remaining_nodes;
        for (size_t j = 1; j < nodes.size(); ++j) {
            if (j != i) remaining_nodes.push_back(nodes[j]);
        }
        
        // Recursively find the cost of pairing the remaining nodes
        int total_cost = pair_cost + findMinMatching(remaining_nodes, distMatrix);
        
        if (total_cost < min_cost) {
            min_cost = total_cost;
        }
    }
    
    return min_cost;
}

// ---------------------------------------------------------
// Scenario 1: Perfect Graph (No odd vertices)
// ---------------------------------------------------------
void calculate_euler(const Graph& g) {
    int optimal_cost = g.getTotalWeight();
    
    cout << "\n--- Eulerian Circuit Strategy ---\n";
    cout << "-> Final optimal cost for the Postman: " << optimal_cost << "\n";
}

// ---------------------------------------------------------
// Scenario 2: Graph with Odd Vertices (General Chinese Postman)
// ---------------------------------------------------------
void calculate_non_eulerian(const Graph& g) {
    vector<string> odd_nodes = g.getOddVertices();
    cout << "\n--- Chinese Postman Strategy ---\n";
    cout << "Found " << odd_nodes.size() << " odd-degree vertices.\n";

    if (odd_nodes.empty()) return;

    // Matrix to store distances between all odd nodes
    unordered_map<string, unordered_map<string, int>> odd_distances;

    cout << "Calculating shortest paths between all odd nodes...\n";
    
    // Run Dijkstra with EACH odd node as the starting point
    for (const string& start_node : odd_nodes) {
        unordered_map<string, int> dists = g.dijkstra_algorithm(start_node);
        
        // Store the distances to all OTHER odd nodes
        for (const string& target_node : odd_nodes) {
            if (start_node != target_node) {
                odd_distances[start_node][target_node] = dists[target_node];
            }
        }
    }

    cout << "\nCalculating Perfect Matching...\n";

    // 1. Base cost: Sum of all unique streets
    int base_cost = g.getTotalWeight();

    // 2. Extra cost: Find the cheapest way to pair the odd nodes
    int extra_cost = findMinMatching(odd_nodes, odd_distances);

    // 3. Final calculation
    int final_cost = base_cost + extra_cost;

    cout << "-> Base cost of all streets: " << base_cost << "\n";
    cout << "-> Extra cost (duplicated paths): " << extra_cost << "\n";
    cout << "-------------------------------------------\n";
    cout << "FINAL POSTMAN COST: " << final_cost << "\n";
    cout << "-------------------------------------------\n";

    // TODO: Implement Fibonacci Heap optimization inside Dijkstra
}

// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
int main() {
    Graph g;
    string filename = "graphs/large_graph_1.txt";

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
        calculate_non_eulerian(g);
    }

    return 0;
}
