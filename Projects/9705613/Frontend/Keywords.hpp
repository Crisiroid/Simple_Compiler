#ifndef KEYWORDS_HPP
#define KEYWORDS_HPP

#include <string>
#include <vector>

class Keywords {
public:
    static const std::vector<std::string>& getKeywords() {
        static std::vector<std::string> keywords = {
            "Program", "End", "Start", "end", "Print",
            "Read", "If", "Iteration", "Put", "Var"
        };
        return keywords;
    }
};

#endif // KEYWORDS_HPP
