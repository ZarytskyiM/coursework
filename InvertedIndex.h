#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <unordered_map>
#include <vector>
#include <string>
#include <shared_mutex>

class InvertedIndex {
private:
    std::unordered_map<std::string, std::vector<int>> index;
    mutable std::shared_mutex rw_lock;

public:
    void add_document(int doc_id, const std::string& content);
    std::vector<int> search(const std::string& word) const;
    void print_index() const;
};

#endif
