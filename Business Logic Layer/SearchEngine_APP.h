#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "PhotoRepository_APP.h"
#include "PhotoSearchCriteria_APP.h"
#include <memory>
#include <algorithm>

class PhotoIterator {
public:
    PhotoIterator(const QList<std::shared_ptr<Photo>>& items) : items_(items), index_(0) {}
    bool hasNext() const { return index_ < items_.size(); }
    std::shared_ptr<Photo> next() { return hasNext() ? items_.at(index_++) : nullptr; }
private:
    QList<std::shared_ptr<Photo>> items_;
    int index_;
};

class SearchEngine {
public:
    static PhotoIterator createIterator(const PhotoSearchCriteria& criteria) {
        auto all = PhotoRepository::getInstance().findAll();
        QList<std::shared_ptr<Photo>> filtered;
        std::copy_if(all.cbegin(), all.cend(), std::back_inserter(filtered),
                     [&criteria](const std::shared_ptr<Photo>& p) { return criteria.matches(p); });
        return PhotoIterator(filtered);
    }
};

#endif // SEARCH_ENGINE_H
