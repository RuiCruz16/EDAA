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
        node->left = min_node;
        node->right = min_node->right;
        min_node->right = node;
        node->right->left = node;

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

    if (z->child != nullptr) {
        FibNode* child = z->child;
        do {
            FibNode* next_child = child->right;
            // Add the children of the removed node to the root list
            child->left = min_node;
            child->right = min_node->right;
            min_node->right = child;
            child->right->left = child;
            child->parent = nullptr;
            child = next_child;
        } while (child != z->child);
    }

    z->left->right = z->right;
    z->right->left = z->left;

    if (z == z->right) {
        min_node = nullptr;
    } else {
        min_node = z->right;
        consolidate(); // The great cleanup and tree reorganization!
    }

    total_nodes--;
    
    std::string vertex_name = z->vertex;
    int dist = z->distance;
    
    node_map.erase(vertex_name);
    delete z;

    return {vertex_name, dist};
}

// The great advantage of Fibonacci Heap over binary heap: amortized O(1)
void FibonacciHeap::decrease_key(const std::string& vertex, int new_distance) {
    if (!contains(vertex)) return;

    FibNode* x = node_map[vertex];
    if (new_distance > x->distance) return; // Only update if it's a better shortcut

    x->distance = new_distance;
    FibNode* y = x->parent;

    if (y != nullptr && x->distance < y->distance) {
        cut(x, y);
        cascading_cut(y);
    }

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
    int max_degree = static_cast<int>(std::log2(total_nodes)) + 2;
    std::vector<FibNode*> A(max_degree, nullptr);

    std::vector<FibNode*> root_nodes;
    FibNode* x = min_node;
    if (x != nullptr) {
        do {
            root_nodes.push_back(x);
            x = x->right;
        } while (x != min_node);
    }

    for (FibNode* w : root_nodes) {
        FibNode* x = w;
        int d = x->degree;
        while (A[d] != nullptr) {
            FibNode* y = A[d];
            if (x->distance > y->distance) {
                std::swap(x, y);
            }
            link(y, x);
            A[d] = nullptr;
            d++;
        }
        A[d] = x;
    }

    min_node = nullptr;
    for (int i = 0; i < max_degree; i++) {
        if (A[i] != nullptr) {
            if (min_node == nullptr) {
                min_node = A[i];
                min_node->left = min_node;
                min_node->right = min_node;
            } else {
                A[i]->left = min_node;
                A[i]->right = min_node->right;
                min_node->right = A[i];
                A[i]->right->left = A[i];
                if (A[i]->distance < min_node->distance) {
                    min_node = A[i];
                }
            }
        }
    }
}
