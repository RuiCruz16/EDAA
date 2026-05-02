#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <climits>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "graph.h"
#include "reader.h"
#include "blossom5/PerfectMatching.h"

using namespace std;
using namespace std::chrono;
using namespace GraphReader;

// ---------------------------------------------------------
// Graph Generator for Benchmarking (Dense Complete Graph)
// ---------------------------------------------------------
void generate_graph(const string& filename, int num_nodes = 2000) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create the file " << filename << "!\n";
        return;
    }

    // Calculate the exact number of edges for a Complete Graph (N * (N-1) / 2)
    long long edges = (long long)num_nodes * (num_nodes - 1) / 2;
    
    cout << "\n--- Generating Graph ---\n";
    cout << "Generating " << num_nodes << " nodes and " << edges << " edges...\n";

    file << num_nodes << " " << edges << "\n";

    // Seed the random number generator
    srand(time(0));
    
    // Generate the connections (every node connects to every other node)
    for(int i = 0; i < num_nodes; i++) {
        for(int j = i + 1; j < num_nodes; j++) {
            // Random weight between 1 and 50
            int weight = (rand() % 50) + 1;
            file << i << " " << j << " " << weight << "\n";
        }
    }

    file.close();
    cout << "Done! File '" << filename << "' generated successfully!\n";
    cout << "-----------------------------------\n\n";
}

// ---------------------------------------------------------
// Minimum Perfect Matching using Blossom V (Kolmogorov)
// Complexity: O(n^3) - handles thousands of vertices efficiently
// ---------------------------------------------------------
int findMinMatching(vector<string> nodes, unordered_map<string, unordered_map<string, int>>& distMatrix) {
    int n = nodes.size();
    
    // Base case: no nodes to match
    if (n == 0) return 0;
    
    // Blossom V requires even number of nodes for perfect matching
    if (n % 2 != 0) {
        throw runtime_error("Cannot find perfect matching with odd number of nodes");
    }
    
    // Number of edges in a complete graph: n*(n-1)/2
    int edgeCount = n * (n - 1) / 2;
    
    // Initialize Blossom V solver
    // Parameters: (number of nodes, number of edges)
    PerfectMatching pm(n, edgeCount);
    
    // Add all edges of the complete graph
    // Blossom V uses integer indices, so we map node names to indices
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int cost = distMatrix[nodes[i]][nodes[j]];
            pm.AddEdge(i, j, cost);
        }
    }
    
    // Solve the minimum weight perfect matching problem
    pm.Solve();
    
    // Calculate total cost by summing matched edge weights
    int totalCost = 0;
    for (int i = 0; i < n; i++) {
        int match = pm.GetMatch(i);
        // Only count each edge once (when i < match)
        if (i < match) {
            totalCost += distMatrix[nodes[i]][nodes[match]];
        }
    }
    
    return totalCost;
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
void calculate_non_eulerian(const Graph& g, bool use_fibonacci_heap) {
    vector<string> odd_nodes = g.getOddVertices();
    cout << "\n--- Chinese Postman Strategy ---\n";
    cout << "Found " << odd_nodes.size() << " odd-degree vertices.\n";

    if (odd_nodes.empty()) return;

    // Matrix to store distances between all odd nodes
    unordered_map<string, unordered_map<string, int>> odd_distances;

    if (use_fibonacci_heap) {
        cout << "Calculating shortest paths using: FIBONACCI HEAP...\n";
    } else {
        cout << "Calculating shortest paths using: PRIORITY QUEUE...\n";
    }

    auto start_time = high_resolution_clock::now();
    
    // Run Dijkstra with EACH odd node as the starting point
    for (const string& start_node : odd_nodes) {
        unordered_map<string, int> dists;
        
        // Choose the engine based on the boolean flag
        if (use_fibonacci_heap) {
            dists = g.dijkstra_fibonacci_heap(start_node);
        } else {
            dists = g.dijkstra_priority_queue(start_node);
        }
        
        // Store the distances to all OTHER odd nodes
        for (const string& target_node : odd_nodes) {
            if (start_node != target_node) {
                odd_distances[start_node][target_node] = dists[target_node];
            }
        }
    }

    auto end_time = high_resolution_clock::now();

    // Calculate the duration
    auto duration = duration_cast<microseconds>(end_time - start_time);
    double time_in_seconds = duration.count() / 1000000.0;
    
    cout << "-> [BENCHMARK] Dijkstra phase took: " << time_in_seconds << " seconds.\n";

    cout << "\nCalculating Perfect Matching...\n";

    // 1. Base cost: Sum of all unique streets
    int base_cost = g.getTotalWeight();

    // 2. Extra cost: Find the cheapest way to pair the odd nodes
    int extra_cost = findMinMatching(odd_nodes, odd_distances);

    // 3. Final calculation
    int final_cost = base_cost + extra_cost;

    cout << "\n-> Base cost of all streets: " << base_cost << "\n";
    cout << "-> Extra cost (duplicated paths): " << extra_cost << "\n";
    cout << "-------------------------------------------\n";
    cout << "FINAL POSTMAN COST: " << final_cost << "\n";
    cout << "-------------------------------------------\n";
}

// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
int main() {
    Graph g;
    string filename = "graphs/medium_graph_2.txt";

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

    bool use_fibonacci_heap = false;

    if (is_euler_circuit) {
        cout << "Status: Eulerian Circuit (All vertices have an even degree).\n";
        calculate_euler(g);
    } else {
        cout << "Status: NOT an Eulerian Circuit (Odd-degree vertices detected).\n";
        calculate_non_eulerian(g, use_fibonacci_heap);
    }

    return 0;
}
