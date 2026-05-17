#include "fibonacci_heap.h"

FibonacciHeap::FibonacciHeap() : min_node(nullptr), total_nodes(0) {}

FibonacciHeap::~FibonacciHeap() {
    if (min_node) {
        clear_nodes(min_node);
    }
}

void FibonacciHeap::clear_nodes(FibNode* head) {
    if (!head) return;
    FibNode* current = head;
    do {
        FibNode* temp = current;
        current = current->right;
        if (temp->child) {
            clear_nodes(temp->child);
        }
        delete temp;
    } while (current != head);
}

bool FibonacciHeap::is_empty() const {
    return min_node == nullptr;
}

bool FibonacciHeap::contains(const std::string& vertex) const {
    return node_map.find(vertex) != node_map.end();
}

// Inserting is super fast O(1): we simply add it to the main circular list
void FibonacciHeap::insert(const std::string& vertex, int distance) {
    FibNode* node = new FibNode(vertex, distance);
    node_map[vertex] = node;

    if (min_node != nullptr) {
        // Correct splice order: save old neighbour reference before overwriting
        node->left            = min_node;
        node->right           = min_node->right;   // (1) save original right neighbour
        min_node->right->left = node;              // (2) old neighbour points back to node
        min_node->right       = node;              // (3) only now update min_node->right

        if (node->distance < min_node->distance) {
            min_node = node;
        }
    } else {
        min_node = node;
    }
    total_nodes++;
}

// Extracting the minimum is the heaviest operation: amortized O(log N)
std::pair<std::string, int> FibonacciHeap::extract_min() {
    FibNode* z = min_node;
    if (z == nullptr) return {"", -1};

    // Snapshot all children before touching any pointers,
    // because splicing modifies left/right and breaks the original child list traversal
    if (z->child != nullptr) {
        std::vector<FibNode*> children;
        FibNode* c = z->child;
        do {
            children.push_back(c);
            c = c->right;
        } while (c != z->child);

        for (FibNode* child : children) {
            child->right          = min_node->right;
            child->left           = min_node;
            min_node->right->left = child;
            min_node->right       = child;
            child->parent         = nullptr;
        }
    }

    // Remove z from the root list
    z->left->right = z->right;
    z->right->left = z->left;

    if (z == z->right) {
        min_node = nullptr;
    } else {
        min_node = z->right;
        consolidate();
    }

    total_nodes--;

    std::string vertex_name = z->vertex;
    int         dist        = z->distance;

    node_map.erase(vertex_name);
    delete z;

    return {vertex_name, dist};
}

// The great advantage of Fibonacci Heap over binary heap: amortized O(1)
void FibonacciHeap::decrease_key(const std::string& vertex, int new_distance) {
    // Safe lookup — operator[] would insert a null entry if the key is missing
    auto it = node_map.find(vertex);
    if (it == node_map.end()) return;

    FibNode* x = it->second;
    if (new_distance >= x->distance) return; // Only process genuine improvements

    x->distance = new_distance;
    FibNode* y  = x->parent;

    // If heap order is violated, cut x and do cascading cuts up the tree
    if (y != nullptr && x->distance < y->distance) {
        cut(x, y);
        cascading_cut(y);
    }

    // Update the global minimum pointer if needed
    if (x->distance < min_node->distance) {
        min_node = x;
    }
}

// --- Internal Tree Organization Functions ---

void FibonacciHeap::cut(FibNode* x, FibNode* y) {
    if (x == x->right) {
        y->child = nullptr;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    y->degree--;

    // Move x to the root list
    x->left = min_node;
    x->right = min_node->right;
    min_node->right = x;
    x->right->left = x;
    x->parent = nullptr;
    x->marked = false;
}

void FibonacciHeap::cascading_cut(FibNode* y) {
    FibNode* z = y->parent;
    if (z != nullptr) {
        if (!y->marked) {
            y->marked = true;
        } else {
            cut(y, z);
            cascading_cut(z);
        }
    }
}

void FibonacciHeap::link(FibNode* y, FibNode* x) {
    y->left->right = y->right;
    y->right->left = y->left;

    y->parent = x;
    if (x->child == nullptr) {
        x->child = y;
        y->right = y;
        y->left = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right = y;
        y->right->left = y;
    }
    x->degree++;
    y->marked = false;
}

void FibonacciHeap::consolidate() {
    // Upper bound on the maximum degree after consolidation.
    // For a Fibonacci Heap of n nodes, max degree <= floor(log_φ(n))
    // where φ = golden ratio ≈ 1.618. Adding 2 gives a safe margin.
    int max_degree = static_cast<int>(
        std::log(static_cast<double>(total_nodes + 1)) / std::log(1.618)
    ) + 2;

    std::vector<FibNode*> A(max_degree + 1, nullptr); // degree → tree root

    // Snapshot of the root list before we modify it (link() changes pointers)
    std::vector<FibNode*> root_nodes;
    FibNode* curr = min_node;
    do {
        root_nodes.push_back(curr);
        curr = curr->right;
    } while (curr != min_node);

    // Merge trees of equal degree, smallest-distance root always wins
    for (FibNode* w : root_nodes) {
        FibNode* x = w;
        int d      = x->degree;

        while (d <= max_degree && A[d] != nullptr) {  // guard: d <= max_degree
            FibNode* y = A[d];
            if (x->distance > y->distance) std::swap(x, y); // x keeps the min
            link(y, x); // y becomes a child of x
            A[d] = nullptr;
            d++;
        }

        if (d <= max_degree) A[d] = x;
    }

    // Rebuild the root list from the degree-indexed array and locate the new minimum
    min_node = nullptr;
    for (int i = 0; i <= max_degree; i++) {
        if (A[i] == nullptr) continue;
        if (min_node == nullptr) {
            // First tree — start a fresh circular list
            min_node        = A[i];
            min_node->left  = min_node;
            min_node->right = min_node;
        } else {
            // Append A[i] to the right of min_node
            A[i]->right              = min_node->right;
            A[i]->left               = min_node;
            min_node->right->left    = A[i];
            min_node->right          = A[i];
            if (A[i]->distance < min_node->distance) {
                min_node = A[i];
            }
        }
    }
}
