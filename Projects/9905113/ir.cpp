#include <iostream>
#include <vector>
using namespace std;

// Class for AST nodes
class ASTNode {
public:
    virtual string generateIR() = 0;  // Function to generate intermediate code (IR)
};

// Value node class for numerical values
class ValueNode : public ASTNode {
public:
    int value;
    
    ValueNode(int val) : value(val) {}

    string generateIR() override {
        return to_string(value);
    }
};

// Binary operator class for (+, -, =)
class BinaryOpNode : public ASTNode {
public:
    string op;
    ASTNode* left;
    ASTNode* right;

    BinaryOpNode(string opr, ASTNode* l, ASTNode* r) : op(opr), left(l), right(r) {}

    string generateIR() override {
        string leftIR = left->generateIR();
        string rightIR = right->generateIR();

        // Optimization: If both values are numeric, perform the operation directly
        if (isdigit(leftIR[0]) && isdigit(rightIR[0])) {
            int result = (op == "+") ? stoi(leftIR) + stoi(rightIR) : stoi(leftIR) - stoi(rightIR);
            return to_string(result);
        }

        // Create a temporary variable to store the result
        static int tempVarCount = 0;
        string tempVar = "t" + to_string(tempVarCount++);
        cout << tempVar << " = " << leftIR << " " << op << " " << rightIR << endl;
        return tempVar;
    }
};

int main() {
    // Constructing the AST for the statement: Put x = 5 + 3;
    ASTNode* five = new ValueNode(5);
    ASTNode* three = new ValueNode(3);
    ASTNode* plus = new BinaryOpNode("+", five, three);
    ASTNode* x = new ValueNode(0);  // Let's assume "x" is an integer
    ASTNode* assignment = new BinaryOpNode("=", x, plus);

    // Generate IR for the assignment
    cout << "Intermediate Representation (IR):" << endl;
    assignment->generateIR();

    // Free memory
    delete five;
    delete three;
    delete plus;
    delete x;
    delete assignment;

    return 0;
}
