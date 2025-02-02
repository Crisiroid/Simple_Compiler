#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

unordered_map<string, string> symbolTable;  // Symbol table to store variable types

// Function to declare a variable and add it to the symbol table
void declareVariable(string varName, string varType) {
    symbolTable[varName] = varType;
}

// Function to check if a variable is declared
bool isDeclared(string varName) {
    return symbolTable.find(varName) != symbolTable.end();
}

// Function to check if an assignment is valid (type matching)
bool checkAssignment(string varName, string valueType) {
    if (!isDeclared(varName)) {
        cout << "Error: Variable " << varName << " is not declared! ?" << endl;
        return false;
    }
    if (symbolTable[varName] != valueType) {
        cout << "Error: Incorrect data type for variable " << varName << "! ?" << endl;
        return false;
    }
    return true;
}

int main() {
    // Simulating a simple program
    declareVariable("x", "int");  // Declare x as an integer

    // Checking correct assignment
    if (checkAssignment("x", "int")) {
        cout << "Assignment to " << "x" << " is correct ?" << endl;
    }

    // Checking incorrect type assignment
    checkAssignment("x", "string");  // ? Type mismatch error

    // Checking undeclared variable
    checkAssignment("y", "int");  // ? Undeclared variable error

    return 0;
}
