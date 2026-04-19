#ifndef PHOTO_CONTENT_PROXY_H
#define PHOTO_CONTENT_PROXY_H

#include "Photo.h"
#include "StorageStrategy_APP.h"
#include <QImage>

class ImageContent {
public:
    virtual ~ImageContent() = default;
    virtual QImage getImage() = 0;
};

class RealPhotoContent : public ImageContent {
public:
    RealPhotoContent(const QString& path) : path_(path) {}
    QImage getImage() override { return QImage(path_); }
private:
    QString path_;
};

class ProxyPhotoContent : public ImageContent {
public:
    ProxyPhotoContent(std::shared_ptr<Photo> photo, std::shared_ptr<User> user)
        : photo_(photo), user_(user) {}
    QImage getImage() override {
        if (!photo_) return QImage();
        if (!(user_->getUserType() == UserType::ADMINISTRATOR || user_->getUserId() == photo_->getAuthorId())) return QImage();
        if (!real_) real_ = std::make_shared<RealPhotoContent>(photo_->getStoragePath());
        return real_->getImage();
    }
private:
    std::shared_ptr<Photo> photo_;
    std::shared_ptr<User> user_;
    std::shared_ptr<RealPhotoContent> real_;
};

#endif // PHOTO_CONTENT_PROXY_H
