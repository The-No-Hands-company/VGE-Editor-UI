#include <iostream>
#include <fstream>
#include <vector>

int main() {
    // This is a minimal TTF file that contains just the header
    std::vector<unsigned char> ttfHeader = {
        0x00, 0x01, 0x00, 0x00,  // TTF version 1.0
        0x00, 0x0A,              // Number of tables (10)
        0x00, 0x80,              // Search range
        0x00, 0x03,              // Entry selector
        0x00, 0x60               // Range shift
    };

    std::ofstream outFile("../Fonts/Roboto-Regular.ttf", std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to create font file" << std::endl;
        return 1;
    }

    outFile.write(reinterpret_cast<const char*>(ttfHeader.data()), ttfHeader.size());
    outFile.close();

    std::cout << "Created minimal TTF file" << std::endl;
    return 0;
}
