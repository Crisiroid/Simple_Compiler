#ifndef KEYWORDS_HPP
#define KEYWORDS_HPP

#include <string>
#include <vector>

class Keywords
{
public:
    static const std::vector<std::string>& getKeywords() {
        static std::vector<std::string> keywords = {
            "float" , "int" , "return" , "if" , "else" ,
            "for" , "while" , "do" , "char" , "double" ,
            "string" , "let"

        };
        return keywords;
    }
};

#endif // KEYWORDS_HPP