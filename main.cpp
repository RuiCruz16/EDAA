#include <iostream>
#include "graph.h"
#include "reader.h"

int main() {
    Graph g;
    std::string filename = "graphs/small_graph_1.txt";

    std::cout << "Starting Chinese Postman Problem Solver...\n";

    if (GraphReader::loadFromFile(filename, g)) {
        std::cout << "Successfully loaded graph. Graph contains " 
                  << g.getVerticesCount() << " unique vertices.\n";
                  
        g.printGraph();
    } else {
        std::cerr << "Failed to load the graph. Exiting program.\n";
        return 1;
    }

    return 0;
}
