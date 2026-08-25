#include <emscripten/bind.h>
#include "search_engine.hpp"
#include "document.hpp"

using namespace emscripten;

SearchEngine* create_engine() {
    SearchEngine* engine = new SearchEngine();
    
    std::vector<Document> docs = Corpus::loadFromDirectory("corpus");
    engine->loadCorpus(docs);
    return engine;
}

EMSCRIPTEN_BINDINGS(search_engine_module) {
    value_object<SearchResult>("SearchResult")
        .field("docId", &SearchResult::docId)
        .field("title", &SearchResult::title)
        .field("score", &SearchResult::score)
        .field("snippet", &SearchResult::snippet);

    value_object<IndexStats>("IndexStats")
        .field("totalDocuments", &IndexStats::totalDocuments)
        .field("vocabularySize", &IndexStats::vocabularySize)
        .field("totalTokensIndexed", &IndexStats::totalTokensIndexed)
        .field("avgDocLength", &IndexStats::avgDocLength);

    register_vector<SearchResult>("VectorSearchResult");

    class_<SearchEngine>("SearchEngine")
        .function("search", &SearchEngine::search)
        .function("getStats", &SearchEngine::getStats);

    function("create_engine", &create_engine, allow_raw_pointers());
}
