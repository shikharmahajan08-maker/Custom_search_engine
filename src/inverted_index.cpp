







#include "inverted_index.hpp"

#include <algorithm>  





const std::set<size_t> InvertedIndex::EMPTY_SET = {};
const std::vector<size_t> InvertedIndex::EMPTY_POSITIONS = {};




InvertedIndex::InvertedIndex()
    : totalDocuments_(0)
{}





















void InvertedIndex::addDocument(
    size_t docId,
    const std::vector<std::string>& tokens
) {
    
    
    
    docTokenCount_[docId] = tokens.size();

    
    for (size_t pos = 0; pos < tokens.size(); ++pos) {
        const auto& token = tokens[pos];

        
        
        
        postings_[token].insert(docId);

        
        
        
        termFrequency_[token][docId]++;

        
        
        positions_[token][docId].push_back(pos);
    }

    
    totalDocuments_++;
}












const std::set<size_t>& InvertedIndex::getPostings(const std::string& term) const {
    auto it = postings_.find(term);
    if (it != postings_.end()) {
        return it->second;
    }
    return EMPTY_SET;
}









size_t InvertedIndex::getTermFrequency(const std::string& term, size_t docId) const {
    
    auto termIt = termFrequency_.find(term);
    if (termIt == termFrequency_.end()) {
        return 0;
    }

    
    auto docIt = termIt->second.find(docId);
    if (docIt == termIt->second.end()) {
        return 0;
    }

    return docIt->second;
}









size_t InvertedIndex::getDocumentFrequency(const std::string& term) const {
    auto it = postings_.find(term);
    if (it != postings_.end()) {
        return it->second.size();
    }
    return 0;
}




size_t InvertedIndex::getDocTokenCount(size_t docId) const {
    auto it = docTokenCount_.find(docId);
    if (it != docTokenCount_.end()) {
        return it->second;
    }
    return 0;
}




size_t InvertedIndex::getTotalDocuments() const {
    return totalDocuments_;
}

size_t InvertedIndex::getVocabularySize() const {
    return postings_.size();
}

std::vector<std::string> InvertedIndex::getVocabulary() const {
    std::vector<std::string> vocab;
    vocab.reserve(postings_.size());
    for (const auto& entry : postings_) {
        vocab.push_back(entry.first);
    }
    return vocab;
}







size_t InvertedIndex::getTotalTokensIndexed() const {
    size_t total = 0;
    for (const auto& entry : docTokenCount_) {
        total += entry.second;
    }
    return total;
}







double InvertedIndex::getAverageDocLength() const {
    if (totalDocuments_ == 0) return 0.0;
    return static_cast<double>(getTotalTokensIndexed()) /
           static_cast<double>(totalDocuments_);
}




const std::vector<size_t>& InvertedIndex::getPositions(
    const std::string& term, size_t docId
) const {
    auto termIt = positions_.find(term);
    if (termIt == positions_.end()) {
        return EMPTY_POSITIONS;
    }
    auto docIt = termIt->second.find(docId);
    if (docIt == termIt->second.end()) {
        return EMPTY_POSITIONS;
    }
    return docIt->second;
}














std::set<size_t> InvertedIndex::phraseSearch(
    const std::vector<std::string>& phraseTerms
) const {
    std::set<size_t> result;

    if (phraseTerms.empty()) return result;
    if (phraseTerms.size() == 1) {
        
        return getPostings(phraseTerms[0]);
    }

    
    const auto& firstPostings = getPostings(phraseTerms[0]);

    for (size_t docId : firstPostings) {
        
        const auto& firstPositions = getPositions(phraseTerms[0], docId);

        
        for (size_t startPos : firstPositions) {
            bool match = true;

            for (size_t i = 1; i < phraseTerms.size(); ++i) {
                size_t expectedPos = startPos + i;
                const auto& positions = getPositions(phraseTerms[i], docId);

                
                bool found = std::binary_search(
                    positions.begin(), positions.end(), expectedPos
                );

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (match) {
                result.insert(docId);
                break;  
            }
        }
    }

    return result;
}
