



















#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <cstddef>   
#include <string>    
#include <vector>    








struct Document {
    size_t      id;       
    std::string title;    
    std::string content;  

    
    
    
    Document(size_t id, std::string title, std::string content);
};











namespace Corpus {

    
    
    
    
    
    
    
    
    
    std::vector<Document> loadFromDirectory(const std::string& dirPath);

    
    
    
    
    
    
    
    
    
    
    Document loadFromFile(const std::string& filePath, size_t docId);

} 

#endif 
