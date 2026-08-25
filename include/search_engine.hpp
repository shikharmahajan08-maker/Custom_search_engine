




























#ifndef SEARCH_ENGINE_HPP
#define SEARCH_ENGINE_HPP

#include "document.hpp"
#include "inverted_index.hpp"
#include "tfidf_ranker.hpp"
#include "tokenizer.hpp"

#include <string>   
#include <vector>   







struct SearchResult {
    size_t      docId;     
    std::string title;     
    double      score;     
    std::string snippet;   
};




struct IndexStats {
    size_t totalDocuments;      
    size_t vocabularySize;      
    size_t totalTokensIndexed;  
    double avgDocLength;        
};








struct ParsedQuery {
    std::vector<std::string> terms;                       
    std::vector<std::vector<std::string>> phrases;        
    std::vector<std::string> allTerms;                    
    std::vector<std::string> originalTerms;               
};




class SearchEngine {
public:
    SearchEngine();

    
    void loadCorpus(const std::vector<Document>& documents);

    
    std::vector<SearchResult> search(const std::string& query) const;

    
    IndexStats getStats() const;

    
    
    std::vector<ScoreBreakdown> explainScore(
        const std::string& query, size_t docId
    ) const;

    
    const Document* getDocument(size_t docId) const;

    
    size_t getDocumentCount() const;

    
    const InvertedIndex& getIndex() const;

    
    const Tokenizer& getTokenizer() const;

    
    const BM25Ranker& getRanker() const;

    
    
    ParsedQuery parseQuery(const std::string& query) const;

private:
    std::vector<Document> documents_;   
    Tokenizer             tokenizer_;   
    InvertedIndex         index_;       
    BM25Ranker            ranker_;      

    
    
    
    
    
    
    
    
    
    
    std::string generateKWICSnippet(
        const Document& doc,
        const std::vector<std::string>& queryTerms,
        size_t maxLength = 200
    ) const;
};

#endif 
