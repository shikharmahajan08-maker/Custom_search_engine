



































#ifndef TFIDF_RANKER_HPP
#define TFIDF_RANKER_HPP

#include "inverted_index.hpp"

#include <cstddef>   
#include <string>    
#include <utility>   
#include <vector>    




struct RankedResult {
    size_t docId;   
    double score;   

    RankedResult(size_t id, double s);
};







struct ScoreBreakdown {
    std::string term;     
    double tf;            
    double idf;           
    double termScore;     
    size_t docFreq;       
};








class BM25Ranker {
public:
    
    
    
    BM25Ranker(double k1 = 1.2, double b = 0.75);

    
    
    
    
    
    std::vector<RankedResult> rank(
        const std::vector<std::string>& queryTerms,
        const InvertedIndex& index
    ) const;

    
    std::vector<RankedResult> rankTFIDF(
        const std::vector<std::string>& queryTerms,
        const InvertedIndex& index
    ) const;

    
    
    
    
    
    
    
    std::vector<ScoreBreakdown> explain(
        const std::vector<std::string>& queryTerms,
        size_t docId,
        const InvertedIndex& index
    ) const;

    
    double getK1() const { return k1_; }

    
    double getB() const { return b_; }

private:
    double k1_;  
    double b_;   

    
    double computeBM25TermScore(
        const std::string& term,
        size_t docId,
        const InvertedIndex& index,
        double avgdl
    ) const;

    
    double computeSmoothedIDF(
        const std::string& term,
        const InvertedIndex& index
    ) const;

    
    double computeTF(
        const std::string& term,
        size_t docId,
        const InvertedIndex& index
    ) const;

    
    double computeIDF(
        const std::string& term,
        const InvertedIndex& index
    ) const;
};


using TFIDFRanker = BM25Ranker;

#endif 
