const Module = require('./search_engine.js');

Module.onRuntimeInitialized = function() {
    console.log("Wasm Module Loaded!");
    
    // Check FS
    try {
        console.log("Reading root:");
        console.log(FS.readdir('/'));
        
        console.log("Reading corpus:");
        console.log(FS.readdir('corpus'));
    } catch(e) {
        console.log("Error reading directory:", e);
    }
    
    const engine = Module.create_engine();
    const stats = engine.getStats();
    console.log("Stats:", stats.totalDocuments, stats.vocabularySize);
};
