#ifndef READER_H
#define READER_H

#include <string>
#include "graph.h"

// Namespace dedicated to handling graph input operations
namespace GraphReader {
    
    // Reads a graph from a text file and populates the given Graph object.
    // Returns true on success, false if an error occurred.
    bool loadFromFile(const std::string& filename, Graph& g);
    
}

#endif // READER_H
