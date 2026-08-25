














#include "tfidf_ranker.hpp"

#include <algorithm>      
#include <cmath>          
#include <set>            
#include <unordered_map>  




RankedResult::RankedResult(size_t id, double s)
    : docId(id)
    , score(s)
{}




BM25Ranker::BM25Ranker(double k1, double b)
    : k1_(k1)
    , b_(b)
{}












std::vector<RankedResult> BM25Ranker::rank(
    const std::vector<std::string>& queryTerms,
    const InvertedIndex& index
) const {
    if (queryTerms.empty()) return {};

    
    double avgdl = index.getAverageDocLength();

    
    std::set<size_t> candidateDocs;
    for (const auto& term : queryTerms) {
        const auto& postings = index.getPostings(term);
        candidateDocs.insert(postings.begin(), postings.end());
    }

    if (candidateDocs.empty()) return {};

    
    std::vector<RankedResult> results;
    results.reserve(candidateDocs.size());

    for (size_t docId : candidateDocs) {
        double score = 0.0;

        for (const auto& term : queryTerms) {
            score += computeBM25TermScore(term, docId, index, avgdl);
        }

        if (score > 0.0) {
            results.emplace_back(docId, score);
        }
    }

    
    std::sort(results.begin(), results.end(),
        [](const RankedResult& a, const RankedResult& b) -> bool {
            if (a.score != b.score) return a.score > b.score;
            return a.docId < b.docId;
        }
    );

    return results;
}




std::vector<RankedResult> BM25Ranker::rankTFIDF(
    const std::vector<std::string>& queryTerms,
    const InvertedIndex& index
) const {
    if (queryTerms.empty()) return {};

    std::set<size_t> candidateDocs;
    for (const auto& term : queryTerms) {
        const auto& postings = index.getPostings(term);
        candidateDocs.insert(postings.begin(), postings.end());
    }

    if (candidateDocs.empty()) return {};

    std::vector<RankedResult> results;
    results.reserve(candidateDocs.size());

    for (size_t docId : candidateDocs) {
        double score = 0.0;
        for (const auto& term : queryTerms) {
            score += computeTF(term, docId, index) * computeIDF(term, index);
        }
        if (score > 0.0) {
            results.emplace_back(docId, score);
        }
    }

    std::sort(results.begin(), results.end(),
        [](const RankedResult& a, const RankedResult& b) -> bool {
            if (a.score != b.score) return a.score > b.score;
            return a.docId < b.docId;
        }
    );

    return results;
}







std::vector<ScoreBreakdown> BM25Ranker::explain(
    const std::vector<std::string>& queryTerms,
    size_t docId,
    const InvertedIndex& index
) const {
    std::vector<ScoreBreakdown> breakdown;
    double avgdl = index.getAverageDocLength();

    for (const auto& term : queryTerms) {
        ScoreBreakdown entry;
        entry.term = term;
        entry.tf = static_cast<double>(index.getTermFrequency(term, docId));
        entry.idf = computeSmoothedIDF(term, index);
        entry.termScore = computeBM25TermScore(term, docId, index, avgdl);
        entry.docFreq = index.getDocumentFrequency(term);
        breakdown.push_back(std::move(entry));
    }

    return breakdown;
}

















double BM25Ranker::computeBM25TermScore(
    const std::string& term,
    size_t docId,
    const InvertedIndex& index,
    double avgdl
) const {
    double f = static_cast<double>(index.getTermFrequency(term, docId));
    if (f == 0.0) return 0.0;

    double dl = static_cast<double>(index.getDocTokenCount(docId));
    double idf = computeSmoothedIDF(term, index);

    
    double numerator = f * (k1_ + 1.0);
    double denominator = f + k1_ * (1.0 - b_ + b_ * (dl / avgdl));

    return idf * (numerator / denominator);
}












double BM25Ranker::computeSmoothedIDF(
    const std::string& term,
    const InvertedIndex& index
) const {
    size_t docFreq = index.getDocumentFrequency(term);
    if (docFreq == 0) return 0.0;

    double N = static_cast<double>(index.getTotalDocuments());
    double df = static_cast<double>(docFreq);

    return std::log((N - df + 0.5) / (df + 0.5) + 1.0);
}




double BM25Ranker::computeTF(
    const std::string& term,
    size_t docId,
    const InvertedIndex& index
) const {
    size_t rawCount = index.getTermFrequency(term, docId);
    size_t totalTokens = index.getDocTokenCount(docId);
    if (totalTokens == 0) return 0.0;
    return static_cast<double>(rawCount) / static_cast<double>(totalTokens);
}




double BM25Ranker::computeIDF(
    const std::string& term,
    const InvertedIndex& index
) const {
    size_t docFreq = index.getDocumentFrequency(term);
    if (docFreq == 0) return 0.0;

    double N = static_cast<double>(index.getTotalDocuments());
    double df = static_cast<double>(docFreq);

    return std::log10(N / df);
}
