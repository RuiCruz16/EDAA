#include "graph.h"
#include <queue>
#include <climits>

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

int Graph::getDegree(const std::string& name) const {
    auto it = vertices.find(name);
    
    if (it != vertices.end()) {
        return it->second.edges.size();
    }
    
    return 0;
}

std::vector<std::string> Graph::getVertexNames() const {
    std::vector<std::string> names;
    
    for (const auto& pair : vertices) {
        names.push_back(pair.first);
    }
    
    return names;
}

int Graph::getTotalWeight() const {
    int total = 0;
    
    for (const auto& pair : vertices) {
        for (const auto& edge : pair.second.edges) {
            total += edge.weight;
        }
    }
    
    return total / 2;
}

std::vector<std::string> Graph::getOddVertices() const {
    std::vector<std::string> odd_nodes;
    for (const auto& pair : vertices) {
        if (pair.second.edges.size() % 2 != 0) {
            odd_nodes.push_back(pair.first);
        }
    }
    return odd_nodes;
}

std::unordered_map<std::string, int> Graph::dijkstra_priority_queue(const std::string& start) const {
    std::unordered_map<std::string, int> distances;
    
    // 1. Initialize all distances to "infinity"
    for (const auto& pair : vertices) {
        distances[pair.first] = INT_MAX;
    }
    distances[start] = 0; // The distance to the starting point itself is 0

    // 2. Create the classic C++ Min-Heap: stores pairs of (distance, node_name)
    // We use greater<> so that the shortest distance is always at the top
    std::priority_queue<std::pair<int, std::string>, 
                        std::vector<std::pair<int, std::string>>, 
                        std::greater<std::pair<int, std::string>>> pq;

    pq.push({0, start});

    // 3. Main Dijkstra loop
    while (!pq.empty()) {
        int current_dist = pq.top().first;
        std::string current_vertex = pq.top().second;
        pq.pop();

        // If we found this node in the queue with an old/greater distance, we ignore it
        if (current_dist > distances[current_vertex]) continue;

        // 4. Explore all neighboring streets (edges)
        for (const auto& edge : vertices.at(current_vertex).edges) {
            int new_dist = current_dist + edge.weight;
            
            // If the new shortcut is better than the known distance, update it!
            if (new_dist < distances[edge.to]) {
                distances[edge.to] = new_dist;
                pq.push({new_dist, edge.to});
            }
        }
    }
    
    return distances;
}

std::unordered_map<std::string, int> Graph::dijkstra_fibonacci_heap(const std::string& start) const {
    std::unordered_map<std::string, int> distances;

    // 1. Initialise all distances to infinity
    for (const auto& pair : vertices) {
        distances[pair.first] = INT_MAX;
    }
    distances[start] = 0;

    // 2. Only insert the source — the heap stays small until nodes are discovered.
    //    Neighbours enter via insert() on first discovery and are updated via
    //    decrease_key() on subsequent relaxations. No stale duplicates ever exist.
    FibonacciHeap fh;
    fh.insert(start, 0);

    // 3. Main loop
    while (!fh.is_empty()) {
        auto [current_vertex, current_dist] = fh.extract_min();

        // All remaining nodes are unreachable
        if (current_dist == INT_MAX) break;

        // Stale extraction guard (cannot occur with proper decrease_key, but
        // provides safety against any edge case)
        if (current_dist > distances[current_vertex]) continue;

        // 4. Relax all outgoing edges
        for (const auto& edge : vertices.at(current_vertex).edges) {
            int new_dist = current_dist + edge.weight;

            if (new_dist < distances[edge.to]) {
                distances[edge.to] = new_dist;

                if (fh.contains(edge.to)) {
                    // Node already in heap: O(1) amortized key update
                    fh.decrease_key(edge.to, new_dist);
                } else {
                    // First time we reach this neighbour: insert it
                    fh.insert(edge.to, new_dist);
                }
            }
        }
    }

    return distances;
}
