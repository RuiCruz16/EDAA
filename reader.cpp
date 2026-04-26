#include "reader.h"
#include <fstream>
#include <iostream>

namespace GraphReader {

    bool loadFromFile(const std::string& filename, Graph& g) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "'!" << std::endl;
            return false;
        }

        int V, E;
        // Read the first line: Expected Vertices and Edges
        if (!(file >> V >> E)) {
            std::cerr << "Error: File format is incorrect or empty!" << std::endl;
            return false;
        }

        // Read all edges
        for (int i = 0; i < E; ++i) {
            std::string u, v;
            int weight;
            
            if (file >> u >> v >> weight) {
                // addEdge already handles the creation of missing vertices
                g.addEdge(u, v, weight);
            } else {
                std::cerr << "Warning: Failed to read edge " << i + 1 << " properly.\n";
            }
        }

        file.close();
        return true;
    }

} // end namespace GraphReader
