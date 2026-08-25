








































#ifndef INVERTED_INDEX_HPP
#define INVERTED_INDEX_HPP

#include <cstddef>         
#include <set>             
#include <string>          
#include <unordered_map>   
#include <vector>          




class InvertedIndex {
public:
    InvertedIndex();

    
    
    

    
    
    
    
    
    
    
    
    
    
    
    
    void addDocument(size_t docId, const std::vector<std::string>& tokens);

    
    
    

    
    
    
    
    
    
    
    const std::set<size_t>& getPostings(const std::string& term) const;

    
    
    
    
    size_t getTermFrequency(const std::string& term, size_t docId) const;

    
    
    
    
    size_t getDocumentFrequency(const std::string& term) const;

    
    
    
    
    size_t getDocTokenCount(size_t docId) const;

    
    size_t getTotalDocuments() const;

    
    size_t getVocabularySize() const;

    
    
    std::vector<std::string> getVocabulary() const;

    
    
    size_t getTotalTokensIndexed() const;

    
    
    
    double getAverageDocLength() const;

    
    
    
    
    
    
    
    const std::vector<size_t>& getPositions(
        const std::string& term, size_t docId
    ) const;

    
    
    
    
    
    
    
    
    std::set<size_t> phraseSearch(
        const std::vector<std::string>& phraseTerms
    ) const;

private:
    
    
    

    
    std::unordered_map<std::string, std::set<size_t>> postings_;

    
    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> termFrequency_;

    
    std::unordered_map<size_t, size_t> docTokenCount_;

    
    size_t totalDocuments_;

    
    
    static const std::set<size_t> EMPTY_SET;

    
    
    
    std::unordered_map<std::string,
        std::unordered_map<size_t, std::vector<size_t>>> positions_;

    
    static const std::vector<size_t> EMPTY_POSITIONS;
};

#endif 
