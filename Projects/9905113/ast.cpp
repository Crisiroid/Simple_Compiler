#include <iostream>
#include <vector>
using namespace std;

// Base class for AST
class ASTNode {
public:
    virtual void print(int depth = 0) = 0;  // Function to display the tree
};

// Class for variable or numerical values
class ValueNode : public ASTNode {
public:
    string value;
    
    ValueNode(string val) : value(val) {}

    void print(int depth = 0) override {
        cout << string(depth, ' ') << value << endl;
    }
};

// Class for binary operators (+, -, =)
class BinaryOpNode : public ASTNode {
public:
    string op;
    ASTNode* left;
    ASTNode* right;

    BinaryOpNode(string opr, ASTNode* l, ASTNode* r) : op(opr), left(l), right(r) {}

    void print(int depth = 0) override {
        cout << string(depth, ' ') << op << endl;
        left->print(depth + 2);
        right->print(depth + 2);
    }
};

int main() {
    // Constructing the AST for the statement: Put x = 5 + 3;
    ASTNode* five = new ValueNode("5");
    ASTNode* three = new ValueNode("3");
    ASTNode* plus = new BinaryOpNode("+", five, three);
    ASTNode* x = new ValueNode("x");
    ASTNode* assignment = new BinaryOpNode("=", x, plus);

    // Display the AST
    cout << "Abstract Syntax Tree (AST):" << endl;
    assignment->print();

    // Free memory
    delete five;
    delete three;
    delete plus;
    delete x;
    delete assignment;

    return 0;
}
