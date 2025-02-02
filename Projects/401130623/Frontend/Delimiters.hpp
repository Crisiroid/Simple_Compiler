#ifndef DELIMITERS_HPP
#define DELIMITERS_HPP

#include <string>
#include <vector>

class Delimiters
{
public:
    static const std::vector<std::string>& getDelimiters() {
        static std::vector<std::string> delimiters = {
            "(" , ")" , "{" , "}" , "[" , "]" , ";" , "," , "." , ":"
        };
        return delimiters;
    }
};

#endif // DELIMITERS_HPP