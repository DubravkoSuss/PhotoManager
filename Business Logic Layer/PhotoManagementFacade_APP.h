#ifndef PHOTO_MANAGEMENT_FACADE_H
#define PHOTO_MANAGEMENT_FACADE_H

#include "User.h"
#include "Photo.h"
#include "PhotoRepository_APP.h"
#include "UserRepository_APP.h"
#include "StorageStrategy_APP.h"
#include "ImageProcessor_APP.h"
#include "ChainOfResponsability.h"
#include "PhotoSearchCriteria_APP.h"
#include "SearchEngine_APP.h"
#include "Logger_APP.h"
#include "EditHistory.h"
#include "PhotoContentProxy_APP.h"
#include "Album.h"
#include <QImage>
#include <QString>
#include <QFileInfo>
#include <memory>


/**
 * Facade Pattern - Simplifies complex business logic
 */

class PhotoManagementFacade {
public:

    PhotoManagementFacade();

    ~PhotoManagementFacade();

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

    void downloadPhoto(const QString& photoId, const QString& savePath);

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
