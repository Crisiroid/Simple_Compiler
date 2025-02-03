#include <iostream>
#include <fstream>
#include "Frontend/Scanner.hpp"
#include "Frontend/Parser.hpp"

std::vector<Token> scanFile(const std::string& filename) {
    std::vector<Token> parsedTokens;
    Scanner scanner;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return parsedTokens;
    }

    std::string line;
    int lineNumber = 1;
    while (std::getline(file, line)) {
        std::vector<Token> tokens = scanner.scan(line, lineNumber);
        for (const Token& token : tokens) {
            parsedTokens.push_back(token);
        }
        lineNumber++;
    }

    file.close();

    // Example: Displaying all parsed tokens after file scanning
    std::cout << "\nAll parsed tokens:\n";
    for (const Token& token : parsedTokens) {
        std::cout << "Token: " << token.value << ", Type: " << token.getTypeAsString() << ", Line: " << token.line << std::endl;
    }
    return parsedTokens;

}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::vector<Token> Tokens = scanFile(filePath);
    Parser parser(Tokens);

    if(parser.Parse()) {
        std::cout << "Parsing completed successfully" << std::endl;
    }else {
        std::cout << "Parsing failed" << std::endl;
    }
    return 0;
}