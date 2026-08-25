













#include "document.hpp"

#include <algorithm>    
#include <fstream>      
#include <iostream>     
#include <sstream>      
#include <utility>      


#ifdef _WIN32
    #include <windows.h>   
#else
    #include <dirent.h>    
    #include <sys/stat.h>  
#endif




Document::Document(size_t id, std::string title, std::string content)
    : id(id)
    , title(std::move(title))
    , content(std::move(content))
{}






static bool endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}










static std::vector<std::string> listTxtFiles(const std::string& dirPath) {
    std::vector<std::string> files;

#ifdef _WIN32
    
    
    
    
    
    
    std::string searchPattern = dirPath + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPattern.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        
        return files;
    }

    do {
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        std::string filename(findData.cFileName);

        
        if (endsWith(filename, ".txt")) {
            
            files.push_back(dirPath + "\\" + filename);
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);

#else
    
    
    
    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        return files;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename(entry->d_name);

        
        if (filename == "." || filename == "..") continue;

        
        std::string fullPath = dirPath + "/" + filename;

        
        struct stat fileStat;
        if (stat(fullPath.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
            
            if (endsWith(filename, ".txt")) {
                files.push_back(fullPath);
            }
        }
    }

    closedir(dir);
#endif

    
    std::sort(files.begin(), files.end());

    return files;
}




static bool directoryExists(const std::string& path) {
#ifdef _WIN32
    DWORD attribs = GetFileAttributesA(path.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES &&
            (attribs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat pathStat;
    return (stat(path.c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode));
#endif
}

















namespace Corpus {

Document loadFromFile(const std::string& filePath, size_t docId) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "  WARNING: Could not open file: " << filePath << "\n";
        return Document(docId, "", "");
    }

    
    std::string title;
    std::getline(file, title);

    
    if (!title.empty() && title.back() == '\r') {
        title.pop_back();
    }

    
    
    
    
    std::ostringstream contentStream;
    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!firstLine) {
            contentStream << ' ';  
        }
        contentStream << line;
        firstLine = false;
    }

    file.close();

    return Document(docId, std::move(title), contentStream.str());
}

















std::vector<Document> loadFromDirectory(const std::string& dirPath) {
    std::vector<Document> documents;

    
    if (!directoryExists(dirPath)) {
        std::cerr << "  ERROR: Corpus directory not found: " << dirPath << "\n";
        std::cerr << "         Run the Python scraper first:\n";
        std::cerr << "           cd scraper && pip install -r requirements.txt && python scraper.py\n";
        return documents;
    }

    
    std::vector<std::string> txtFiles = listTxtFiles(dirPath);

    if (txtFiles.empty()) {
        std::cerr << "  WARNING: No .txt files found in: " << dirPath << "\n";
        std::cerr << "           Run the Python scraper first to populate the corpus.\n";
        return documents;
    }

    
    documents.reserve(txtFiles.size());

    for (size_t i = 0; i < txtFiles.size(); ++i) {
        Document doc = loadFromFile(txtFiles[i], i);

        
        if (!doc.content.empty()) {
            
            std::string displayName = txtFiles[i];
            size_t lastSep = displayName.find_last_of("/\\");
            if (lastSep != std::string::npos) {
                displayName = displayName.substr(lastSep + 1);
            }

            std::cout << "  Loaded: [" << i << "] " << displayName << "\n";
            documents.push_back(std::move(doc));
        }
    }

    return documents;
}

} 
