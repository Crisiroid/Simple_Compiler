#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <string>
#include <vector>

class Operators
{
public:
    static const std::vector<std::string>& getOperators() {
        static std::vector<std::string> operators = {
            "+" , "-" , "*" , "/" , "/" ,
            "=" , "++" , "--" , "==" , "!=" ,
            "<=" 

        };
        return operator;
    }
};

#endif // OPERATORS_HPP