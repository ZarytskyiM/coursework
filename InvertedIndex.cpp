#include "InvertedIndex.h"
#include <sstream>
#include <algorithm>
#include <iostream>

void InvertedIndex::add_document(int doc_id, const std::string& content) {
    std::istringstream iss(content);
    std::string word;
    std::unordered_map<std::string, bool> unique_words;

    while (iss >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return std::ispunct(c); }), word.end());
        std::unique_lock<std::shared_mutex> lock(rw_lock);
        if (!unique_words[word]) {
            index[word].push_back(doc_id);
            unique_words[word] = true;
        }
    }
}

std::vector<int> InvertedIndex::search(const std::string& word) const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = index.find(word);
    if (it != index.end()) {
        return it->second;
    }
    return {};
}

