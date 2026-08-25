










#include "search_engine.hpp"
#include "stemmer.hpp"

#include <algorithm>       
#include <iostream>        
#include <sstream>         
#include <unordered_set>   


namespace SnippetColor {
    const std::string BOLD_YELLOW = "\033[1;33m";
    const std::string RESET       = "\033[0m";
}




SearchEngine::SearchEngine()
    : documents_()
    , tokenizer_()
    , index_()
    , ranker_()
{}




void SearchEngine::loadCorpus(const std::vector<Document>& documents) {
    documents_ = documents;

    for (const auto& doc : documents_) {
        std::vector<std::string> tokens = tokenizer_.process(doc.content);
        index_.addDocument(doc.id, tokens);
    }
}











ParsedQuery SearchEngine::parseQuery(const std::string& query) const {
    ParsedQuery parsed;

    std::string remaining = query;
    std::string nonPhraseText;

    
    size_t pos = 0;
    while (pos < remaining.size()) {
        size_t quoteStart = remaining.find('"', pos);
        if (quoteStart == std::string::npos) {
            
            nonPhraseText += remaining.substr(pos);
            break;
        }

        
        nonPhraseText += remaining.substr(pos, quoteStart - pos);

        
        size_t quoteEnd = remaining.find('"', quoteStart + 1);
        if (quoteEnd == std::string::npos) {
            
            nonPhraseText += remaining.substr(quoteStart + 1);
            break;
        }

        
        std::string phraseText = remaining.substr(
            quoteStart + 1, quoteEnd - quoteStart - 1
        );

        
        std::vector<std::string> phraseTokens = tokenizer_.process(phraseText);
        if (!phraseTokens.empty()) {
            parsed.phrases.push_back(phraseTokens);
            
            for (const auto& t : phraseTokens) {
                parsed.allTerms.push_back(t);
            }
        }

        pos = quoteEnd + 1;
    }

    
    if (!nonPhraseText.empty()) {
        
        std::string lower = Tokenizer::toLowercase(nonPhraseText);
        std::string clean = Tokenizer::removePunctuation(lower);
        std::vector<std::string> rawTokens = Tokenizer::tokenize(clean);
        for (const auto& t : rawTokens) {
            parsed.originalTerms.push_back(t);
        }

        std::vector<std::string> tokens = tokenizer_.process(nonPhraseText);
        parsed.terms = tokens;
        for (const auto& t : tokens) {
            parsed.allTerms.push_back(t);
        }
    }

    return parsed;
}











std::vector<SearchResult> SearchEngine::search(const std::string& query) const {
    ParsedQuery parsed = parseQuery(query);

    if (parsed.allTerms.empty()) return {};

    
    std::vector<RankedResult> rankedResults = ranker_.rank(
        parsed.allTerms, index_
    );

    
    if (!parsed.phrases.empty()) {
        
        std::unordered_set<size_t> phraseMatchDocs;
        for (const auto& phrase : parsed.phrases) {
            auto matches = index_.phraseSearch(phrase);
            phraseMatchDocs.insert(matches.begin(), matches.end());
        }

        
        for (auto& result : rankedResults) {
            if (phraseMatchDocs.count(result.docId) > 0) {
                result.score *= 2.0;
            }
        }

        
        std::sort(rankedResults.begin(), rankedResults.end(),
            [](const RankedResult& a, const RankedResult& b) -> bool {
                if (a.score != b.score) return a.score > b.score;
                return a.docId < b.docId;
            }
        );
    }

    
    std::vector<SearchResult> results;
    results.reserve(rankedResults.size());

    for (const auto& ranked : rankedResults) {
        if (ranked.docId < documents_.size()) {
            const Document& doc = documents_[ranked.docId];

            SearchResult result;
            result.docId   = ranked.docId;
            result.title   = doc.title;
            result.score   = ranked.score;
            result.snippet = generateKWICSnippet(doc, parsed.allTerms);

            results.push_back(std::move(result));
        }
    }

    return results;
}




IndexStats SearchEngine::getStats() const {
    IndexStats stats;
    stats.totalDocuments     = index_.getTotalDocuments();
    stats.vocabularySize     = index_.getVocabularySize();
    stats.totalTokensIndexed = index_.getTotalTokensIndexed();
    stats.avgDocLength       = index_.getAverageDocLength();
    return stats;
}




std::vector<ScoreBreakdown> SearchEngine::explainScore(
    const std::string& query, size_t docId
) const {
    ParsedQuery parsed = parseQuery(query);
    return ranker_.explain(parsed.allTerms, docId, index_);
}




const Document* SearchEngine::getDocument(size_t docId) const {
    if (docId < documents_.size()) return &documents_[docId];
    return nullptr;
}

size_t SearchEngine::getDocumentCount() const {
    return documents_.size();
}

const InvertedIndex& SearchEngine::getIndex() const {
    return index_;
}

const Tokenizer& SearchEngine::getTokenizer() const {
    return tokenizer_;
}

const BM25Ranker& SearchEngine::getRanker() const {
    return ranker_;
}
















std::string SearchEngine::generateKWICSnippet(
    const Document& doc,
    const std::vector<std::string>& queryTerms,
    size_t maxLength
) const {
    if (doc.content.empty() || queryTerms.empty()) {
        
        if (doc.content.length() <= maxLength) return doc.content;
        size_t cutoff = maxLength;
        while (cutoff > 0 && doc.content[cutoff] != ' ') cutoff--;
        if (cutoff == 0) cutoff = maxLength;
        return doc.content.substr(0, cutoff) + "...";
    }

    
    std::unordered_set<std::string> querySet(queryTerms.begin(), queryTerms.end());

    
    struct WordInfo {
        std::string original;   
        std::string stemmed;    
        size_t startPos;        
        size_t endPos;          
    };

    std::vector<WordInfo> words;
    size_t i = 0;
    while (i < doc.content.size()) {
        
        while (i < doc.content.size() && std::isspace(static_cast<unsigned char>(doc.content[i]))) i++;
        if (i >= doc.content.size()) break;

        size_t wordStart = i;
        while (i < doc.content.size() && !std::isspace(static_cast<unsigned char>(doc.content[i]))) i++;

        std::string word = doc.content.substr(wordStart, i - wordStart);

        
        std::string cleaned;
        for (char c : word) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                cleaned += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }

        std::string stemmed = cleaned.empty() ? "" : stem(cleaned);

        words.push_back({word, stemmed, wordStart, i});
    }

    if (words.empty()) return doc.content.substr(0, std::min(maxLength, doc.content.size()));

    
    
    size_t windowSize = 0;
    {
        
        windowSize = maxLength / 6;
        if (windowSize > words.size()) windowSize = words.size();
        if (windowSize < 5) windowSize = std::min<size_t>(5, words.size());
    }

    int bestScore = -1;
    size_t bestStart = 0;

    for (size_t start = 0; start + windowSize <= words.size(); ++start) {
        int score = 0;
        for (size_t j = start; j < start + windowSize; ++j) {
            if (querySet.count(words[j].stemmed) > 0) {
                score++;
            }
        }
        if (score > bestScore) {
            bestScore = score;
            bestStart = start;
        }
    }

    
    std::string snippet;
    if (bestStart > 0) snippet += "...";

    for (size_t j = bestStart; j < bestStart + windowSize && j < words.size(); ++j) {
        if (j > bestStart) snippet += " ";

        if (querySet.count(words[j].stemmed) > 0) {
            
            snippet += SnippetColor::BOLD_YELLOW + words[j].original + SnippetColor::RESET;
        } else {
            snippet += words[j].original;
        }
    }

    if (bestStart + windowSize < words.size()) snippet += "...";

    return snippet;
}
