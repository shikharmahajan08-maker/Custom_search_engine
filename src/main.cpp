


















#include "search_engine.hpp"
#include "document.hpp"
#include "stemmer.hpp"

#include <algorithm>   
#include <cctype>      
#include <cmath>       
#include <iomanip>     
#include <iostream>    
#include <string>      
#include <vector>      
#include <chrono>      
#include <unordered_map>  




namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    const std::string CYAN    = "\033[36m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string MAGENTA = "\033[35m";
    const std::string WHITE   = "\033[37m";
    const std::string BLUE    = "\033[34m";
    const std::string RED     = "\033[31m";
}




void printBanner(const IndexStats& stats) {
    std::cout << "\n";
    std::cout << Color::CYAN << Color::BOLD;
    std::cout << "  ╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ║       ██████╗██╗   ██╗███████╗████████╗ ██████╗ ███╗   ███╗  ║\n";
    std::cout << "  ║      ██╔════╝██║   ██║██╔════╝╚══██╔══╝██╔═══██╗████╗ ████║  ║\n";
    std::cout << "  ║      ██║     ██║   ██║███████╗   ██║   ██║   ██║██╔████╔██║  ║\n";
    std::cout << "  ║      ██║     ██║   ██║╚════██║   ██║   ██║   ██║██║╚██╔╝██║  ║\n";
    std::cout << "  ║      ╚██████╗╚██████╔╝███████║   ██║   ╚██████╔╝██║ ╚═╝ ██║  ║\n";
    std::cout << "  ║       ╚═════╝ ╚═════╝ ╚══════╝   ╚═╝    ╚═════╝ ╚═╝     ╚═╝  ║\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ║        ███████╗███████╗ █████╗ ██████╗  ██████╗██╗  ██╗      ║\n";
    std::cout << "  ║        ██╔════╝██╔════╝██╔══██╗██╔══██╗██╔════╝██║  ██║      ║\n";
    std::cout << "  ║        ███████╗█████╗  ███████║██████╔╝██║     ███████║      ║\n";
    std::cout << "  ║        ╚════██║██╔══╝  ██╔══██║██╔══██╗██║     ██╔══██║      ║\n";
    std::cout << "  ║        ███████║███████╗██║  ██║██║  ██║╚██████╗██║  ██║      ║\n";
    std::cout << "  ║        ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝      ║\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ║        " << Color::YELLOW << "BM25-Powered Search Engine" << Color::CYAN << Color::BOLD << "                        ║\n";
    std::cout << "  ║              " << Color::DIM << Color::WHITE << "C++17  •  BM25  •  Porter Stemmer  •  Phrase Search" << Color::RESET << Color::CYAN << Color::BOLD << " ║\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << Color::RESET << "\n";

    
    std::cout << Color::GREEN << "  ✓ " << Color::WHITE << "Engine initialized successfully\n";
    std::cout << Color::GREEN << "  ✓ " << Color::WHITE
              << "Indexed " << Color::BOLD << stats.totalDocuments << Color::RESET
              << Color::WHITE << " documents  │  "
              << Color::BOLD << stats.vocabularySize << Color::RESET
              << Color::WHITE << " unique terms  │  "
              << Color::BOLD << stats.totalTokensIndexed << Color::RESET
              << Color::WHITE << " total tokens\n" << Color::RESET;
    std::cout << Color::GREEN << "  ✓ " << Color::WHITE
              << "Avg doc length: " << Color::BOLD << std::fixed << std::setprecision(1)
              << stats.avgDocLength << Color::RESET << Color::WHITE
              << " tokens  │  Ranking: " << Color::BOLD << "BM25"
              << Color::RESET << Color::WHITE
              << " (k1=1.2, b=0.75)\n" << Color::RESET;

    std::cout << "\n";
    std::cout << Color::DIM << "  Type a search query and press Enter. Use \"quotes\" for phrase search.\n"
              << "  Commands: "
              << Color::YELLOW << ":help" << Color::DIM << " | "
              << Color::YELLOW << ":stats" << Color::DIM << " | "
              << Color::YELLOW << ":explain" << Color::DIM << " | "
              << Color::YELLOW << ":top" << Color::DIM << " | "
              << Color::YELLOW << ":similar" << Color::DIM << " | "
              << Color::YELLOW << ":quit" << Color::RESET << "\n";
    std::cout << Color::DIM << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;
}




void printResults(
    const std::vector<SearchResult>& results,
    double elapsedMs,
    const ParsedQuery& parsed
) {
    if (results.empty()) {
        std::cout << "\n" << Color::YELLOW << "  ⚠  No matching documents found."
                  << Color::RESET << "\n\n";
        return;
    }

    
    if (!parsed.allTerms.empty()) {
        std::cout << "\n" << Color::DIM << "  Searched for: ";
        for (size_t i = 0; i < parsed.allTerms.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << Color::CYAN << parsed.allTerms[i] << Color::DIM;
        }
        if (!parsed.phrases.empty()) {
            std::cout << "  (phrase search active)";
        }
        std::cout << Color::RESET << "\n";
    }

    
    std::cout << Color::GREEN << Color::BOLD
              << "  Found " << results.size() << " result(s)"
              << Color::RESET << Color::DIM
              << "  (" << std::fixed << std::setprecision(3)
              << elapsedMs << " ms)" << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;

    
    int rank = 1;
    for (const auto& result : results) {
        std::string rankColor;
        if (rank == 1) rankColor = Color::YELLOW;
        else if (rank == 2) rankColor = Color::WHITE;
        else if (rank == 3) rankColor = Color::MAGENTA;
        else rankColor = Color::DIM;

        std::cout << "\n";

        
        std::cout << "  " << rankColor << Color::BOLD
                  << "  #" << rank << Color::RESET << "  "
                  << Color::CYAN << Color::BOLD << result.title
                  << Color::RESET << "\n";

        
        std::cout << "       " << Color::DIM << "Doc ID: " << result.docId
                  << "  │  BM25 Score: " << Color::RESET
                  << Color::GREEN << std::fixed << std::setprecision(6)
                  << result.score << Color::RESET << "\n";

        
        std::cout << "       " << Color::DIM << result.snippet
                  << Color::RESET << "\n";

        rank++;
    }

    std::cout << "\n" << Color::DIM
              << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;
}




void printHelp() {
    std::cout << "\n" << Color::CYAN << Color::BOLD
              << "  Custom Search Engine — Help" << Color::RESET << "\n\n";

    std::cout << Color::WHITE << "  Usage:" << Color::RESET << "\n";
    std::cout << "    Type any search query and press Enter to find relevant documents.\n";
    std::cout << "    Multi-word queries search for documents containing ANY of the terms.\n";
    std::cout << "    Wrap terms in \"quotes\" for exact phrase matching.\n";
    std::cout << "    Results are ranked by BM25 relevance score (highest first).\n";
    std::cout << "    The Porter Stemmer matches word variants automatically.\n\n";

    std::cout << Color::WHITE << "  Examples:" << Color::RESET << "\n";
    std::cout << Color::GREEN << "    > " << Color::RESET << "machine learning neural networks\n";
    std::cout << Color::GREEN << "    > " << Color::RESET << "\"quantum computing\" algorithms\n";
    std::cout << Color::GREEN << "    > " << Color::RESET << "\"deep learning\" optimization\n";
    std::cout << Color::GREEN << "    > " << Color::RESET << "cryptography security\n\n";

    std::cout << Color::WHITE << "  Commands:" << Color::RESET << "\n";
    std::cout << "    " << Color::YELLOW << ":help"         << Color::RESET << "              — Show this help message\n";
    std::cout << "    " << Color::YELLOW << ":stats"        << Color::RESET << "             — Display index statistics\n";
    std::cout << "    " << Color::YELLOW << ":explain <q>"  << Color::RESET << "        — Show BM25 scoring breakdown\n";
    std::cout << "    " << Color::YELLOW << ":top <term>"   << Color::RESET << "         — Top documents for a single term\n";
    std::cout << "    " << Color::YELLOW << ":similar <id>" << Color::RESET << "       — Find documents similar to doc #id\n";
    std::cout << "    " << Color::YELLOW << ":quit"         << Color::RESET << "              — Exit the search engine\n\n";

    std::cout << Color::WHITE << "  Algorithm (BM25):" << Color::RESET << "\n";
    std::cout << "    IDF(t)      = log((N - df + 0.5) / (df + 0.5) + 1)\n";
    std::cout << "    Score(t, d) = IDF(t) × [f(t,d) × (k1+1)] / [f(t,d) + k1 × (1-b+b×|d|/avgdl)]\n";
    std::cout << "    Score(q, d) = Sigma Score(t, d) for each term t in query q\n\n";
}




void printStats(const IndexStats& stats) {
    std::cout << "\n" << Color::CYAN << Color::BOLD
              << "  Index Statistics" << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;
    std::cout << "    Documents indexed:     " << Color::BOLD << stats.totalDocuments
              << Color::RESET << "\n";
    std::cout << "    Unique terms (vocab):  " << Color::BOLD << stats.vocabularySize
              << Color::RESET << "\n";
    std::cout << "    Total tokens indexed:  " << Color::BOLD << stats.totalTokensIndexed
              << Color::RESET << "\n";
    std::cout << "    Avg tokens/document:   " << Color::BOLD << std::fixed << std::setprecision(1)
              << stats.avgDocLength << Color::RESET << "\n";
    std::cout << "    Ranking algorithm:     " << Color::BOLD << "BM25"
              << Color::RESET << " (k1=1.2, b=0.75)\n";
    std::cout << "    NLP pipeline:          " << Color::BOLD
              << "lowercase → depunct → tokenize → stopwords → stem"
              << Color::RESET << "\n\n";
}




void handleExplain(const SearchEngine& engine, const std::string& queryStr) {
    if (queryStr.empty()) {
        std::cout << Color::YELLOW << "  Usage: :explain <query>\n" << Color::RESET;
        return;
    }

    
    auto results = engine.search(queryStr);

    if (results.empty()) {
        std::cout << Color::YELLOW << "  No results to explain.\n" << Color::RESET;
        return;
    }

    
    const auto& topResult = results[0];
    auto breakdown = engine.explainScore(queryStr, topResult.docId);

    std::cout << "\n" << Color::CYAN << Color::BOLD
              << "  BM25 Score Breakdown — \"" << topResult.title << "\""
              << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;

    
    std::cout << "    " << Color::BOLD
              << std::left << std::setw(20) << "Term"
              << std::right << std::setw(8) << "TF"
              << std::setw(10) << "IDF"
              << std::setw(10) << "df(t)"
              << std::setw(14) << "BM25 Score"
              << Color::RESET << "\n";
    std::cout << "    " << Color::DIM
              << "────────────────────────────────────────────────────────────"
              << Color::RESET << "\n";

    double totalScore = 0.0;
    for (const auto& entry : breakdown) {
        std::string termDisplay = entry.term;
        if (termDisplay.length() > 18) termDisplay = termDisplay.substr(0, 18) + "..";

        std::cout << "    "
                  << Color::CYAN << std::left << std::setw(20) << termDisplay << Color::RESET
                  << std::right << std::setw(8) << std::fixed << std::setprecision(0) << entry.tf
                  << std::setw(10) << std::setprecision(4) << entry.idf
                  << std::setw(10) << entry.docFreq;

        if (entry.termScore > 0) {
            std::cout << Color::GREEN << std::setw(14) << std::setprecision(6) << entry.termScore
                      << Color::RESET;
        } else {
            std::cout << Color::DIM << std::setw(14) << "0.000000" << Color::RESET;
        }
        std::cout << "\n";
        totalScore += entry.termScore;
    }

    std::cout << "    " << Color::DIM
              << "────────────────────────────────────────────────────────────"
              << Color::RESET << "\n";
    std::cout << "    " << Color::BOLD
              << std::left << std::setw(48) << "Total BM25 Score:"
              << Color::GREEN << std::right << std::setprecision(6) << totalScore
              << Color::RESET << "\n\n";
}




void handleTop(const SearchEngine& engine, const std::string& termStr) {
    if (termStr.empty()) {
        std::cout << Color::YELLOW << "  Usage: :top <term>\n" << Color::RESET;
        return;
    }

    
    std::string stemmed = stem(termStr);
    std::transform(stemmed.begin(), stemmed.end(), stemmed.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    const auto& index = engine.getIndex();
    size_t df = index.getDocumentFrequency(stemmed);

    std::cout << "\n" << Color::CYAN << Color::BOLD
              << "  Top Documents for: \"" << termStr << "\" (stemmed: \"" << stemmed << "\")"
              << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;
    std::cout << "    Document frequency: " << Color::BOLD << df << Color::RESET
              << " / " << index.getTotalDocuments() << " documents\n\n";

    if (df == 0) {
        std::cout << Color::YELLOW << "    Term not found in any document.\n" << Color::RESET << "\n";
        return;
    }

    
    const auto& postings = index.getPostings(stemmed);
    struct TermDocInfo {
        size_t docId;
        size_t tf;
        double tfNorm;
    };

    std::vector<TermDocInfo> docs;
    for (size_t docId : postings) {
        size_t tf = index.getTermFrequency(stemmed, docId);
        size_t docLen = index.getDocTokenCount(docId);
        double tfNorm = docLen > 0 ? static_cast<double>(tf) / static_cast<double>(docLen) : 0.0;
        docs.push_back({docId, tf, tfNorm});
    }

    std::sort(docs.begin(), docs.end(), [](const TermDocInfo& a, const TermDocInfo& b) {
        return a.tf > b.tf;
    });

    
    std::cout << "    " << Color::BOLD
              << std::left << std::setw(6) << "ID"
              << std::setw(30) << "Title"
              << std::right << std::setw(8) << "TF"
              << std::setw(12) << "TF (norm)"
              << Color::RESET << "\n";
    std::cout << "    " << Color::DIM << "──────────────────────────────────────────────────────\n"
              << Color::RESET;

    for (const auto& d : docs) {
        const Document* doc = engine.getDocument(d.docId);
        std::string title = doc ? doc->title : "Unknown";
        if (title.length() > 28) title = title.substr(0, 28) + "..";

        std::cout << "    "
                  << Color::DIM << std::left << std::setw(6) << d.docId << Color::RESET
                  << Color::CYAN << std::setw(30) << title << Color::RESET
                  << std::right << std::setw(8) << d.tf
                  << Color::GREEN << std::setw(12) << std::fixed << std::setprecision(6) << d.tfNorm
                  << Color::RESET << "\n";
    }
    std::cout << "\n";
}




void handleSimilar(const SearchEngine& engine, const std::string& idStr) {
    if (idStr.empty()) {
        std::cout << Color::YELLOW << "  Usage: :similar <docId>\n" << Color::RESET;
        return;
    }

    size_t targetId;
    try {
        targetId = std::stoul(idStr);
    } catch (...) {
        std::cout << Color::RED << "  Invalid document ID.\n" << Color::RESET;
        return;
    }

    const Document* targetDoc = engine.getDocument(targetId);
    if (!targetDoc) {
        std::cout << Color::RED << "  Document ID " << targetId << " not found.\n" << Color::RESET;
        return;
    }

    const auto& index = engine.getIndex();
    size_t numDocs = engine.getDocumentCount();

    
    
    auto vocab = index.getVocabulary();

    
    std::unordered_map<std::string, double> targetVector;
    double targetNorm = 0.0;

    for (const auto& term : vocab) {
        size_t tf = index.getTermFrequency(term, targetId);
        if (tf == 0) continue;

        size_t docLen = index.getDocTokenCount(targetId);
        double tfNorm = docLen > 0 ? static_cast<double>(tf) / static_cast<double>(docLen) : 0.0;
        size_t df = index.getDocumentFrequency(term);
        double idf = df > 0 ? std::log10(static_cast<double>(numDocs) / static_cast<double>(df)) : 0.0;

        double weight = tfNorm * idf;
        targetVector[term] = weight;
        targetNorm += weight * weight;
    }
    targetNorm = std::sqrt(targetNorm);

    if (targetNorm == 0.0) {
        std::cout << Color::YELLOW << "  Document has no indexable terms.\n" << Color::RESET;
        return;
    }

    
    struct SimResult {
        size_t docId;
        double similarity;
    };
    std::vector<SimResult> similarities;

    for (size_t docId = 0; docId < numDocs; ++docId) {
        if (docId == targetId) continue;

        double dotProduct = 0.0;
        double otherNorm = 0.0;

        for (const auto& entry : targetVector) {
            const std::string& tvTerm = entry.first;
            double targetWeight = entry.second;
            size_t otherTf = index.getTermFrequency(tvTerm, docId);
            if (otherTf == 0) continue;

            size_t otherDocLen = index.getDocTokenCount(docId);
            double otherTfNorm = otherDocLen > 0
                ? static_cast<double>(otherTf) / static_cast<double>(otherDocLen) : 0.0;
            size_t df = index.getDocumentFrequency(tvTerm);
            double idf = df > 0
                ? std::log10(static_cast<double>(numDocs) / static_cast<double>(df)) : 0.0;

            double otherWeight = otherTfNorm * idf;
            dotProduct += targetWeight * otherWeight;
        }

        
        for (const auto& term : vocab) {
            size_t otherTf = index.getTermFrequency(term, docId);
            if (otherTf == 0) continue;

            size_t otherDocLen = index.getDocTokenCount(docId);
            double otherTfNorm = otherDocLen > 0
                ? static_cast<double>(otherTf) / static_cast<double>(otherDocLen) : 0.0;
            size_t df = index.getDocumentFrequency(term);
            double idf = df > 0
                ? std::log10(static_cast<double>(numDocs) / static_cast<double>(df)) : 0.0;

            double w = otherTfNorm * idf;
            otherNorm += w * w;
        }
        otherNorm = std::sqrt(otherNorm);

        double cosine = (targetNorm > 0.0 && otherNorm > 0.0)
            ? dotProduct / (targetNorm * otherNorm) : 0.0;

        if (cosine > 0.0) {
            similarities.push_back({docId, cosine});
        }
    }

    std::sort(similarities.begin(), similarities.end(),
        [](const SimResult& a, const SimResult& b) {
            return a.similarity > b.similarity;
        }
    );

    std::cout << "\n" << Color::CYAN << Color::BOLD
              << "  Documents Similar to: \"" << targetDoc->title << "\" (ID: " << targetId << ")"
              << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────────────────────\n"
              << Color::RESET;

    if (similarities.empty()) {
        std::cout << Color::YELLOW << "    No similar documents found.\n" << Color::RESET << "\n";
        return;
    }

    
    std::cout << "    " << Color::BOLD
              << std::left << std::setw(6) << "ID"
              << std::setw(35) << "Title"
              << std::right << std::setw(15) << "Cosine Sim"
              << Color::RESET << "\n";
    std::cout << "    " << Color::DIM << "──────────────────────────────────────────────────────\n"
              << Color::RESET;

    size_t shown = 0;
    for (const auto& sim : similarities) {
        if (shown >= 10) break;  

        const Document* doc = engine.getDocument(sim.docId);
        std::string title = doc ? doc->title : "Unknown";
        if (title.length() > 33) title = title.substr(0, 33) + "..";

        std::string barStr;
        int barLen = static_cast<int>(sim.similarity * 20);
        for (int i = 0; i < barLen; i++) barStr += "█";

        std::cout << "    "
                  << Color::DIM << std::left << std::setw(6) << sim.docId << Color::RESET
                  << Color::CYAN << std::setw(35) << title << Color::RESET
                  << Color::GREEN << std::right << std::setw(8) << std::fixed << std::setprecision(4) << sim.similarity
                  << " " << Color::YELLOW << barStr
                  << Color::RESET << "\n";
        shown++;
    }
    std::cout << "\n";
}




int main(int argc, char* argv[]) {
    
    
    
    
    
    
    std::string corpusDir = "corpus";
    if (argc > 1) {
        corpusDir = argv[1];
    }

    std::cout << "\n" << Color::CYAN << Color::BOLD
              << "  Loading corpus from: " << Color::RESET
              << Color::WHITE << corpusDir << "/" << Color::RESET << "\n\n";

    SearchEngine engine;

    std::vector<Document> corpus = Corpus::loadFromDirectory(corpusDir);

    if (corpus.empty()) {
        std::cout << "\n" << Color::RED << Color::BOLD
                  << "  ✗ No documents found in corpus/ directory." << Color::RESET << "\n";
        std::cout << Color::YELLOW
                  << "    Please run the Python scraper first:\n"
                  << "      cd scraper\n"
                  << "      pip install -r requirements.txt\n"
                  << "      python scraper.py\n"
                  << Color::RESET << "\n";
        return 1;
    }

    
    auto indexStart = std::chrono::high_resolution_clock::now();
    engine.loadCorpus(corpus);
    auto indexEnd = std::chrono::high_resolution_clock::now();
    double indexMs = std::chrono::duration<double, std::milli>(indexEnd - indexStart).count();

    IndexStats stats = engine.getStats();
    printBanner(stats);

    std::cout << Color::GREEN << "  ✓ " << Color::DIM
              << "Indexing completed in " << std::fixed << std::setprecision(1)
              << indexMs << " ms\n" << Color::RESET;

    
    
    
    std::string query;
    while (true) {
        std::cout << Color::GREEN << Color::BOLD << "  > " << Color::RESET;

        if (!std::getline(std::cin, query)) {
            std::cout << "\n" << Color::DIM << "  EOF received. Goodbye!\n"
                      << Color::RESET;
            break;
        }

        
        size_t start = query.find_first_not_of(" \t\r\n");
        size_t end   = query.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        query = query.substr(start, end - start + 1);

        
        if (query == ":quit" || query == ":exit" || query == ":q") {
            std::cout << "\n" << Color::CYAN
                      << "  Thank you for using Custom Search Engine. Goodbye!\n"
                      << Color::RESET << "\n";
            break;
        }
        if (query == ":help" || query == ":h") {
            printHelp();
            continue;
        }
        if (query == ":stats" || query == ":s") {
            printStats(engine.getStats());
            continue;
        }

        
        if (query.substr(0, 8) == ":explain") {
            std::string explainQuery = query.size() > 9 ? query.substr(9) : "";
            
            size_t s = explainQuery.find_first_not_of(" \t");
            if (s != std::string::npos) explainQuery = explainQuery.substr(s);
            handleExplain(engine, explainQuery);
            continue;
        }

        
        if (query.substr(0, 4) == ":top") {
            std::string topTerm = query.size() > 5 ? query.substr(5) : "";
            size_t s = topTerm.find_first_not_of(" \t");
            if (s != std::string::npos) topTerm = topTerm.substr(s);
            
            size_t e = topTerm.find_last_not_of(" \t");
            if (e != std::string::npos) topTerm = topTerm.substr(0, e + 1);
            handleTop(engine, topTerm);
            continue;
        }

        
        if (query.substr(0, 8) == ":similar") {
            std::string simId = query.size() > 9 ? query.substr(9) : "";
            size_t s = simId.find_first_not_of(" \t");
            if (s != std::string::npos) simId = simId.substr(s);
            size_t e = simId.find_last_not_of(" \t");
            if (e != std::string::npos) simId = simId.substr(0, e + 1);
            handleSimilar(engine, simId);
            continue;
        }

        
        if (query[0] == ':') {
            std::cout << Color::RED << "  Unknown command: " << query
                      << Color::RESET << "\n";
            std::cout << Color::DIM << "  Type :help for available commands.\n"
                      << Color::RESET;
            continue;
        }

        
        auto startTime = std::chrono::high_resolution_clock::now();

        ParsedQuery parsed = engine.parseQuery(query);
        std::vector<SearchResult> results = engine.search(query);

        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(
            endTime - startTime
        ).count();

        
        printResults(results, elapsedMs, parsed);
    }

    return 0;
}
