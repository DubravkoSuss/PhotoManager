#ifndef PHOTO_MANAGEMENT_FACADE_H
#define PHOTO_MANAGEMENT_FACADE_H

#include "User.h"
#include "Photo.h"
#include "PhotoRepository.h"
#include "UserRepository.h"
#include "StorageStrategy.h"
#include "ImageProcessor.h"
#include "ChainOfResponsability.h"
#include "PhotoSearchCriteria.h"
#include "SearchEngine.h"
#include "Logger.h"
#include "EditHistory.h"
#include "PhotoContentProxy.h"
#include "Album.h"
#include <QImage>
#include <QString>
#include <QFileInfo>
#include <memory>


/**
 * Facade Pattern - Simplifies complex business logic
 * 
 * SOLID: Single Responsibility Principle (SRP)
 * Although this class has many methods, its single responsibility is ORCHESTRATION.
 * It coordinates User, Photo, Repositories, and Strategies to perform complex tasks,
 * delegating the actual work to those classes.
 */

class PhotoManagementFacade {
public:

    PhotoManagementFacade();

    ~PhotoManagementFacade();

    // SOLID: Dependency Inversion Principle (DIP)
    // Depends on abstraction (StorageStrategy) rather than concrete details.
    // Allows injection of different strategies at runtime.
    void setStorageStrategy(std::unique_ptr<StorageStrategy> strategy);


    std::shared_ptr<Photo> uploadPhoto(std::shared_ptr<User> user,
                                        const QString& filepath,
                                        const QString & description,
                                        const QStringList& hashtags,
                                        std::shared_ptr<ImageProcessor> processor);

    std::shared_ptr<Photo> uploadPhotoWithPipeline(std::shared_ptr<User> user,
                                        const QString& filepath,
                                        const QString & description,
                                        const QStringList& hashtags,
                                        std::shared_ptr<ImageHandler> pipeline);

    QList<std::shared_ptr<Photo>> SearchPhotos(const PhotoSearchCriteria& criteria);

    PhotoIterator CreateSearchIterator(const PhotoSearchCriteria& criteria);

    void updatePhoto(std::shared_ptr<User> user,
                     const QString& photoId,
                     const QString& newDescription,
                     const QStringList& newHashtags);

    void undoLastEdit(std::shared_ptr<User> user, const QString& photoId);

    void deletePhotos(std::shared_ptr<User> user, const QString& photoId);

    QImage getPhotoImage(std::shared_ptr<User> user, const QString& photoId);

    void createAlbum(const QString& albumName);
    void addPhotoToAlbum(const QString& albumName, std::shared_ptr<Photo> photo);
    QList<std::shared_ptr<Photo>> listAlbumPhotos(const QString& albumName);

private:

    std::unique_ptr<StorageStrategy> storageStrategy_;
    QMap<QString, std::shared_ptr<AlbumComposite>> albums_;

    bool checkUploadLimits(std::shared_ptr<User> user, const QString& filePath);
    bool canModify(std::shared_ptr<User> user, std::shared_ptr<Photo> photo);
};

#endif // PHOTO_MANAGEMENT_FACADE_H
