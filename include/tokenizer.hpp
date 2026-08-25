





















#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>          
#include <vector>          
#include <unordered_set>   







class Tokenizer {
public:
    
    
    
    
    
    
    
    Tokenizer();

    
    
    

    
    
    
    
    
    
    
    std::vector<std::string> process(const std::string& text) const;

    
    
    

    
    
    
    static std::string toLowercase(const std::string& text);

    
    
    
    
    static std::string removePunctuation(const std::string& text);

    
    
    
    
    static std::vector<std::string> tokenize(const std::string& text);

    
    
    
    
    std::vector<std::string> removeStopWords(
        const std::vector<std::string>& tokens
    ) const;

    
    
    
    
    
    static std::vector<std::string> stemTokens(
        const std::vector<std::string>& tokens
    );

    
    
    size_t getStopWordCount() const;

private:
    
    
    
    
    
    
    
    
    std::unordered_set<std::string> stopWords_;

    
    
    static std::unordered_set<std::string> buildStopWordSet();
};

#endif 
