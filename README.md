# Custom Search Engine

A high-performance search engine built from scratch in **C++17**, featuring **BM25 ranking**, **phrase search**, **Porter stemming**, and **KWIC snippet highlighting**.

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    CLI (main.cpp)                       │
│         Interactive REPL • Phrase Search • KWIC         │
├─────────────────────────────────────────────────────────┤
│              SearchEngine (Facade)                      │
│     Query Parser • Phrase Detection • Orchestration     │
├──────────┬──────────────────┬──────────┬────────────────┤
│Tokenizer │  InvertedIndex   │BM25Ranker│    Stemmer     │
│ 5-stage  │ Postings + TF +  │ BM25 +   │ Porter Algo    │
│   NLP    │ Positional Index │ TF-IDF   │ (5-step)       │
├──────────┴──────────────────┴──────────┴────────────────┤
│              Document / Corpus                          │
│        10 diverse tech documents (Wikipedia)            │
└─────────────────────────────────────────────────────────┘
```

## Features

### BM25 Ranking (Industry Standard)
The engine uses **Okapi BM25**, the same ranking algorithm used by Elasticsearch, Apache Lucene/Solr, and most production search engines. BM25 improves on basic TF-IDF with:
- **Term frequency saturation** (k1=1.2): Prevents a term appearing 100× from dominating one appearing 10×
- **Length normalization** (b=0.75): Normalizes relative to average document length

```
IDF(t)      = log((N - df + 0.5) / (df + 0.5) + 1)     — Smoothed IDF
Score(t, d) = IDF(t) × [f(t,d) × (k1+1)] / [f(t,d) + k1 × (1-b+b×|d|/avgdl)]
Score(q, d) = Σ Score(t, d)  for each term t ∈ query q
```

### Phrase Search
Wrap terms in double quotes for exact phrase matching using the positional index:
```
> "machine learning"          — exact phrase match
> "deep learning" optimization  — phrase + individual term
```

### Porter Stemmer
The 5-stage text processing pipeline includes the Porter Stemming Algorithm, enabling morphological matching:
- `computing`, `computer`, `computation` → all match via stem `comput`
- `running`, `runner`, `runs` → all match via stem `run`

### KWIC Snippets
Search results show **Keyword In Context** snippets — the most relevant passage from each document with query terms **highlighted** in bold yellow.

### Advanced CLI Commands
| Command | Description |
|---------|-------------|
| `:help` | Show help information |
| `:stats` | Display index statistics |
| `:explain <query>` | Show BM25 scoring breakdown per term |
| `:top <term>` | Top documents for a single term with TF stats |
| `:similar <docId>` | Find similar documents via cosine similarity |
| `:quit` | Exit the engine |

## Key Data Structures

| Structure | STL Container | Purpose | Lookup |
|-----------|--------------|---------|--------|
| Postings List | `unordered_map<string, set<size_t>>` | Term → Document IDs | O(1) avg |
| Term Frequency | `unordered_map<string, unordered_map<size_t, size_t>>` | Term × Doc → Count | O(1) avg |
| Positional Index | `unordered_map<string, unordered_map<size_t, vector<size_t>>>` | Term × Doc → Positions | O(1) avg |
| Stop Words | `unordered_set<string>` | High-frequency word filter | O(1) avg |
| Doc Tokens | `unordered_map<size_t, size_t>` | Doc → Total token count | O(1) avg |

## Text Processing Pipeline

```
Raw Text → lowercase → remove punctuation → tokenize → remove stop words → Porter stem
```

## Build & Run

### Option 1: Direct Compilation (Recommended)

```bash
# Using g++ (GCC)
g++ -std=c++17 -O2 -Wall -Wextra -I include src/*.cpp -o search_engine
./search_engine

# Using clang++
clang++ -std=c++17 -O2 -Wall -Wextra -I include src/*.cpp -o search_engine
./search_engine

# With custom corpus directory
./search_engine /path/to/corpus
```

### Option 2: Using MSVC (Visual Studio on Windows)

```powershell
cl /std:c++17 /EHsc /O2 /I include src\*.cpp /Fe:search_engine.exe
.\search_engine.exe
```

### Option 3: CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
./search_engine
```

## Usage

Once running, type any search query at the `>` prompt:

```
  > machine learning neural networks
  > "quantum computing" algorithms
  > cryptography security
  > "deep learning" optimization
```

## File Structure

```
Custom_Search_Engine/
├── include/
│   ├── document.hpp          # Document model + corpus factory
│   ├── tokenizer.hpp         # 5-stage text processing pipeline
│   ├── inverted_index.hpp    # Core inverted index + positional index
│   ├── tfidf_ranker.hpp      # BM25 + TF-IDF ranking
│   ├── search_engine.hpp     # Orchestrator facade + query parser
│   └── stemmer.hpp           # Porter Stemming Algorithm
├── src/
│   ├── document.cpp          # File-based corpus loader
│   ├── tokenizer.cpp         # Tokenization, normalization, stemming
│   ├── inverted_index.cpp    # Postings + positional index management
│   ├── tfidf_ranker.cpp      # BM25 scoring implementation
│   ├── search_engine.cpp     # Pipeline + KWIC snippets + phrase search
│   ├── stemmer.cpp           # Porter Stemmer (5-step algorithm)
│   └── main.cpp              # CLI with ANSI colors, timing, commands
├── corpus/                   # 10 Wikipedia articles (scraped)
├── scraper/
│   ├── scraper.py            # Wikipedia article scraper
│   └── requirements.txt      # Python dependencies
├── CMakeLists.txt            # CMake build configuration
└── README.md                 # This file
```

## Technical Highlights

- **BM25 ranking**: Industry-standard algorithm with tunable k1/b parameters
- **Phrase search**: Positional index enables exact phrase matching
- **Porter stemming**: Morphological normalization for better recall
- **KWIC snippets**: Context-aware, highlighted search result previews
- **Score explanation**: Full BM25 breakdown via `:explain` command
- **Document similarity**: Cosine similarity over TF-IDF vectors via `:similar`
- **Zero memory leaks**: RAII throughout — no raw `new`/`delete`
- **Move semantics**: `std::move` used for efficient string transfers
- **O(1) lookups**: `unordered_map`/`unordered_set` for all hot-path operations
- **Sorted postings**: `std::set` enables O(n+m) set intersection
- **Facade pattern**: Clean API boundary between CLI and engine internals
