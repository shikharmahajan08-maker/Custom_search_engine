







#include "tokenizer.hpp"
#include "stemmer.hpp"

#include <algorithm>   
#include <cctype>      
#include <sstream>     




Tokenizer::Tokenizer()
    : stopWords_(buildStopWordSet())
{}











std::vector<std::string> Tokenizer::process(const std::string& text) const {
    
    std::string normalized = toLowercase(text);

    
    std::string clean = removePunctuation(normalized);

    
    std::vector<std::string> tokens = tokenize(clean);

    
    std::vector<std::string> filtered = removeStopWords(tokens);

    
    return stemTokens(filtered);
}









std::string Tokenizer::toLowercase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) -> char {
            return static_cast<char>(std::tolower(c));
        }
    );
    return result;
}











std::string Tokenizer::removePunctuation(const std::string& text) {
    std::string result = text;
    result.erase(
        std::remove_if(result.begin(), result.end(),
            [](unsigned char c) -> bool {
                
                return !std::isalnum(c) && !std::isspace(c);
            }
        ),
        result.end()
    );
    return result;
}















std::vector<std::string> Tokenizer::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream stream(text);
    std::string token;

    
    while (stream >> token) {
        
        
        if (!token.empty()) {
            tokens.push_back(std::move(token));
        }
    }

    return tokens;
}












std::vector<std::string> Tokenizer::removeStopWords(
    const std::vector<std::string>& tokens
) const {
    std::vector<std::string> filtered;
    
    
    
    filtered.reserve(tokens.size());

    for (const auto& token : tokens) {
        
        if (stopWords_.count(token) == 0) {
            filtered.push_back(token);
        }
    }

    return filtered;
}




size_t Tokenizer::getStopWordCount() const {
    return stopWords_.size();
}















std::vector<std::string> Tokenizer::stemTokens(
    const std::vector<std::string>& tokens
) {
    std::vector<std::string> stemmed;
    stemmed.reserve(tokens.size());

    for (const auto& token : tokens) {
        stemmed.push_back(stem(token));
    }

    return stemmed;
}















std::unordered_set<std::string> Tokenizer::buildStopWordSet() {
    return {
        
        "a", "an", "the",
        
        "i", "me", "my", "myself", "we", "our", "ours", "ourselves",
        "you", "your", "yours", "yourself", "yourselves",
        "he", "him", "his", "himself", "she", "her", "hers", "herself",
        "it", "its", "itself", "they", "them", "their", "theirs", "themselves",
        
        "in", "on", "at", "to", "for", "of", "with", "by", "from",
        "as", "into", "through", "during", "before", "after",
        "above", "below", "between", "under", "about",
        
        "is", "am", "are", "was", "were", "be", "been", "being",
        "have", "has", "had", "having",
        "do", "does", "did", "doing",
        "will", "would", "shall", "should", "may", "might",
        "can", "could", "must",
        
        "not", "no", "nor", "but", "and", "or", "so", "if", "then",
        "than", "too", "very", "just", "also",
        "that", "this", "these", "those", "which", "what", "who", "whom",
        "when", "where", "why", "how",
        "all", "each", "every", "both", "few", "more", "most",
        "other", "some", "such", "only", "own", "same",
        "here", "there", "again", "once",
        "up", "out", "off", "over", "down",
        
        "dont", "doesnt", "didnt", "wont", "wouldnt", "cant", "couldnt",
        "shouldnt", "mustnt", "isnt", "arent", "wasnt", "werent",
        "hasnt", "havent", "hadnt"
    };
}
