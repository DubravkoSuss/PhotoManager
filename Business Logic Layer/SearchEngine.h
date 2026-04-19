#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "PhotoRepository.h"
#include "PhotoSearchCriteria.h"
#include <memory>

class PhotoIterator {
public:
    PhotoIterator(const QList<std::shared_ptr<Photo>>& items) : items_(items), index_(0) {}
    bool hasNext() const { return index_ < items_.size(); }
    std::shared_ptr<Photo> next() { return hasNext() ? items_.at(index_++) : nullptr; }
private:
    QList<std::shared_ptr<Photo>> items_;
    int index_;
};

// SOLID: Single Responsibility Principle (SRP)
// SearchEngine has the single responsibility of executing search logic.
// It delegates traversal to PhotoIterator.
class SearchEngine {
public:
    static PhotoIterator createIterator(const PhotoSearchCriteria& criteria) {
        auto all = PhotoRepository::getInstance().findAll();
        QList<std::shared_ptr<Photo>> filtered;
        for (auto& p : all) if (criteria.matches(p)) filtered.append(p);
        return PhotoIterator(filtered);
    }
};

#endif // SEARCH_ENGINE_H
