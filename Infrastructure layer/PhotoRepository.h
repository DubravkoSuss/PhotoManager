#ifndef PHOTO_REPOSITORY_H
#define PHOTO_REPOSITORY_H

#include "Photo.h"
#include "Logger.h"
#include <QMap>
#include <QList>
#include <memory>

/**
 * Repository Pattern + Singleton for Photo data access
 */
class PhotoRepository {
public:
    static PhotoRepository& getInstance() {
        static PhotoRepository instance;
        return instance;
    }

    void save(std::shared_ptr<Photo> photo) {
        photos_[photo->getPhotoId()] = photo;
        Logger::getInstance().log("PhotoRepository", "Photo saved: " + photo->getFilename());
    }

    std::shared_ptr<Photo> findById(const QString& photoId) {
        return photos_.value(photoId, nullptr);
    }

    QList<std::shared_ptr<Photo>> findAll() {
        return photos_.values();
    }

    QList<std::shared_ptr<Photo>> findByAuthor(const QString& authorId) {
        QList<std::shared_ptr<Photo>> result;
        for (auto photo : photos_.values()) {
            if (photo->getAuthorId() == authorId) {
                result.append(photo);
            }
        }
        return result;
    }

    void remove(const QString& photoId) {
        photos_.remove(photoId);
        Logger::getInstance().log("PhotoRepository", "Photo deleted: " + photoId);
    }

    // Delete copy constructor and assignment operator
    PhotoRepository(const PhotoRepository&) = delete;
    PhotoRepository& operator=(const PhotoRepository&) = delete;

private:
    PhotoRepository() = default;
    QMap<QString, std::shared_ptr<Photo>> photos_;
};

#endif // PHOTO_REPOSITORY_H