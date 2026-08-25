# Activate Emscripten environment
C:\Users\"Shikhar Mahajan"\emsdk\emsdk_env.ps1

# Define source files (excluding main.cpp)
$SOURCES = "src/document.cpp src/tokenizer.cpp src/inverted_index.cpp src/tfidf_ranker.cpp src/search_engine.cpp src/stemmer.cpp src/wasm_bindings.cpp"

# Run emcc to compile C++ to WebAssembly
Write-Host "Compiling C++ to WebAssembly..." -ForegroundColor Cyan

em++ -std=c++17 -O3 $SOURCES.Split(" ") -I include --bind -s ALLOW_MEMORY_GROWTH=1 -s EXPORT_ES6=0 -s ENVIRONMENT=web --preload-file corpus -o docs/search_engine.js

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build Successful!" -ForegroundColor Green
    Write-Host "Files generated in docs/ directory:"
    Get-ChildItem docs/search_engine.* | Select-Object Name, @{Name="Size(MB)";Expression={"{0:N2}" -f ($_.Length/1MB)}}
} else {
    Write-Host "Build Failed!" -ForegroundColor Red
}
