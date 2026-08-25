let searchEngine = null;

// The Module object is used by Emscripten
var Module = {
    onRuntimeInitialized: function() {
        console.log("Wasm Module Loaded!");
        
        try {
            searchEngine = Module.create_engine();
            
            // Get stats to populate the UI
            const stats = searchEngine.getStats();
            
            // Redirect console.error to UI for debugging
            const debugEl = document.createElement('div');
            debugEl.style.color = '#ef4444';
            debugEl.style.position = 'fixed';
            debugEl.style.bottom = '10px';
            debugEl.style.left = '10px';
            document.body.appendChild(debugEl);
            
            const origErr = console.error;
            console.error = function(...args) {
                debugEl.innerHTML += args.join(' ') + '<br>';
                origErr.apply(console, args);
            };
            
            const origLog = console.log;
            console.log = function(...args) {
                if(args.join(' ').includes("Loaded")) {
                    debugEl.innerHTML += args.join(' ') + '<br>';
                }
                origLog.apply(console, args);
            };
            
            // Populate stats
            document.getElementById('statDocs').textContent = stats.totalDocuments.toLocaleString();
            document.getElementById('statVocab').textContent = stats.vocabularySize.toLocaleString();
            document.getElementById('statTokens').textContent = stats.totalTokensIndexed.toLocaleString();
            
            document.getElementById('statsPanel').classList.add('visible');
            
            // Hide the init overlay
            const overlay = document.getElementById('initOverlay');
            overlay.style.opacity = '0';
            setTimeout(() => {
                overlay.style.display = 'none';
                document.getElementById('searchInput').focus();
            }, 500);
            
        } catch (e) {
            console.error("Failed to initialize engine:", e);
            document.querySelector('.init-content').innerHTML = `
                <h2 style="color: #ef4444">Initialization Failed</h2>
                <p>Could not load the search engine. Check the console.</p>
            `;
        }
    },
    // Locate the .wasm and .data files correctly
    locateFile: function(path, prefix) {
        if (path.endsWith('.data')) return prefix + path;
        return prefix + path;
    }
};

document.addEventListener('DOMContentLoaded', () => {
    const searchInput = document.getElementById('searchInput');
    const resultsList = document.getElementById('resultsList');
    const emptyState = document.getElementById('emptyState');
    const resultsMeta = document.getElementById('resultsMeta');
    const resultCount = document.getElementById('resultCount');
    const searchTime = document.getElementById('searchTime');
    
    // Auto-focus search on slash
    document.addEventListener('keydown', (e) => {
        if (e.key === '/' && document.activeElement !== searchInput) {
            e.preventDefault();
            searchInput.focus();
        }
    });

    // Debounce timer
    let debounceTimer;

    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.trim();
        
        clearTimeout(debounceTimer);
        
        if (query === '') {
            resultsList.innerHTML = '';
            emptyState.classList.remove('hidden');
            resultsMeta.classList.add('hidden');
            return;
        }

        emptyState.classList.add('hidden');

        debounceTimer = setTimeout(() => {
            performSearch(query);
        }, 150); // Fast 150ms debounce for near-instant feel
    });

    function performSearch(query) {
        if (!searchEngine) return;

        const t0 = performance.now();
        
        try {
            // Call into C++ via Embind
            const results = searchEngine.search(query);
            
            const t1 = performance.now();
            const timeMs = (t1 - t0).toFixed(2);
            
            displayResults(results, timeMs);
            
            // Clean up the vector to avoid memory leaks in the Wasm heap
            results.delete();
            
        } catch (error) {
            console.error("Search error:", error);
        }
    }

    function displayResults(resultsVector, timeMs) {
        resultsList.innerHTML = '';
        
        const count = resultsVector.size();
        
        if (count === 0) {
            resultsMeta.classList.add('hidden');
            resultsList.innerHTML = `
                <div class="empty-state" style="padding: 2rem 0">
                    <p>No results found.</p>
                </div>
            `;
            return;
        }

        resultCount.textContent = count;
        searchTime.textContent = timeMs;
        resultsMeta.classList.remove('hidden');

        let html = '';
        
        // Iterate through the C++ std::vector exposed by Embind
        for (let i = 0; i < count; i++) {
            const res = resultsVector.get(i);
            
            // Add animation delay based on index for a cascading fade-in effect
            const delay = i * 0.05;
            
            html += `
                <div class="result-card" style="animation-delay: ${delay}s">
                    <div class="result-header">
                        <div class="result-title">${res.title}</div>
                        <div class="result-score">BM25: ${res.score.toFixed(4)}</div>
                    </div>
                    <div class="result-snippet">${res.snippet}</div>
                </div>
            `;
        }
        
        resultsList.innerHTML = html;
    }
});
