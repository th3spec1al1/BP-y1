#include <iostream>
#include <fstream>          // ← было забыто
#include <lib/interpreter.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: itmoscript_interpreter <source_file>\n";
        return 1;
    }

    const char* filename = argv[1];
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << '\n';
        return 1;
    }

    if (!interpret(file, std::cout)) {
        std::cerr << "\nInterpreter error occurred.\n";
        return 1;
    }
    return 0;
}
