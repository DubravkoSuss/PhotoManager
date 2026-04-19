#include "PhotoManagementFacade.h"

#include "User.h"
#include "Photo.h"
#include "PhotoRepository.h"
#include "UserRepository.h"
#include "StorageStrategy.h"
#include "ImageProcessor.h"
#include "PhotoSearchCriteria.h"
#include "Logger.h"


#include <QImage>
#include <QFileInfo>
#include <QDateTime>

// Constructor implementation
// Creating the LocalStorageStrategy requires its full definition
PhotoManagementFacade::PhotoManagementFacade()
    : storageStrategy_(std::make_unique<LocalStorageStrategy>()) {
}

PhotoManagementFacade::~PhotoManagementFacade() = default;

void PhotoManagementFacade::setStorageStrategy(std::unique_ptr<StorageStrategy> strategy) {
    storageStrategy_ = std::move(strategy);
}

std::shared_ptr<Photo> PhotoManagementFacade::uploadPhoto(
    std::shared_ptr<User> user,
    const QString& filePath,
    const QString& description,
    const QStringList& hashtags,
    std::shared_ptr<ImageProcessor> processor) {

    // Check subscription limits
    if (!checkUploadLimits(user, filePath)) {
        Logger::getInstance().log(user->getUserId(), "Upload failed: Limit exceeded");
        return nullptr;
    }

    try {
        // Load and process image
        QImage image(filePath);
        if (image.isNull()) {
            Logger::getInstance().log(user->getUserId(), "Upload failed: Invalid image");
            return nullptr;
        }

        QImage processed = processor->process(image);

        // Upload to storage
        QString storagePath = storageStrategy_->upload(filePath, user->getUserId());

        // Create photo entity
        auto photo = std::make_shared<Photo>();
        photo->setPhotoId(QString("PHOTO_%1").arg(QDateTime::currentMSecsSinceEpoch()));
        photo->setFilename(QFileInfo(filePath).fileName());
        photo->setDescription(description);
        photo->setHashtags(hashtags);
        photo->setAuthorId(user->getUserId());
        photo->setAuthorName(user->getUsername());
        photo->setFileSize(QFileInfo(filePath).size());
        photo->setStoragePath(storagePath);
        photo->setWidth(processed.width());
        photo->setHeight(processed.height());
        photo->setState(Photo::State::PUBLISHED);

        // Save to repository
        PhotoRepository::getInstance().save(photo);

        Logger::getInstance().log(user->getUserId(), "Photo uploaded: " + photo->getFilename());
        return photo;

    } catch (...) {
        Logger::getInstance().log(user->getUserId(), "Upload failed: Exception");
        return nullptr;
    }
}

QList<std::shared_ptr<Photo>> PhotoManagementFacade::SearchPhotos(
    const PhotoSearchCriteria& criteria) {

    auto allPhotos = PhotoRepository::getInstance().findAll();
    QList<std::shared_ptr<Photo>> filtered;

    for (auto photo : allPhotos) {
        if (criteria.matches(photo)) {
            filtered.append(photo);
        }
    }

    Logger::getInstance().log("System", "Photo search performed");
    return filtered;
}

void PhotoManagementFacade::updatePhoto(
    std::shared_ptr<User> user,
    const QString& photoId,
    const QString& newDescription,
    const QStringList& newHashtags) {

    auto photo = PhotoRepository::getInstance().findById(photoId);
    if (photo && canModify(user, photo)) {
        EditHistory::getInstance().push(photoId, PhotoMemento(photo->getDescription(), photo->getHashtags()));
        photo->setDescription(newDescription);
        photo->setHashtags(newHashtags);
        PhotoRepository::getInstance().save(photo);
        Logger::getInstance().log(user->getUserId(), "Photo updated: " + photoId);
    }
}

void PhotoManagementFacade::deletePhotos(std::shared_ptr<User> user, const QString& photoId) {
    auto photo = PhotoRepository::getInstance().findById(photoId);
    if (photo && canModify(user, photo)) {
        storageStrategy_->remove(photo->getStoragePath());
        PhotoRepository::getInstance().remove(photoId);
        Logger::getInstance().log(user->getUserId(), "Photo deleted: " + photoId);
    }
}

QImage PhotoManagementFacade::getPhotoImage(std::shared_ptr<User> user, const QString& photoId) {
    auto photo = PhotoRepository::getInstance().findById(photoId);
    if (!photo) return QImage();
    ProxyPhotoContent proxy(photo, user);
    return proxy.getImage();
}

// Private helper methods
// These are implementation details completely hidden from header users
bool PhotoManagementFacade::checkUploadLimits(
    std::shared_ptr<User> user,
    const QString& filePath) {

    PackageLimits limits = User::getPackageLimits(user->getSubscriptionPackage());

    // Check file size
    qint64 fileSize = QFileInfo(filePath).size();
    if (fileSize > limits.maxUploadSize) {
        return false;
    }

    // Check total photos limit
    auto userPhotos = PhotoRepository::getInstance().findByAuthor(user->getUserId());
    if (limits.maxTotalPhotos != -1 && userPhotos.size() >= limits.maxTotalPhotos) {
        return false;
    }

    return true;
}

bool PhotoManagementFacade::canModify(
    std::shared_ptr<User> user,
    std::shared_ptr<Photo> photo) {

    return user->getUserType() == UserType::ADMINISTRATOR ||
           user->getUserId() == photo->getAuthorId();
}

void PhotoManagementFacade::createAlbum(const QString& albumName) {
    if (!albums_.contains(albumName)) albums_[albumName] = std::make_shared<AlbumComposite>(albumName);
}

void PhotoManagementFacade::addPhotoToAlbum(const QString& albumName, std::shared_ptr<Photo> photo) {
    if (!albums_.contains(albumName)) createAlbum(albumName);
    albums_[albumName]->add(std::make_shared<PhotoLeaf>(photo));
}

QList<std::shared_ptr<Photo>> PhotoManagementFacade::listAlbumPhotos(const QString& albumName) {
    if (!albums_.contains(albumName)) return {};
    return albums_[albumName]->collectPhotos();
}

void PhotoManagementFacade::undoLastEdit(std::shared_ptr<User> user, const QString& photoId) {
    auto photo = PhotoRepository::getInstance().findById(photoId);
    if (photo && canModify(user, photo) && EditHistory::getInstance().canUndo(photoId)) {
        auto m = EditHistory::getInstance().pop(photoId);
        photo->setDescription(m.description());
        photo->setHashtags(m.hashtags());
        PhotoRepository::getInstance().save(photo);
        Logger::getInstance().log(user->getUserId(), "Undo photo edit: " + photoId);
    }
}

PhotoIterator PhotoManagementFacade::CreateSearchIterator(const PhotoSearchCriteria& criteria) {
    return SearchEngine::createIterator(criteria);
}

std::shared_ptr<Photo> PhotoManagementFacade::uploadPhotoWithPipeline(
    std::shared_ptr<User> user,
    const QString& filePath,
    const QString& description,
    const QStringList& hashtags,
    std::shared_ptr<ImageHandler> pipeline) {

    if (!checkUploadLimits(user, filePath)) {
        Logger::getInstance().log(user->getUserId(), "Upload failed: Limit exceeded");
        return nullptr;
    }

    try {
        QImage image(filePath);
        if (image.isNull()) {
            Logger::getInstance().log(user->getUserId(), "Upload failed: Invalid image");
            return nullptr;
        }

        QImage processed = pipeline ? pipeline->handle(image) : image;

        QString storagePath = storageStrategy_->upload(filePath, user->getUserId());

        auto photo = std::make_shared<Photo>();
        photo->setPhotoId(QString("PHOTO_%1").arg(QDateTime::currentMSecsSinceEpoch()));
        photo->setFilename(QFileInfo(filePath).fileName());
        photo->setDescription(description);
        photo->setHashtags(hashtags);
        photo->setAuthorId(user->getUserId());
        photo->setAuthorName(user->getUsername());
        photo->setFileSize(QFileInfo(filePath).size());
        photo->setStoragePath(storagePath);
        photo->setWidth(processed.width());
        photo->setHeight(processed.height());
        photo->setState(Photo::State::PUBLISHED);

        PhotoRepository::getInstance().save(photo);

        Logger::getInstance().log(user->getUserId(), "Photo uploaded: " + photo->getFilename());
        return photo;

    } catch (...) {
        Logger::getInstance().log(user->getUserId(), "Upload failed: Exception");
        return nullptr;
    }
}
