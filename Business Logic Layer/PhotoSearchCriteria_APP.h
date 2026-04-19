#ifndef PHOTO_SEARCH_CRITERIA_H
#define PHOTO_SEARCH_CRITERIA_H

#include "Photo.h"
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <algorithm>

/**
 * Specification Pattern for Search Criteria
 */
class PhotoSearchCriteria {
public:
    PhotoSearchCriteria() : minSize_(0), maxSize_(0) {}

    bool matches(std::shared_ptr<Photo> photo) const {
        // Check hashtags
        if (!hashtags_.isEmpty()) {
            const auto& photoTags = photo->getHashtags();
            bool hasTag = std::any_of(hashtags_.cbegin(), hashtags_.cend(),
                [&photoTags](const QString& tag) { return photoTags.contains(tag); });
            if (!hasTag) return false;
        }

        // Check size range
        if (minSize_ > 0 && photo->getFileSize() < minSize_) {
            return false;
        }
        if (maxSize_ > 0 && photo->getFileSize() > maxSize_) {
            return false;
        }

        // Check date range
        if (startDate_.isValid() && photo->getUploadDateTime() < startDate_) {
            return false;
        }
        if (endDate_.isValid() && photo->getUploadDateTime() > endDate_) {
            return false;
        }

        // Check author
        if (!author_.isEmpty() && !photo->getAuthorName().contains(author_, Qt::CaseInsensitive)) {
            return false;
        }

        return true;
    }

    // Setters
    void setHashtags(const QStringList& tags) { hashtags_ = tags; }
    void setMinSize(qint64 size) { minSize_ = size; }
    void setMaxSize(qint64 size) { maxSize_ = size; }
    void setStartDate(const QDateTime& date) { startDate_ = date; }
    void setEndDate(const QDateTime& date) { endDate_ = date; }
    void setAuthor(const QString& author) { author_ = author; }

private:
    QStringList hashtags_;
    qint64 minSize_;
    qint64 maxSize_;
    QDateTime startDate_;
    QDateTime endDate_;
    QString author_;
};

#endif // PHOTO_SEARCH_CRITERIA_H