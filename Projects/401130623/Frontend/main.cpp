#include <iostream>
#include <fstream>
#include "Frontend/Scanner.hpp"

void scanFile(const std::string& filePath){
    std::ifstram file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: could not open the file " << filePath <<std::endl;
        return;
    }

    Scanner scanner;
    std::string line;
    int lineNumber = 1;

    while (std::getline( file, line)) {
        std::vector<Token> tokens = scanner.scan(line, lineNumber);
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.value << ", Type: "
                    <<static_cast<int>(token.type) << ", line: " << token.line << std::endl;
        }
        lineNumber++;
    }
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc !=2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" <<std::endl;
        return 1;
    }
    std::string filePath = arg[1];
    scanFile(filePath);

    return 0;
}