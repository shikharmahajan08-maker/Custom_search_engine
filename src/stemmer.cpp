















#include "stemmer.hpp"

#include <algorithm>  
#include <string>




namespace {








bool isConsonant(const std::string& word, size_t i) {
    if (i >= word.size()) return false;
    char c = word[i];
    switch (c) {
        case 'a': case 'e': case 'i': case 'o': case 'u':
            return false;
        case 'y':
            
            return (i == 0) || !isConsonant(word, i - 1);
        default:
            return true;
    }
}













int measure(const std::string& stem) {
    int m = 0;
    size_t i = 0;
    size_t len = stem.size();

    
    while (i < len && isConsonant(stem, i)) i++;

    while (i < len) {
        
        while (i < len && !isConsonant(stem, i)) i++;
        if (i >= len) break;

        
        while (i < len && isConsonant(stem, i)) i++;
        m++;
    }

    return m;
}




bool containsVowel(const std::string& stem) {
    for (size_t i = 0; i < stem.size(); i++) {
        if (!isConsonant(stem, i)) return true;
    }
    return false;
}





bool endsWithDoubleConsonant(const std::string& word) {
    size_t len = word.size();
    if (len < 2) return false;
    return (word[len - 1] == word[len - 2]) &&
           isConsonant(word, len - 1);
}








bool endsCVC(const std::string& word) {
    size_t len = word.size();
    if (len < 3) return false;

    char last = word[len - 1];
    if (last == 'w' || last == 'x' || last == 'y') return false;

    return isConsonant(word, len - 1) &&
          !isConsonant(word, len - 2) &&
           isConsonant(word, len - 3);
}




bool endsWith(const std::string& word, const std::string& suffix) {
    if (suffix.size() > word.size()) return false;
    return word.compare(word.size() - suffix.size(), suffix.size(), suffix) == 0;
}




std::string replaceSuffix(const std::string& word, size_t suffixLen,
                          const std::string& replacement) {
    return word.substr(0, word.size() - suffixLen) + replacement;
}

} 





std::string stem(const std::string& word) {
    
    if (word.size() < 3) return word;

    std::string w = word;

    
    
    
    if (endsWith(w, "sses")) {
        w = replaceSuffix(w, 4, "ss");       
    } else if (endsWith(w, "ies")) {
        w = replaceSuffix(w, 3, "i");        
    } else if (!endsWith(w, "ss") && endsWith(w, "s")) {
        w = replaceSuffix(w, 1, "");         
    }

    
    
    
    if (endsWith(w, "eed")) {
        std::string stem_part = w.substr(0, w.size() - 3);
        if (measure(stem_part) > 0) {
            w = replaceSuffix(w, 3, "ee");   
        }
        
    } else if (endsWith(w, "ed")) {
        std::string stem_part = w.substr(0, w.size() - 2);
        if (containsVowel(stem_part)) {
            w = stem_part;                   
            
            if (endsWith(w, "at") || endsWith(w, "bl") || endsWith(w, "iz")) {
                w += "e";                    
            } else if (endsWithDoubleConsonant(w) &&
                       w.back() != 'l' && w.back() != 's' && w.back() != 'z') {
                w.pop_back();                
            } else if (measure(w) == 1 && endsCVC(w)) {
                w += "e";                    
            }
        }
    } else if (endsWith(w, "ing")) {
        std::string stem_part = w.substr(0, w.size() - 3);
        if (containsVowel(stem_part)) {
            w = stem_part;                   
            
            if (endsWith(w, "at") || endsWith(w, "bl") || endsWith(w, "iz")) {
                w += "e";
            } else if (endsWithDoubleConsonant(w) &&
                       w.back() != 'l' && w.back() != 's' && w.back() != 'z') {
                w.pop_back();
            } else if (measure(w) == 1 && endsCVC(w)) {
                w += "e";
            }
        }
    }

    
    
    
    if (endsWith(w, "y")) {
        std::string stem_part = w.substr(0, w.size() - 1);
        if (containsVowel(stem_part)) {
            w = stem_part + "i";             
        }
    }

    
    
    
    
    
    auto step2replace = [&](const std::string& suffix, const std::string& repl) -> bool {
        if (endsWith(w, suffix)) {
            std::string stem_part = w.substr(0, w.size() - suffix.size());
            if (measure(stem_part) > 0) {
                w = stem_part + repl;
            }
            return true;
        }
        return false;
    };

    
    if (!step2replace("ational", "ate"))    
    if (!step2replace("tional", "tion"))    
    if (!step2replace("enci", "ence"))      
    if (!step2replace("anci", "ance"))      
    if (!step2replace("izer", "ize"))       
    if (!step2replace("abli", "able"))      
    if (!step2replace("alli", "al"))        
    if (!step2replace("entli", "ent"))      
    if (!step2replace("eli", "e"))          
    if (!step2replace("ousli", "ous"))      
    if (!step2replace("ization", "ize"))    
    if (!step2replace("ation", "ate"))      
    if (!step2replace("ator", "ate"))       
    if (!step2replace("alism", "al"))       
    if (!step2replace("iveness", "ive"))    
    if (!step2replace("fulness", "ful"))    
    if (!step2replace("ousness", "ous"))    
    if (!step2replace("aliti", "al"))       
    if (!step2replace("iviti", "ive"))      
    step2replace("biliti", "ble");          

    
    
    
    auto step3replace = [&](const std::string& suffix, const std::string& repl) -> bool {
        if (endsWith(w, suffix)) {
            std::string stem_part = w.substr(0, w.size() - suffix.size());
            if (measure(stem_part) > 0) {
                w = stem_part + repl;
            }
            return true;
        }
        return false;
    };

    if (!step3replace("icate", "ic"))       
    if (!step3replace("ative", ""))         
    if (!step3replace("alize", "al"))       
    if (!step3replace("iciti", "ic"))       
    if (!step3replace("ical", "ic"))        
    if (!step3replace("ful", ""))           
    step3replace("ness", "");               

    
    
    
    
    
    
    auto step4replace = [&](const std::string& suffix) -> bool {
        if (endsWith(w, suffix)) {
            std::string stem_part = w.substr(0, w.size() - suffix.size());
            if (measure(stem_part) > 1) {
                w = stem_part;
            }
            return true;
        }
        return false;
    };

    if (!step4replace("al"))
    if (!step4replace("ance"))
    if (!step4replace("ence"))
    if (!step4replace("er"))
    if (!step4replace("ic"))
    if (!step4replace("able"))
    if (!step4replace("ible"))
    if (!step4replace("ant"))
    if (!step4replace("ement"))
    if (!step4replace("ment"))
    if (!step4replace("ent")) {
        
        if (endsWith(w, "ion")) {
            std::string stem_part = w.substr(0, w.size() - 3);
            if (measure(stem_part) > 1 && !stem_part.empty() &&
                (stem_part.back() == 's' || stem_part.back() == 't')) {
                w = stem_part;
            }
        } else {
            if (!step4replace("ou"))
            if (!step4replace("ism"))
            if (!step4replace("ate"))
            if (!step4replace("iti"))
            if (!step4replace("ous"))
            if (!step4replace("ive"))
            step4replace("ize");
        }
    }

    
    
    
    
    if (endsWith(w, "e")) {
        std::string stem_part = w.substr(0, w.size() - 1);
        int m = measure(stem_part);
        if (m > 1 || (m == 1 && !endsCVC(stem_part))) {
            w = stem_part;
        }
    }

    
    
    
    if (endsWithDoubleConsonant(w) && w.back() == 'l') {
        if (measure(w.substr(0, w.size() - 1)) > 1) {
            w.pop_back();                    
        }
    }

    return w;
}
