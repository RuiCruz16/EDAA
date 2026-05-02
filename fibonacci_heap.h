#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>

// Our logistic "Post-it" (Node)
struct FibNode {
    std::string vertex;
    int distance;
    int degree;
    bool marked;
    
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;

    FibNode(std::string v, int d) : vertex(v), distance(d), degree(0), marked(false),
                                    parent(nullptr), child(nullptr), left(this), right(this) {}
};

class FibonacciHeap {
private:
    FibNode* min_node;
    int total_nodes;
    
    // Hash map to find nodes instantly in O(1) time
    std::unordered_map<std::string, FibNode*> node_map;

    // Internal functions of the logistic engine
    void consolidate();
    void link(FibNode* y, FibNode* x);
    void cut(FibNode* x, FibNode* y);
    void cascading_cut(FibNode* y);
    void clear_nodes(FibNode* node);

public:
    FibonacciHeap();
    ~FibonacciHeap();

    // Public functions that Dijkstra will use
    void insert(const std::string& vertex, int distance);
    std::pair<std::string, int> extract_min();
    void decrease_key(const std::string& vertex, int new_distance);
    bool is_empty() const;
    bool contains(const std::string& vertex) const;
};

#endif // FIBONACCI_HEAP_H
