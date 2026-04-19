#ifndef PHOTO_REPOSITORY_H
#define PHOTO_REPOSITORY_H

#include "Photo.h"
#include "Logger_APP.h"
#include <QMap>
#include <QList>
#include <memory>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include "MetricsCollector_APP.h"

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
        MetricsCollector::instance().incrementCounter("photos.saved");
        saveRepo(); // Persist changes
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
        saveRepo(); // Persist changes
    }

private:
    PhotoRepository() {
        loadRepo();
        
        // Only add demo data if repo is empty
        if (photos_.isEmpty()) {
            // Create demo photos for testing
            auto demoPhoto1 = std::make_shared<Photo>();
            demoPhoto1->setPhotoId("DEMO_001");
            demoPhoto1->setFilename("sample_landscape.jpg");
            demoPhoto1->setDescription("A beautiful landscape demo photo");
            demoPhoto1->setHashtags({"nature", "demo"});
            demoPhoto1->setAuthorId("ADMIN_001");
            demoPhoto1->setAuthorName("admin");
            demoPhoto1->setFileSize(1024 * 500);
            photos_[demoPhoto1->getPhotoId()] = demoPhoto1;

            // Demo photo for guest/anonymous users
            auto guestPhoto = std::make_shared<Photo>();
            guestPhoto->setPhotoId("DEMO_GUEST_001");
            guestPhoto->setFilename("guest_sample.jpg");
            guestPhoto->setDescription("Sample photo for guest users");
            guestPhoto->setHashtags({"guest", "demo"});
            guestPhoto->setAuthorId("ANON_DEMO");
            guestPhoto->setAuthorName("Anonymous");
            guestPhoto->setFileSize(1024 * 300);
            photos_[guestPhoto->getPhotoId()] = guestPhoto;

            Logger::getInstance().log("PhotoRepository", "Demo photos initialized");
            saveRepo();
        }
    }
    
    void loadRepo() {
        QFile file("photos.json");
        if (!file.open(QIODevice::ReadOnly)) {
            Logger::getInstance().log("PhotoRepository", "No existing repo file found");
            return;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        
        for (const auto& val : array) {
            auto photo = Photo::fromJson(val.toObject());
            photos_[photo->getPhotoId()] = photo;
        }
        
        Logger::getInstance().log("PhotoRepository", "Repository loaded from disk. Photos: " + QString::number(photos_.size()));
    }
    
    void saveRepo() {
        QJsonArray array;
        for (const auto& photo : photos_) {
            array.append(photo->toJson());
        }
        
        QJsonDocument doc(array);
        QFile file("photos.json");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            Logger::getInstance().log("PhotoRepository", "Repository saved to disk");
        } else {
            Logger::getInstance().log("PhotoRepository", "Failed to save repository to disk");
        }
    }

    QMap<QString, std::shared_ptr<Photo>> photos_;
};

#endif // PHOTO_REPOSITORY_H